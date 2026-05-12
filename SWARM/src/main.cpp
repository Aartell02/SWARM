#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Renderer.hpp"
#include "components/MeshRenderer.hpp"
#include "components/Transform.hpp"
#include "Input.hpp"
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <d3dcompiler.h>
#include "components/Entity.hpp"
#include "Scene.hpp"

constexpr int WINDOW_WIDTH{ 1280 };
constexpr int WINDOW_HEIGHT{ 720 };
constexpr float PLAYER_SPEED{ 0.1f };

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("SWARM - Platform & Player", WINDOW_WIDTH, WINDOW_HEIGHT,SDL_WINDOW_RESIZABLE);

    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    HWND hwnd = (HWND)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

    try 
    {
        Renderer renderer;
        renderer.Init(hwnd, WINDOW_WIDTH, WINDOW_HEIGHT);

        constexpr UINT alignedSize = (sizeof(ConstantBuffer) + 255) & ~255;
        constexpr UINT maxObjects = 3;

		// Create scene
        Scene scene;

        // Create platform (large plane)

        Entity platform = scene.CreateEntity("Platform");
        platform.AddComponent<Transform>().position = DirectX::XMFLOAT3(0.0f, -2.0f, 0.0f);
        platform.AddComponent<MeshRenderer>(MeshFactory::CreatePlane(renderer.GetDevice(), 10.0f, 10.0f, DirectX::XMFLOAT4(0.2f, 0.8f, 0.2f, 1.0f)));

        Entity randomCube = scene.CreateEntity("RandomCube");
        randomCube.AddComponent<Transform>().position = DirectX::XMFLOAT3(5.0f, 0.0f, 5.0f);
        randomCube.AddComponent<MeshRenderer>(MeshFactory::CreateCube(renderer.GetDevice(), 1.0f, DirectX::XMFLOAT4(0.2f, 0.2f, 0.8f, 1.0f)));

        Entity player = scene.CreateEntity("Player");
        player.AddComponent<Transform>().position = DirectX::XMFLOAT3(0.0f, 2.0f, 5.0f);
        player.AddComponent<MeshRenderer>(MeshFactory::CreateCube(renderer.GetDevice(), 1.0f, DirectX::XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f)));
		Camera camera;

        camera.Init(static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT));
		camera.SetTarget(&player.GetComponent<Transform>());
		renderer.SetCamera(&camera);

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

                auto& t = player.GetComponent<Transform>();
                t.position.x += moveX * PLAYER_SPEED;
                t.position.z += moveZ * PLAYER_SPEED;
            }

            // Render
            renderer.BeginRender();
            auto view = scene.m_registry.view<Transform, MeshRenderer>();
            for (auto [entity, transform, meshRenderer] : view.each()) {
                renderer.RenderMesh(meshRenderer.mesh, transform);
            }
            renderer.EndRender();
        }

        renderer.Shutdown();
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}