#include "Window.hpp"

int main(int argc, char* argv[]) {
    Window window("RPGVTT - Engine", 1280, 720);

    if (!window.Init()) {
        return -1;
    }

    while (window.IsRunning()) {
        window.PollEvents();
        window.Clear();
        window.Present();
    }

    return 0;
}