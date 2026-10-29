#pragma once

#include <string>

#include "raylib.h"
#include "context.h"
#include "imgui.h"
#include "rlImGui.h"
#include "Networking/NetworkWrapper.h"
#include "Networking/Packets.h"

constexpr double TICK_RATE = 1.0/128.0;

class App {
    public:
        App(int w, int h);
        ~App();

        void run();
        void run_fixed();

        void menu_loop();
        void in_game_loop();
        void pause_loop();

        bool is_running();

    private:
        int width;
        int height;

        char ip[128];
        int port;
        NetLib::GameSession<PacketType> net;

        double dt;
        double accumulator;

        ui_context current_ui;
};