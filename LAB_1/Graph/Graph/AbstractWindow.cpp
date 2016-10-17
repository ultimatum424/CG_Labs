#include "stdafx.h"
#include "AbstractWindow.h"
#include <SDL2/SDL_video.h>

#ifdef _WIN32
#include <Windows.h>
#endif
#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <chrono>
#include <type_traits>
#include <iostream>

#include <string>
#include <mutex>
namespace
{
	const char WINDOW_TITLE[] = "SDL2/OpenGL Demo";
	std::once_flag g_glewInitOnceFlag;

	// ���������� unique_ptr � ���� �������� �������� �������� ������ delete.
	using SDLWindowPtr = std::unique_ptr<SDL_Window, void(*)(SDL_Window*)>;
	using SDLGLContextPtr = std::unique_ptr<void, void(*)(SDL_GLContext)>;

	class CChronometer
	{
	public:
		CChronometer()
			: m_lastTime(std::chrono::system_clock::now())
		{
		}

		float GrabDeltaTime()
		{
			auto newTime = std::chrono::system_clock::now();
			auto timePassed = std::chrono::duration_cast<std::chrono::milliseconds>(newTime - m_lastTime);
			m_lastTime = newTime;
			return 0.001f * float(timePassed.count());
		}

	private:
		std::chrono::system_clock::time_point m_lastTime;
	};
}

class CAbstractWindow::Impl
{
public:
	Impl()
		: m_pWindow(nullptr, SDL_DestroyWindow)
		, m_pGLContext(nullptr, SDL_GL_DeleteContext)
	{
	}

	void Show(glm::ivec2 const& size)
	{
		m_size = size;

		// �������� Compatiblity Profile
		// ��������� ��������� SDL_GL ������ ����������� �� �������� ����,
		// ����� �� ��������� ����������� �������� ��������� ����������
		// OpenGL � �������.
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		// ����������� �������� SDL_WINDOWPOS_CENTERED ������ x � y �������� SDL2
		// ���������� ���� � ������ �������� �� ���� x � y.
		// ��� ������������� OpenGL �� ������ ������� ���� SDL_WINDOW_OPENGL.
		m_pWindow.reset(SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			size.x, size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE));

		// ������ �������� OpenGL, ��������� � �����.
		m_pGLContext.reset(SDL_GL_CreateContext(m_pWindow.get()));
		if (!m_pGLContext)
		{
			std::cerr << "OpenGL context initialization failed" << std::endl;
			std::abort();
		}
		InitGlewOnce();
	}

	glm::ivec2 GetWindowSize() const
	{
		return m_size;
	}

	bool IsTerminated() const
	{
		return m_isTerminated;
	}

	void SetBackgroundColor(const glm::vec4 &color)
	{
		m_clearColor = color;
	}

	void Clear()const
	{
		// ������� ����� ������ ���� ���������� OpenGL
		glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void SwapBuffers()const
	{
		// ����� ������������� ����� � ���� �� ������.
		// ��� ���� ������� ����� ������ ����� ��� ��������� ������ �����.
		// ����� ���� ������� ������ �������� ����� ��������� �� ������� ������� �������.
		SDL_GL_SwapWindow(m_pWindow.get());
	}

	void DumpGLErrors()
	{
		for (GLenum error = glGetError(); error != GL_NO_ERROR; error = glGetError())
		{
			std::string message;
			switch (error)
			{
			case GL_INVALID_ENUM:
				message = "invalid enum passed to GL function (GL_INVALID_ENUM)";
				break;
			case GL_INVALID_VALUE:
				message = "invalid parameter passed to GL function (GL_INVALID_VALUE)";
				break;
			case GL_INVALID_OPERATION:
				message = "cannot execute some of GL functions in current state (GL_INVALID_OPERATION)";
				break;
			case GL_STACK_OVERFLOW:
				message = "matrix stack overflow occured inside GL (GL_STACK_OVERFLOW)";
				break;
			case GL_STACK_UNDERFLOW:
				message = "matrix stack underflow occured inside GL (GL_STACK_UNDERFLOW)";
				break;
			case GL_OUT_OF_MEMORY:
				message = "no enough memory to execute GL function (GL_OUT_OF_MEMORY)";
				break;
			default:
				message = "error in some GL extension (framebuffers, shaders, etc)";
				break;
			}
			std::cerr << "OpenGL error: " << message << std::endl;
#ifdef _WIN32
			__debugbreak();
#endif
		}
	}

	bool ConsumeEvent(const SDL_Event &event)
	{
		bool consumed = false;
		if (event.type == SDL_QUIT)
		{
			m_isTerminated = true;
			consumed = true;
		}
		else if (event.type == SDL_WINDOWEVENT)
		{
			OnWindowEvent(event.window);
			consumed = true;
		}
		return consumed;
	}

private:
	void OnWindowEvent(const SDL_WindowEvent &event)
	{
		if (event.event == SDL_WINDOWEVENT_RESIZED)
		{
			m_size = { event.data1, event.data2 };
		}
	}

	void InitGlewOnce()
	{
		// �������� ������������� GLEW ������ ���� ��� �� ����� ������ ����������.
		std::call_once(g_glewInitOnceFlag, []() {
			glewExperimental = GL_TRUE;
			if (GLEW_OK != glewInit())
			{
				std::cerr << "GLEW initialization failed, aborting." << std::endl;
				std::abort();
			}
		});
	}

	SDLWindowPtr m_pWindow;
	SDLGLContextPtr m_pGLContext;
	glm::ivec2 m_size;
	glm::vec4 m_clearColor;
	bool m_isTerminated = false;
};

CAbstractWindow::CAbstractWindow()
	: m_pImpl(new Impl)
{
}

CAbstractWindow::~CAbstractWindow()
{
}

void CAbstractWindow::Show(const glm::fvec2 &size)
{
	m_pImpl->Show(size);
}

void CAbstractWindow::DoGameLoop()
{
	SDL_Event event;
	CChronometer chronometer;
	while (true)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (!m_pImpl->ConsumeEvent(event))
			{
				OnWindowEvent(event);
			}
		}
		if (m_pImpl->IsTerminated())
		{
			break;
		}
		// ������� ������ �����, ���������� � ��������� �����, ����� ������ �����.
		m_pImpl->Clear();
		const float deltaSeconds = chronometer.GrabDeltaTime();
		OnUpdateWindow(deltaSeconds);
		OnDrawWindow(m_pImpl->GetWindowSize());
		m_pImpl->DumpGLErrors();
		m_pImpl->SwapBuffers();
	}
}

void CAbstractWindow::SetBackgroundColor(const glm::vec4 &color)
{
	m_pImpl->SetBackgroundColor(color);
}
