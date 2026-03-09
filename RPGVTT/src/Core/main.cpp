#include "Window.hpp"
#include <imgui.h>
#include <iostream>

int main(int argc, char* argv[]) {
    Window window("RPGVTT - Engine", 1280, 720);

    if (!window.Init()) {
        std::cout << "Inicjalizacja okna ZAKONCZONA NIEPOWODZENIEM!" << std::endl;
        return -1;
    }

    std::cout << "Inicjalizacja okna SUKCES! Wchodzimy do petli gry." << std::endl;

    bool showDebugPanel = true;

    while (window.IsRunning()) {
        window.PollEvents();
        window.Clear();

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