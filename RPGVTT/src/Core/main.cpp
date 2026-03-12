#include "Window.hpp"
#include <imgui.h>
#include <iostream>

constexpr int WINDOW_WIDTH { 1280 };
constexpr int WINDOW_HEIGHT { 720 };

constexpr int GRID_SIZE { 32 };




int main(int argc, char* argv[]) {
    Window window("RPGVTT - Engine", WINDOW_WIDTH, WINDOW_HEIGHT, GRID_SIZE);

    if (!window.Init()) {
        std::cout << "Window initialization error!" << std::endl;
        return -1;
    }

    std::cout << "Window initialization successful"<< std::endl;

    bool showDebugPanel = true;

    while (window.IsRunning()) {
        window.PollEvents();
        window.Clear();

        window.DrawGrid();
        // --- UI Start ---
        window.ImGuiStartFrame();

        if (showDebugPanel) {
            ImGui::Begin("Panel Diagnostyczny VTT", &showDebugPanel);

            ImGui::Text("Witaj w swoim wlasnym silniku!");
            ImGui::Separator();

            ImGui::Text("Wydajnosc: %.1f FPS", ImGui::GetIO().Framerate);

            if (ImGui::Button("Wygeneruj Siatke (Test)")) {
                // Tu w przyszłości wywołamy logikę ECS
            }

            ImGui::End();
        }

        // Renderujemy UI i wyświetlamy klatkę
        window.ImGuiRender();
        // --- UI End ---
        window.Present();
    }

    return 0;
}