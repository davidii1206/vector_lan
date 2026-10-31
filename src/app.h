#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "raylib.h"
#include "context.h"
#include "imgui.h"
#include "rlImGui.h"
#include "Networking/NetworkWrapper.h"
#include "Networking/Packets.h"
#include "server.h"
#include "client.h"

constexpr double TICK_RATE = 1.0/128.0;

class App;

namespace ui {
    void render_network_menu(App& app);
    void render_lobby(App& app);
}

class App {
    public:
        App(int w, int h);
        ~App();

        void run();
        void run_fixed();

        void menu_loop();
        void in_game_loop();
        void pause_loop();
        void lobby_loop();

        bool is_running();

        void start_host_session();
        bool join_server();
        void on_player_connected(uint32_t peer_id);
        void handle_join_request(uint32_t client_id, const std::string& username);
        void send_lobby_snapshot();
        std::unordered_map<uint32_t, std::string> retrieve_lobby_snapshot();

        std::unordered_map<uint32_t, std::string> lobby_players;
        std::unordered_map<uint32_t, bool> lobby_players_ready;

        void send_ready_packet(bool ready);
        void retrieve_ready_packet();
        bool all_ready();

        void start_game_host();
        void start_game_client();

        void send_game_start(uint32_t state);

    private:
        friend void ui::render_network_menu(App& app);
        friend void ui::render_lobby(App& app);

        int width;
        int height;

        char ip[128] = "127.0.0.1";
        int port;
        NetLib::GameSession<PacketType> net;
        std::unique_ptr<Server> server;
        std::unique_ptr<Client> client;
        char username_buffer[128];

        double dt;
        double accumulator;

        state_context current_state;
};