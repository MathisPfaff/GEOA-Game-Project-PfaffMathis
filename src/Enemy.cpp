#include "Enemy.h"
#include "FlyFish.h"
#include "math.h"
#include "utils.h"
#include <iostream>

Enemy::Enemy()
{
	m_Position = ThreeBlade{ 0.f, 0.f, 0.f, 1.f };
	IsMoving = false;
}

Enemy::~Enemy()
{}

void Enemy::Update(float elapsedSec, OneBlade Left, OneBlade Right, OneBlade Up, OneBlade Bottom)
{
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
	}

	switch (m_StartDirection)
	{
	case Enemy::StartingPosition::Left:
		m_Position[0] += 100 * elapsedSec;
		break;
	case Enemy::StartingPosition::Right:
		m_Position[0] -= 100 * elapsedSec;
		break;
	case Enemy::StartingPosition::Up:
		m_Position[1] -= 100 * elapsedSec;
		break;
	case Enemy::StartingPosition::Bottom:
		m_Position[1] += 100 * elapsedSec;
		break;
	}

}

void Enemy::Draw() const
{
	utils::FillRect(m_Position[0] - 20.f, m_Position[1] - 20.f, 40.f, 40.f);
}

void Enemy::GetRandomPos()
{
	m_StartDirection = StartingPosition(rand() % 4);
}