#pragma once
#include <SDL3/SDL.h>

class Input {
public:
	static void Update();

	static bool IsKeyPressed(SDL_Scancode key);
	static bool GetMovementInput(float& moveX, float& moveZ);
	static bool ShouldQuit();

private:
	static bool m_keys[512];
	static bool m_quit;
};
