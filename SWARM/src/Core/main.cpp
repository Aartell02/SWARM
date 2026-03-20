#include "Window.hpp"
#include "Components.hpp"
#include <imgui.h>
#include <iostream>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr int WINDOW_WIDTH{ 1280 };
constexpr int WINDOW_HEIGHT{ 720 };

// ==========================================
// 1. ZAKTUALIZOWANY VERTEX SHADER
// ==========================================
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    out vec3 vertexColor;

    // NOWE: Uniform to zmienna, którą C++ wysyła do GPU co klatkę
    // mat4 to macierz 4x4, idealna do przesuwania i obracania w 3D
    uniform mat4 u_Transform; 

    void main() {
        // Mnożymy pozycję wierzchołka przez macierz transformacji!
        gl_Position = u_Transform * vec4(aPos, 1.0); 
        vertexColor = aColor;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 vertexColor;
    out vec4 FragColor;

    void main() {
        FragColor = vec4(vertexColor, 1.0);
    }
)";

int main(int argc, char* argv[]) {
    Window window("RPGVTT - 3D Engine", WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!window.Init()) return -1;

    // --- KOMPILACJA SHADERÓW ---
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --- DANE GEOMETRII ---
    float vertices[] = {
         0.0f,  0.5f, 0.0f,     1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --- REJESTR ECS ---
    entt::registry registry;
    auto entity = registry.create();
    registry.emplace<TagComponent>(entity, "Trojkat 3D");
    // Zakładam, że TransformComponent ma GridPosition (użyjemy go jako pozycji w świecie)
    registry.emplace<TransformComponent>(entity, glm::vec3(0.0f, 0.0f, 0.0f));

    // NOWE: Dodajemy zmienną do obracania specjalnie dla tego testu
    float rotationAngle = 0.0f;

    bool showDebugPanel = true;

    while (window.IsRunning()) {
        SDL_Event event;
        while (window.PollEvent(event)) {
            if (event.type == SDL_KEYDOWN && !ImGui::GetIO().WantCaptureKeyboard) {
                if (event.key.keysym.sym == SDLK_ESCAPE) window.Quit();
            }
        }

        window.Clear();

        // --- RYSOWANIE ---
        glUseProgram(shaderProgram);

        // ==========================================
        // 2. MATEMATYKA 3D (Wysyłanie danych z ECS do Karty Graficznej)
        // ==========================================
        auto& transform = registry.get<TransformComponent>(entity);

        // Tworzymy "pustą" macierz jednostkową (Identity Matrix)
        glm::mat4 transformMatrix = glm::mat4(1.0f);

        // KROK A: Przesunięcie (Translation) na podstawie danych z ECS
        transformMatrix = glm::translate(transformMatrix, glm::vec3(transform.GridPosition.x, transform.GridPosition.y, transform.GridPosition.z));

        // KROK B: Obrót (Rotation) wokół osi Z (żeby kręcił się jak wskazówki zegara)
        transformMatrix = glm::rotate(transformMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

        // KROK C: Wysłanie obliczonej macierzy do zmiennej "u_Transform" w naszym shaderze
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "u_Transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformMatrix));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // --- INTERFEJS UŻYTKOWNIKA ---
        window.ImGuiStartFrame();
        if (showDebugPanel) {
            ImGui::Begin("Kontroler 3D");
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Separator();

            // Suwaki bezpośrednio modyfikujące pamięć ECS i zmienne
            ImGui::DragFloat3("Pozycja (X,Y,Z)", &transform.GridPosition.x, 0.01f);
            ImGui::SliderFloat("Obrot (Stopnie)", &rotationAngle, 0.0f, 360.0f);

            ImGui::End();
        }
        window.ImGuiRender();

        window.Present();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    return 0;
}