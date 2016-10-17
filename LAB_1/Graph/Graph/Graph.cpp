// Graph.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "AbstractWindow.h"
#include "WorkWindow.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <SDL2/SDL.h>

// Выключаем макроподмену main на SDL_main,
// т.к. приложение собирается c SUBSYSTEM:CONSOLE
#ifdef _WIN32
#undef main
#endif

int main()
{
	CWorkWindow window;
	window.Show({ 800.0f, 600.0f });
	window.DoGameLoop();

	return 0;
}