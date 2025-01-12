#pragma once
#include "FlyFish.h"
#include "utils.h"

class Enemy
{
private:
	ThreeBlade m_Position{};
	float m_StartPos{};

	enum class StartingPosition
	{
		Left,
		Right,
		Up,
		Bottom
	};

	StartingPosition m_StartDirection{};

	bool IsMoving{};

	float m_Speed{};

public:
	Enemy();
	Enemy(const Enemy& other) = delete;
	Enemy& operator= (const Enemy& other) = delete;
	Enemy(Enemy&& other) = delete;
	Enemy& operator=(Enemy&& other) = delete;

	~Enemy();

	void Update(float elapsedSec, OneBlade Left, OneBlade Right, OneBlade Up, OneBlade Bottom);
	
	void Draw() const;

	void GetRandomPos();
};