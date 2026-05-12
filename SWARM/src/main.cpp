#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Renderer.hpp"
#include "Mesh.hpp"
#include "Transform.hpp"
#include "Input.hpp"
#include <stdexcept>
#include <iostream>
#include <cmath>

constexpr int WINDOW_WIDTH{ 1280 };
constexpr int WINDOW_HEIGHT{ 720 };
constexpr float PLAYER_SPEED{ 0.1f };

int main(int argc, char* argv[]) 

{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        "SWARM - Platform & Player", WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );

    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    HWND hwnd = (HWND)SDL_GetPointerProperty(
        props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

    try {
        Renderer renderer;
        renderer.Init(hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);

        constexpr UINT alignedSize = (sizeof(ConstantBuffer) + 255) & ~255;
        constexpr UINT maxObjects = 3;

        // Create platform (large plane)
        Mesh platformMesh = MeshFactory::CreatePlane(renderer.GetDevice(), 10.0f, 10.0f, 
            DirectX::XMFLOAT4(0.2f, 0.8f, 0.2f, 1.0f)); // Green
        Transform platformTransform;
        platformTransform.position = DirectX::XMFLOAT3(0.0f, -2.0f, 0.0f);

		Mesh randomCubeMesh = MeshFactory::CreateCube(renderer.GetDevice(), 1.0f,
			DirectX::XMFLOAT4(0.2f, 0.2f, 0.8f, 1.0f)); // Blue
        Transform randomTransform;
        randomTransform.position = DirectX::XMFLOAT3(5.0f, 0.0f, 0.0f);

        // Create player cube
        Mesh playerMesh = MeshFactory::CreateCube(renderer.GetDevice(), 1.0f,
            DirectX::XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f)); // Red
        Transform playerTransform;
        playerTransform.position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

        bool running = true;
        while (running && !Input::ShouldQuit()) {
            Input::Update();
            running = !Input::ShouldQuit();

            // Handle player movement
            float moveX = 0.0f, moveZ = 0.0f;
            if (Input::GetMovementInput(moveX, moveZ)) {
                // Normalize diagonal movement
                float magnitude = sqrt(moveX * moveX + moveZ * moveZ);
                if (magnitude > 0.0f) {
                    moveX /= magnitude;
                    moveZ /= magnitude;
                }

                playerTransform.position.x += moveX * PLAYER_SPEED;
                playerTransform.position.z += moveZ * PLAYER_SPEED;
            }

            // Render
            renderer.BeginRender();
            renderer.RenderMesh(platformMesh, platformTransform, 0);
            renderer.RenderMesh(randomCubeMesh, randomTransform, 1);
            renderer.RenderMesh(playerMesh, playerTransform, 2);
            renderer.EndRender();
        }

        renderer.Shutdown();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}