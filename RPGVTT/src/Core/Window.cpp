#include "Window.hpp"
#include <iostream>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

Window::Window(const std::string& title, int width, int height, int gridSize)
    : m_title(title), m_width(width), m_height(height), m_gridSize(gridSize),
    m_isRunning(false), m_window(nullptr), m_renderer(nullptr) {
}

Window::~Window() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer2_Init(m_renderer);

    return true;
}

void Window::PollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) {
            m_isRunning = false;
        }
    }
}

void Window::Clear() {
    SDL_SetRenderDrawColor(m_renderer, 30, 30, 30, 255);
    SDL_RenderClear(m_renderer);
}

void Window::ImGuiStartFrame() {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void Window::ImGuiRender() {
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_renderer);
}

void Window::Present() {
    SDL_RenderPresent(m_renderer);
}

void Window::DrawGrid()
{
    SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_Rect rowLine{ .x = 0, .y = 0, .w = m_width , .h = 0 };
    for (rowLine.y = 0; rowLine.y <= m_height; rowLine.y += m_gridSize)
    {
        SDL_RenderDrawRect(m_renderer, &rowLine);
    }
}