#include "app.h"
#include "Networking/Packets.h"
#include "context.h"
#include "ui/screens.h"

#include <chrono>
#include <charconv>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <ostream>
#include <thread>
#include <unordered_map>
#include <utility>

App::App(int w, int h) : width(w), height(h) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(width, height, "Vector");
    current_state = state_context::menu;
    rlImGuiSetup(true);
    port = 25565;
    memset(username_buffer, 0, sizeof(username_buffer));
}

App::~App() {
    CloseWindow();
}

bool App::is_running() {
    if (IsWindowResized()) {
        int newWidth = GetScreenWidth();
        int newHeight = GetScreenHeight();
    }
    return !WindowShouldClose();
}

void App::run() {
    dt = GetFrameTime();

    if (accumulator >= TICK_RATE) {
        accumulator = 0;
        run_fixed();
    }

    accumulator += dt;
    net.update();

    switch (current_state) {
        case menu:
            menu_loop();
            break;
        case in_game:
            in_game_loop();
            break;
        case pause_menu:
            pause_loop();
            break;
        case lobby:
            lobby_loop();
            break;
        default:
            break;
    }
}

void App::run_fixed() {
}

void App::menu_loop() {
    ui::render_network_menu(*this);
}

void App::in_game_loop() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
}

void App::pause_loop() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    EndDrawing();
}

void App::lobby_loop() {
    if (!net.isHosting()) {
        start_game_client();
    }

    ui::render_lobby(*this);

    retrieve_ready_packet();
}

void App::start_host_session() {
    if (!net.startHost(port)) {
        std::cerr << "Couldn't host on port " << port << std::endl;
        return;
    }

    std::cout << "Hosting on port " << port << std::endl;
    server = std::make_unique<Server>();
    server->add_connection(username_buffer, 0);
    lobby_players_ready[0] = true;
    send_lobby_snapshot();
    client = std::make_unique<Client>(net.getMyPlayerId(), username_buffer);
    client->toggle_ready();
    current_state = state_context::lobby;
}

bool App::join_server() {
    if (!net.connectToServer(ip, port)) {
        std::cerr << "Couldn't connect!" << std::endl;
        return false;
    }

    std::cout << "Connected!" << std::endl;
    client = std::make_unique<Client>(net.getMyPlayerId(), username_buffer);

    JoinPacket join_packet;
    std::memset(join_packet.username, 0, sizeof(join_packet.username));
    std::strncpy(join_packet.username, username_buffer, sizeof(join_packet.username) - 1);
    net.send(join_packet, true);

    for (int attempt = 0; attempt < 200; ++attempt) {
        net.update();

        if (auto pkt = net.receiveFromServer<JoinSuccess>(static_cast<uint8_t>(PacketType::JOIN_SUCCESS))) {
            std::cout << "Logged in! ID: " << pkt->client_id << std::endl;
            net.assignMyPlayerId(pkt->client_id);
            current_state = state_context::lobby;
            return true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "Couldn't log in!" << std::endl;
    return false;
}

void App::on_player_connected(uint32_t peer_id) {
    if (!server) {
        return;
    }

    std::cout << "Player " << peer_id << " connected!" << std::endl;
    //server->add_connection("unknown", peer_id);
    send_lobby_snapshot();
}

void App::handle_join_request(uint32_t client_id, const std::string& username) {
    if (!net.isHosting() || !server) {
        return;
    }

    if (client_id >= 0) {
        server->add_connection(username, client_id);
    }

    JoinSuccess success;
    success.client_id = client_id;
    net.sendToClient(client_id, success, true);
    send_lobby_snapshot();
}

void App::send_lobby_snapshot() {
    if (!server) return;

    auto connections = server->get_connections();
    LobbySnapshot packet;
    packet.player_count = static_cast<uint32_t>(std::min<size_t>(connections.size(), 32ull));

    uint32_t index = 0;
    for (const auto& [id, username] : connections) {
        if (index >= 32) break;

        packet.ids[index] = id;
        std::memset(packet.usernames[index], 0, sizeof(packet.usernames[index]));
        std::strncpy(packet.usernames[index], username.c_str(), sizeof(packet.usernames[index]) - 1);
        packet.usernames[index][sizeof(packet.usernames[index]) - 1] = '\0';
        packet.is_ready[index] = lobby_players_ready[id];
        ++index;
    }

    net.broadcastToAll(packet, true);
}

std::unordered_map<uint32_t, std::string> App::retrieve_lobby_snapshot() {
    std::unordered_map<uint32_t, std::string> connections;

    while (auto pkt = net.receiveFromServer<LobbySnapshot>(static_cast<uint8_t>(PacketType::LOBBY_SNAPSHOT))) {
        connections.clear();
        for (uint32_t i = 0; i < pkt->player_count; ++i) {
            connections[pkt->ids[i]] = pkt->usernames[i];
            lobby_players_ready[pkt->ids[i]] = pkt->is_ready[i];
        }
    }

    if (!connections.empty()) {
        lobby_players = std::move(connections);
    }

    return lobby_players;
}

void App::send_ready_packet(bool ready) {
    ReadyPacket packet;
    packet.id = net.getMyPlayerId();
    packet.ready = ready;

    net.send(packet, true);
}

void App::retrieve_ready_packet() {
    if (auto result = net.receiveFromClient<ReadyPacket>(static_cast<uint8_t>(PacketType::READY_PACKET))) {
        auto [pkt, client_id] = *result;
        lobby_players_ready[pkt.id] = pkt.ready;
        send_lobby_snapshot();
    }
}

bool App::all_ready() {
    bool ready = true;
    for (auto& player_ready : lobby_players_ready) {
        if (!player_ready.second) {
            ready = false;
        }
    }

    return ready;
}

void App::start_game_host() {
    send_game_start(1);

    current_state = state_context::in_game;
}

void App::send_game_start(uint32_t state) {
    StatePacket packet;
    packet.state = state;

    net.broadcastToAll(packet);
}

void App::start_game_client() {
    if (auto packet = net.receiveFromServer<StatePacket>(static_cast<uint8_t>(PacketType::STATE))) {
        current_state = static_cast<state_context>(packet->state);
    }
}