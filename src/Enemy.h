#pragma once
#include "FlyFish.h"
#include "structs.h"

class Enemy
{
private:
	ThreeBlade m_Position{};
	

public:
	Enemy();
	Enemy(const Enemy& other) = delete;
	Enemy& operator= (const Enemy& other) = delete;
	Enemy(Enemy&& other) = delete;
	Enemy& operator=(Enemy&& other) = delete;

	~Enemy();

	void Update();
	
	void Draw();
};