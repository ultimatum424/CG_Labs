#include "stdafx.h"
#include "WorkWindow.h"

#include <GL\glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace
{
	void DrawCoordinateSystem(const glm::ivec2 & size)
	{
		
		glBegin(GL_LINES);
		//X axis
		glVertex2f(0.0f, size.y / 2);
		glVertex2f(size.x, size.y / 2);

		//Y axis
		glVertex2f(size.x / 2, 0.0f);
		glVertex2f(size.x / 2, size.y);

		//axis direction
		glVertex2f(size.x, size.y / 2);
		glVertex2f(size.x - 15.0f, size.y / 2 + 5.0f);
		
		glVertex2f(size.x, size.y / 2);
		glVertex2f(size.x - 15.0f, size.y / 2 - 5.0f);
		
		glVertex2f(size.x / 2, 0);
		glVertex2f(size.x / 2 + 5.0f, 15.0f);

		glVertex2f(size.x / 2, 0.0f);
		glVertex2f(size.x / 2 - 5, 15.0f);

		// single segments
		for (float i = 20.0f; i <= size.y - 20.0f; i += 10.0f)
		{
			glVertex2f(size.x / 2 - 5.0f, i);
			glVertex2f(size.x / 2 + 5.0f, i);
		}

		for (float i = 20.0f; i <= size.x - 20.0f; i += 10.0f)
		{
			glVertex2f(i, size.y / 2 + 5.0f);
			glVertex2f(i, size.y / 2 - 5.0f);
		}
		glEnd();
	}

	void DrawCardioid(const float radius, const glm::ivec2 centre)
	{
		float max = 2.0f * float(M_PI);
		glBegin(GL_LINE_STRIP);
		for (float i = 0.0f; i <= max; i += 0.1f)
		{
			float x = (2 * radius * cosf(i)) - (radius * cos(2 * i)) + centre.x;
			float y = (2 * radius * sinf(i)) - (radius * sin(2 * i)) + centre.y;
			glVertex2f(x, y);
		}

		glEnd();
	}

}
CWorkWindow::CWorkWindow()
{
}


CWorkWindow::~CWorkWindow()
{
}

void CWorkWindow::OnUpdateWindow(float deltaSeconds)
{
	m_time += deltaSeconds;
}

void CWorkWindow::OnDrawWindow(const glm::ivec2 & size)
{
	SetupView(size);
	
	glColor3f(1, 1, 1);
	DrawCoordinateSystem(size);

	glColor3f(0, 1, 0);
	DrawCardioid(100.0f, size / 2);
	
}

void CWorkWindow::OnWindowEvent(const SDL_Event & event)
{
}

void CWorkWindow::SetupView(const glm::ivec2 & size)
{
	// Матрица ортографического проецирования изображения в трёхмерном пространстве
	// из параллелипипеда с размером, равным (size.X x size.Y x 2).
	const glm::mat4 matrix = glm::ortho<float>(0.f, size.x, size.y, 0.f);
	glViewport(0, 0, size.x, size.y);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(matrix));
	glMatrixMode(GL_MODELVIEW);
}
