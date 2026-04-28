#include "Input.hpp"

bool Input::m_keys[512] = {};
bool Input::m_quit = false;

void Input::Update() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_EVENT_QUIT:
			m_quit = true;
			break;
		case SDL_EVENT_KEY_DOWN:
			if (event.key.scancode < 512) {
				m_keys[event.key.scancode] = true;
			}
			break;
		case SDL_EVENT_KEY_UP:
			if (event.key.scancode < 512) {
				m_keys[event.key.scancode] = false;
			}
			break;
		}
	}
}

bool Input::IsKeyPressed(SDL_Scancode key) {
	if (key >= 512) return false;
	return m_keys[key];
}

bool Input::GetMovementInput(float& moveX, float& moveZ) {
	moveX = 0.0f;
	moveZ = 0.0f;

	if (IsKeyPressed(SDL_SCANCODE_W)) moveZ += 1.0f;
	if (IsKeyPressed(SDL_SCANCODE_S)) moveZ -= 1.0f;
	if (IsKeyPressed(SDL_SCANCODE_A)) moveX -= 1.0f;
	if (IsKeyPressed(SDL_SCANCODE_D)) moveX += 1.0f;

	return (moveX != 0.0f || moveZ != 0.0f);
}

bool Input::ShouldQuit() {
	return m_quit;
}
