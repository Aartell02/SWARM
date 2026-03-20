#pragma once
#include <string>
#include <glad/glad.h>
#include <SDL.h>

class Window {
public:
    Window(const std::string& title, int width, int height);
    ~Window();

    bool Init();
    bool PollEvent(SDL_Event& event);
    void Clear();
    void Present();

    void ImGuiStartFrame();
    void ImGuiRender();

    bool IsRunning() const { return m_isRunning; }
    void Quit() { m_isRunning = false; }

private:
    std::string m_title;
    int m_width;
    int m_height;
    bool m_isRunning;

    SDL_Window* m_window;
    SDL_GLContext m_glContext; // Zastąpił SDL_Renderer
};