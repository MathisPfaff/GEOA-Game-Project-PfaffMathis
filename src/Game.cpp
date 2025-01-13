#include <iostream>
#include <algorithm>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>
#include <chrono>
#include "Game.h"
#include "utils.h"
#include "structs.h"
#include "FlyFish.h"
#include "Enemy.h"
#include <vector>
#include <cmath>

Game::Game(const Window& window)
	: m_Window{ window }
	, m_Viewport{ 0,0,window.width,window.height }
	, m_pWindow{ nullptr }
	, m_pContext{ nullptr }
	, m_Initialized{ false }
	, m_MaxElapsedSeconds{ 0.1f }
{
	InitializeGameEngine();
	m_Position[0] = (m_Window.width / 2) - 200;
	m_Position[1] = m_Window.height / 2;

	m_MousePosition[0] = m_Window.width / 2;
	m_MousePosition[1] = m_Window.height / 2;

	Borders.RightBorder[0] = -(m_Window.width + 22.f);
	Borders.RightBorder[1] = 1.f;

	Borders.LeftBorder[0] = 22.f;
	Borders.LeftBorder[1] = 1.f;

	Borders.UpperBorder[0] = -(m_Window.height + 22.f);
	Borders.UpperBorder[2] = 1.f;

	Borders.BottomBorder[0] = 22.f;
	Borders.BottomBorder[2] = 1.f;

	m_MaxHealth = 20;
	m_CurrentHealth = m_MaxHealth;
}

Game::~Game()
{
	CleanupGameEngine();
}

void Game::InitializeGameEngine()
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "BaseGame::Initialize( ), error when calling SDL_Init: " << SDL_GetError() << std::endl;
		return;
	}

	// Use OpenGL 2.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	// Create window
	m_pWindow = SDL_CreateWindow(
		m_Window.title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		int(m_Window.width),
		int(m_Window.height),
		SDL_WINDOW_OPENGL);
	if (m_pWindow == nullptr)
	{
		std::cerr << "BaseGame::Initialize( ), error when calling SDL_CreateWindow: " << SDL_GetError() << std::endl;
		return;
	}

	// Create OpenGL context 
	m_pContext = SDL_GL_CreateContext(m_pWindow);
	if (m_pContext == nullptr)
	{
		std::cerr << "BaseGame::Initialize( ), error when calling SDL_GL_CreateContext: " << SDL_GetError() << std::endl;
		return;
	}

	// Set the swap interval for the current OpenGL context,
	// synchronize it with the vertical retrace
	if (m_Window.isVSyncOn)
	{
		if (SDL_GL_SetSwapInterval(1) < 0)
		{
			std::cerr << "BaseGame::Initialize( ), error when calling SDL_GL_SetSwapInterval: " << SDL_GetError() << std::endl;
			return;
		}
	}
	else
	{
		SDL_GL_SetSwapInterval(0);
	}

	// Set the Projection matrix to the identity matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set up a two-dimensional orthographic viewing region.
	glOrtho(0, m_Window.width, 0, m_Window.height, -1, 1); // y from bottom to top

	// Set the viewport to the client window area
	// The viewport is the rectangular region of the window where the image is drawn.
	glViewport(0, 0, int(m_Window.width), int(m_Window.height));

	// Set the Modelview matrix to the identity matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Enable color blending and use alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		std::cerr << "BaseGame::Initialize( ), error when calling TTF_Init: " << TTF_GetError() << std::endl;
		return;
	}

	m_Initialized = true;
}

void Game::Run()
{
	if (!m_Initialized)
	{
		std::cerr << "BaseGame::Run( ), BaseGame not correctly initialized, unable to run the BaseGame\n";
		std::cin.get();
		return;
	}

	// Main loop flag
	bool quit{ false };

	// Set start time
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

	//The event loop
	SDL_Event e{};
	while (!quit)
	{
		// Poll next event from queue
		while (SDL_PollEvent(&e) != 0)
		{
			// Handle the polled event
			switch (e.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				this->ProcessKeyDownEvent(e.key);
				break;
			case SDL_KEYUP:
				this->ProcessKeyUpEvent(e.key);
				break;
			case SDL_MOUSEMOTION:
				e.motion.y = int(m_Window.height) - e.motion.y;
				this->ProcessMouseMotionEvent(e.motion);
				break;
			case SDL_MOUSEBUTTONDOWN:
				e.button.y = int(m_Window.height) - e.button.y;
				this->ProcessMouseDownEvent(e.button);
				break;
			case SDL_MOUSEBUTTONUP:
				e.button.y = int(m_Window.height) - e.button.y;
				this->ProcessMouseUpEvent(e.button);
				break;
			}
		}

		if (!quit)
		{
			// Get current time
			std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

			// Calculate elapsed time
			float elapsedSeconds = std::chrono::duration<float>(t2 - t1).count();

			// Update current time
			t1 = t2;

			// Prevent jumps in time caused by break points
			elapsedSeconds = std::min(elapsedSeconds, m_MaxElapsedSeconds);

			// Call the BaseGame object 's Update function, using time in seconds (!)
			this->Update(elapsedSeconds);

			// Draw in the back buffer
			this->Draw();

			// Update screen: swap back and front buffer
			SDL_GL_SwapWindow(m_pWindow);
		}
	}
}

void Game::CleanupGameEngine()
{
	SDL_GL_DeleteContext(m_pContext);

	SDL_DestroyWindow(m_pWindow);
	m_pWindow = nullptr;

	//Quit SDL subsystems
	TTF_Quit();
	SDL_Quit();

}

