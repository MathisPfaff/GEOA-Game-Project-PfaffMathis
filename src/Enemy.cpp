#include "Enemy.h"
#include "FlyFish.h"
#include "math.h"
#include "utils.h"
#include <iostream>

Enemy::Enemy()
{
	m_Position = ThreeBlade{ 0.f, 0.f, 0.f, 1.f };
	m_PillarPos = ThreeBlade{ 0.f, 0.f, 0.f, 1.f };
	m_Rotator = false;
	IsMoving = false;
	m_Speed = 0.f;
}

Enemy::~Enemy()
{}

void Enemy::Update(float elapsedSec, OneBlade Left, OneBlade Right, OneBlade Up, OneBlade Bottom)
{
	//INITIALIZE POS AND DIRECTION
	
	if (IsMoving == false)
	{
		GetRandomPos();
		IsMoving = true;

		switch (m_StartDirection)
		{
		case Enemy::StartingPosition::Left:
			m_Position[1] = rand() % 721;
			m_Position[0] = -Left[0];
			break;
		case Enemy::StartingPosition::Right:
			m_Position[1] = rand() % 721;
			m_Position[0] = -Right[0];
			break;
		case Enemy::StartingPosition::Up:
			m_Position[0] = rand() % 1281;
			m_Position[1] = -Up[0];
			break;
		case Enemy::StartingPosition::Bottom:
			m_Position[0] = rand() % 1281;
			m_Position[1] = -Bottom[0];
			break;
		}

		if (m_Rotator == true)
		{
			m_PillarPos = m_Position;
			float distance{ float(rand() % 101) + 50 };
			Motor translator{ Motor::Translation(distance, TwoBlade{ 1,0,0,0,0,0})};
			m_Position = (translator * m_Position * ~translator).Grade3();
		}
	}

	//MOVEMENT

	if (m_Rotator == false)
	{
		Motor translator{};

		switch (m_StartDirection)
		{
		case Enemy::StartingPosition::Left:
			translator = Motor::Translation(m_Speed * elapsedSec, TwoBlade{ 1,0,0,0,0,0 });
			m_Position = (translator * m_Position * ~translator).Grade3();
			break;
		case Enemy::StartingPosition::Right:
			translator = Motor::Translation(m_Speed * elapsedSec, TwoBlade{ -1,0,0,0,0,0 });
			m_Position = (translator * m_Position * ~translator).Grade3();
			break;
		case Enemy::StartingPosition::Up:
			translator = Motor::Translation(m_Speed * elapsedSec, TwoBlade{ 0,-1,0,0,0,0 });
			m_Position = (translator * m_Position * ~translator).Grade3();
			break;
		case Enemy::StartingPosition::Bottom:
			translator = Motor::Translation(m_Speed * elapsedSec, TwoBlade{ 0,1,0,0,0,0 });
			m_Position = (translator * m_Position * ~translator).Grade3();
			break;
		}
	}
	else
	{
		Motor translator{};
		
		switch (m_StartDirection)
		{
		case Enemy::StartingPosition::Left:
			translator = Motor::Translation(m_Speed * elapsedSec, TwoBlade{ 1,0,0,0,0,0 });
			m_PillarPos = (translator * m_PillarPos * ~translator).Grade3();
			m_Position = (translator * m_Position * ~translator).Grade3();
			break;
		case Enemy::StartingPosition::Right:
			translator = Motor::Translation(m_Speed * elapsedSec, TwoBlade{ -1,0,0,0,0,0 });
			m_PillarPos = (translator * m_PillarPos * ~translator).Grade3();
			m_Position = (translator * m_Position * ~translator).Grade3();
			break;
		case Enemy::StartingPosition::Up:
			translator = Motor::Translation(m_Speed * elapsedSec, TwoBlade{ 0,-1,0,0,0,0 });
			m_PillarPos = (translator * m_PillarPos * ~translator).Grade3();
			m_Position = (translator * m_Position * ~translator).Grade3();
			break;
		case Enemy::StartingPosition::Bottom:
			translator = Motor::Translation(m_Speed * elapsedSec, TwoBlade{ 0,1,0,0,0,0 });
			m_PillarPos = (translator * m_PillarPos * ~translator).Grade3();
			m_Position = (translator * m_Position * ~translator).Grade3();
			break;
		}

		Motor rotator{ Motor::Rotation(m_Speed * elapsedSec, TwoBlade{0,0,0,0,0,-1}) };
		translator = Motor::Translation(-m_PillarPos.VNorm(), !TwoBlade{m_PillarPos & ThreeBlade(0,0,0)});
		Motor total{ translator * rotator * ~translator };
		m_Position = (total * m_Position * ~total).Grade3();
	}

	

	//BORDER CHECK

	switch (m_StartDirection)
	{
	case Enemy::StartingPosition::Left:
		if ((Right & m_Position) > 0.f)
		{
			IsMoving = false;
		}
		break;
	case Enemy::StartingPosition::Right:
		if ((Left & m_Position) < 0.f)
		{
			IsMoving = false;
		}
		break;
	case Enemy::StartingPosition::Up:
		if ((Bottom & m_Position) < 0.f)
		{
			IsMoving = false;
		}
		break;
	case Enemy::StartingPosition::Bottom:
		if ((Up & m_Position) > 0.f)
		{
			IsMoving = false;
		}
		break;
	}
}

void Enemy::Draw() const
{
	if (m_Rotator == true)
	{
		utils::SetColor(Color4f{ 1.f, 1.f, 1.f, 1.f });
		utils::FillEllipse(m_PillarPos[0], m_PillarPos[1], 10.f, 10.f);
		utils::DrawLine(m_Position[0], m_Position[1], m_PillarPos[0], m_PillarPos[1]);
	}
	
	utils::SetColor(Color4f{ 0.f, 1.f, 0.f, 1.f });
	utils::FillRect(m_Position[0] - 20.f, m_Position[1] - 20.f, 40.f, 40.f);
}

void Enemy::GetRandomPos()
{
	m_StartDirection = StartingPosition(rand() % 4);
	m_Speed = rand() % 151 + 75;
	if (rand() % 6 == 0)
	{
		m_Rotator = true;
	}
	else
	{
		m_Rotator = false;
	}
}

const ThreeBlade& Enemy::GetPos() const
{
	return m_Position;
}

void Enemy::ResetEnemy()
{
	IsMoving = false;
}