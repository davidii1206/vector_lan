#include "screens.h"
#include "imgui.h"

#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>
#include <unordered_map>

namespace ui {

void render_network_menu(App& app) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    rlImGuiBegin();
    ImGui::Begin("Networking");

    ImGui::InputText("Username", app.username_buffer, sizeof(app.username_buffer));

    if (ImGui::Button("Host")) {
        app.start_host_session();
    }

    ImGui::InputText("IP Address", app.ip, IM_ARRAYSIZE(app.ip));

    if (ImGui::Button("Join")) {
        app.join_server();
    }

    ImGui::End();
    rlImGuiEnd();
    EndDrawing();
}

void render_lobby(App& app) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    rlImGuiBegin();
    ImGui::Begin("Lobby");

    uint32_t peer_id;
    bool isConnect;

    if (app.net.isHosting()) {
        if (app.net.pollPlayerConnection(peer_id, isConnect)) {
            if (isConnect) {
                app.on_player_connected(peer_id);
            }
        }

        if (auto result = app.net.receiveFromClient<JoinPacket>(static_cast<uint8_t>(PacketType::JOIN_PACKET))) {
            auto [pkt, client_id] = *result;
            app.handle_join_request(client_id, pkt.username);
        }

        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

        if (ImGui::BeginTable("UserTable", 2, flags)) {
            ImGui::TableSetupColumn("User ID", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Username", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (auto& [id, username] : app.server->get_connections()) {
                
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", id);

                ImGui::TableSetColumnIndex(1);
                
                ImGui::Text("%s", username.c_str());
            }

            ImGui::EndTable();
        }

        if(ImGui::Button("Start") && app.all_ready() && app.server->get_connections().size() > 1) {
            app.start_game_host();
        }

    } else {
        auto connections = app.retrieve_lobby_snapshot();

        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | 
                                    ImGuiTableFlags_RowBg | 
                                    ImGuiTableFlags_Resizable;

        if (ImGui::BeginTable("UserTable", 3, flags)) {
            ImGui::TableSetupColumn("User ID", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Username", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Ready", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (auto& [id, username] : connections) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", id);

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", username.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d", app.lobby_players_ready.at(id));
            }

            ImGui::EndTable();
        }

        if(ImGui::Button("Ready Up")) {
            app.client->toggle_ready();
            app.send_ready_packet(app.client->get_ready());
        }
    }

    ImGui::End();
    rlImGuiEnd();
    EndDrawing();
}

} // namespace ui