void Game::Update(float elapsedSec)
{
	
	//MOVEMENT

	if (m_Position[2] <= 0.f)
	{
		m_EnergyActive = 1;
	}
	
	if (m_MoveState == MoveState::Translate)
	{
		if (m_TranslateForth == true)
		{
			ThreeBlade xyPosition{ m_Position[0], m_Position[1], 0.f, m_Position[3] };
			Motor translator{ Motor::Translation(m_EnergyActive * 200 * elapsedSec, !TwoBlade{xyPosition & m_MousePosition}) };
			m_Position = (translator * m_Position * ~translator).Grade3();
		}
		else
		{
			ThreeBlade xyPosition{ m_Position[0], m_Position[1], 0.f, m_Position[3] };
			Motor translator{ Motor::Translation(m_EnergyActive * 200 * elapsedSec, !TwoBlade{m_MousePosition & xyPosition}) };
			m_Position = (translator * m_Position * ~translator).Grade3();
		}
	}
	else if (m_MoveState == MoveState::Rotate)
	{
		if (m_RotateClockWise == true)
		{
			Motor rotator{ Motor::Rotation(m_EnergyActive * 50 * elapsedSec, TwoBlade{0,0,0,0,0,-1}) };
			Motor translator{ Motor::Translation(-m_MousePosition.VNorm(), !TwoBlade{m_MousePosition & ThreeBlade(0,0,0)}) };
			Motor total{ translator * rotator * ~translator };
			m_Position = (total * m_Position * ~total).Grade3();
		}
		else
		{
			Motor rotator{ Motor::Rotation(m_EnergyActive * 50 * elapsedSec, TwoBlade{0,0,0,0,0,1}) };
			Motor translator{ Motor::Translation(-m_MousePosition.VNorm(), !TwoBlade{m_MousePosition & ThreeBlade(0,0,0)}) };
			Motor total{ translator * rotator * ~translator };
			m_Position = (total * m_Position * ~total).Grade3();
		}
	}

	if (m_MirrorPower == MirrorState::Activated)
	{
		m_MirrorPower = MirrorState::NotPressed;
		m_Position = (m_MousePosition * m_Position * ~m_MousePosition).Grade3();
	}

	if (m_EnergyActive == 2)
	{
		m_Position[2] -= 0.4f * elapsedSec;
	}
	else
	{
		m_Position[2] += 0.2f * elapsedSec;
	}
	

	if (m_Position[2] > 1.f)
	{
		m_Position[2] = 1.f;
	}

	// Border checks

	if ((Borders.LeftBorder & m_Position) < 0.f)
	{
		m_Position[0] = m_Window.width / 2;
		m_Position[1] = m_Window.height / 2;
	}
	else if ((Borders.RightBorder & m_Position) > 0.f)
	{
		m_Position[0] = m_Window.width / 2;
		m_Position[1] = m_Window.height / 2;
	}
	else if ((Borders.UpperBorder & m_Position) > 0.f)
	{
		m_Position[0] = m_Window.width / 2;
		m_Position[1] = m_Window.height / 2;
	}
	else if ((Borders.BottomBorder & m_Position) < 0.f)
	{
		m_Position[0] = m_Window.width / 2;
		m_Position[1] = m_Window.height / 2;
	}

	//ENEMY

	for (int index{}; index <= 9; index++)
	{
		Enemies[index].Update(elapsedSec, Borders.LeftBorder, Borders.RightBorder, Borders.UpperBorder, Borders.BottomBorder);
	}

	for (int index{}; index <= 9; index++)
	{
		if (CheckOverLapping(m_Position, Enemies[index].GetPos(), 40.f, 40.f))
		{
			Enemies[index].ResetEnemy();
			m_CurrentHealth--;
		}
	}

	//GAME OVER CHECK

	if (m_CurrentHealth <= 0)
	{
		for (int index{}; index <= 9; index++)
		{
			Enemies[index].ResetEnemy();
			m_CurrentHealth = m_MaxHealth;
		}
	}
}

void Game::Draw() const
{
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	//PLAYER

	utils::SetColor(Color4f{ 1.f - m_Position[2], 0.f, m_Position[2], 1.f});
	utils::FillRect(m_Position[0] - 20.f, m_Position[1] - 20.f, 40.f, 40.f);
	utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	utils::FillEllipse(m_MousePosition[0], m_MousePosition[1], 10.f, 10.f);
	utils::SetColor(Color4f{ 1.f, 1.f, 0.f, 1.f });
	if (m_MirrorPower == MirrorState::Showing)
	{
		ThreeBlade mirror{ (m_MousePosition * m_Position * ~m_MousePosition).Grade3() };
		utils::FillRect(mirror[0] - 20.f, mirror[1] - 20.f, 40.f, 40.f);
	}
	utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });

	//ENEMY

	for (int index{}; index <= 9; index++)
	{
		Enemies[index].Draw();
	}

	//HEALTH

	utils::SetColor(Color4f{ 1.f, 0.f, 0.f, 1.f });
	utils::FillRect(20.f, m_Window.height - 40.f, m_CurrentHealth * 7.f, 20.f);

	utils::SetColor(Color4f{ 1.f, 1.f, 1.f, 1.f });
	utils::DrawRect(20.f, m_Window.height - 40.f, m_MaxHealth * 7.f, 20.f, 2.f);
}

bool Game::CheckOverLapping(const ThreeBlade& playerPos, const ThreeBlade& enemyPos, float playerWidth, float enemyWidth) const
{
	bool xOverlap = std::abs(playerPos[0] - enemyPos[0]) <= ((playerWidth / 2.f) + (enemyWidth / 2.f));

	bool yOverlap = std::abs(playerPos[1] - enemyPos[1]) <= ((playerWidth / 2.f) + (enemyWidth / 2.f));
	
	return xOverlap && yOverlap;
}