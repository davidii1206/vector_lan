#include "app.h"
#include "context.h"
#include "imgui.h"
#include "raylib.h"

#include <charconv>
#include <cstring>
#include <iostream>

App::App(int w, int h) : width(w), height(h) {
    InitWindow(width, height, "Vector");
    current_ui = ui_context::menu;
    rlImGuiSetup(true);
    port = 25565;
}

App::~App() {
    CloseWindow();
}

bool App::is_running() {
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

    switch (current_ui) {
        case menu: 
            menu_loop();
            break;
        case in_game:
            in_game_loop();
            break;
        case pause_menu:
            pause_loop();
            break;
    }
}

void App::run_fixed() {

}

void App::menu_loop() {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    rlImGuiBegin();

    ImGui::Begin("Networking");
    if (ImGui::Button("Host")) {
        if (net.startHost(port)) {
            std::cout << "Hosting on port 25565" << std::endl;
        }
    }

    ImGui::InputText("IP Address", ip, IM_ARRAYSIZE(ip));

    if (ImGui::Button("Join")) {
        if (net.connectToServer(ip, port)) {
            std::cout << "Connected!" << std::endl;
        }
    }
    ImGui::End();

    rlImGuiEnd();

    EndDrawing();
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
