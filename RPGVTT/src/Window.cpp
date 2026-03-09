#include "Window.hpp"
#include <iostream>

Window::Window(const std::string& title, int width, int height)
    : m_title(title), m_width(width), m_height(height),
    m_isRunning(false), m_window(nullptr), m_renderer(nullptr) {
}

Window::~Window() {
    if (m_renderer) SDL_DestroyRenderer(m_renderer);
    if (m_window) SDL_DestroyWindow(m_window);
    SDL_Quit();
}

bool Window::Init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Błąd SDL_Init: " << SDL_GetError() << std::endl;
        return false;
    }

    m_window = SDL_CreateWindow(
        m_title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_width, m_height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!m_window) {
        std::cerr << "Błąd SDL_CreateWindow: " << SDL_GetError() << std::endl;
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer) {
        std::cerr << "Błąd SDL_CreateRenderer: " << SDL_GetError() << std::endl;
        return false;
    }

    m_isRunning = true;
    return true;
}

void Window::PollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_isRunning = false;
        }
    }
}

void Window::Clear() {
    SDL_SetRenderDrawColor(m_renderer, 30, 30, 30, 255);
    SDL_RenderClear(m_renderer);
}

void Window::Present() {
    SDL_RenderPresent(m_renderer);
}