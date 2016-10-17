// Graph.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include "AbstractWindow.h"
#include "WorkWindow.h"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <SDL2/SDL.h>

// ��������� ������������ main �� SDL_main,
// �.�. ���������� ���������� c SUBSYSTEM:CONSOLE
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