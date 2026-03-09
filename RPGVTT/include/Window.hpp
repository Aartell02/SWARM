#pragma once
#include <string>
#include <SDL2/SDL.h>

class Window 
{
public:
	Window(const std::string& title, int width, int height);
	~Window();
	bool Init();
	void PollEvents();
	void Clear();
	void Present();

	inline bool IsRunning() const { return m_isRunning; }

private:
	std::string m_title;
	int m_width;
	int m_height;
	bool m_isRunning;

	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
};