#pragma once
#include "structs.h"
#include "SDL.h"
#include "SDL_opengl.h"
#include "FlyFish.h"
#include <iostream>

class Game
{
private:
	enum class MoveState
	{
		None,
		Translate,
		Rotate
	};

	MoveState m_MoveState{};
	bool m_TranslateForth{ true };
	bool m_RotateClockWise{ true };

	enum class MirrorState
	{
		NotPressed,
		Showing,
		Activated
	};

	MirrorState m_MirrorPower{};
	bool m_Mirror{ true };

	ThreeBlade m_MousePosition{ 100, 100, 0, 1 };
	bool m_MouseLeftPressed{ false };

public:
	explicit Game(const Window& window);
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;

	~Game();

	void Run();

	void Update(float elapsedSec);

	void Draw() const;

	// Event handling
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
	{
		if ((e.keysym.sym == SDLK_e) && (m_Mirror == true))
		{
			m_MirrorPower = MirrorState::Showing;
		}
	}
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
	{
		switch (e.keysym.sym)
		{
		case SDLK_q:
			if (m_MoveState == MoveState::Translate)
			{
				m_TranslateForth = !m_TranslateForth;
			}
			else
			{
				m_TranslateForth = true;
				m_MoveState = MoveState::Translate;
			}
			break;
		case SDLK_w:
			if (m_MoveState == MoveState::Rotate)
			{
				m_RotateClockWise = !m_RotateClockWise;
			}
			else
			{
				m_RotateClockWise = true;
				m_MoveState = MoveState::Rotate;
			}
			break;
		case SDLK_SPACE:
			m_MoveState = MoveState::None;
			break;
		}

		if ((e.keysym.sym == SDLK_e) && (m_Mirror == true))
		{
			m_MirrorPower = MirrorState::Activated;
		}
	}
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
	{

	}
	void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
	{
		if ((e.button == SDL_BUTTON_LEFT) && (m_MouseLeftPressed == false))
		{
			m_MousePosition = ThreeBlade(e.x, e.y, 0, 1);
			m_MouseLeftPressed = true;
		}

	}
	void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
	{
		if (e.button == SDL_BUTTON_LEFT)
		{
			m_MouseLeftPressed = false;
		}
	}

	const Rectf& GetViewPort() const
	{
		return m_Viewport;
	}

private:
	// DATA MEMBERS
	// The window properties
	const Window m_Window;
	const Rectf m_Viewport;
	// The window we render tocontext
	SDL_GLContext m_pContext;
	SDL_Window* m_pWindow;
	// OpenGL 
	// Init info
	bool m_Initialized;
	// Prevent timing jumps when debugging
	const float m_MaxElapsedSeconds;


	//custom members
	ThreeBlade m_Position{ 0, 0, 0, 1 };

	// FUNCTIONS
	void InitializeGameEngine();
	void CleanupGameEngine();
};
