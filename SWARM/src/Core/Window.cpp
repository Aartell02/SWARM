#include "Window.hpp"
#include <iostream>
#include <entt/entt.hpp>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

Window::Window(const std::string& title, int width, int height)
    : m_title(title), m_width(width), m_height(height),
    m_isRunning(false), m_window(nullptr) {
}

Window::~Window() {
    // Cleaning
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    if (m_glContext) SDL_GL_DeleteContext(m_glContext);
    if (m_window) SDL_DestroyWindow(m_window);

    SDL_Quit();
}

bool Window::Init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Błąd SDL_Init: " << SDL_GetError() << std::endl;
        return false;
    }

    // --- OPENGL CONFIG ---
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Depth Buffer
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // --- WINDOW CREATION ---
    m_window = SDL_CreateWindow(
        m_title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_width, m_height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
    );
    if (!m_window) {
        std::cerr << "Window Creation Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // --- OPENGL Context ---
    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        std::cerr << "GL Context Error: " << SDL_GetError() << std::endl;
        return false;
    }
    SDL_GL_MakeCurrent(m_window, m_glContext);
    SDL_GL_SetSwapInterval(1); // VSync ON

    // --- 4. GLAD init ---
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return false;
    }

    // --- 5. KONFIGURACJA STANU OPENGL ---
    glEnable(GL_DEPTH_TEST); // Włączamy testowanie głębi dla 3D

    // --- 6. INICJALIZACJA IMGUI ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    m_isRunning = true;
    return true;
}

bool Window::PollEvent(SDL_Event& event) {
    if (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) {
            m_isRunning = false;
        }
        return true;
    }
    return false;
}

void Window::Clear() {
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::ImGuiStartFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void Window::ImGuiRender() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::Present() {
    SDL_GL_SwapWindow(m_window);
}