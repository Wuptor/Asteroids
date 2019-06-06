#include "pch.h"
#include "Player.h"


Player::Player(SDL_Texture* _used, SDL_Texture* _damagedUsed) : splitCount(8), lifeCount(3), maxSpeed(5), Speed(1), floatingSpeed(0), keepRotation(0), turningSpeed(4), maxTurningSpeed(7), fireRate(25), fireCounter(0), shieldCounter(0),
shieldActieTime(0), maxHealth(10), maxFireRate(5),
thrust(false), doubleShot(false), tripleShot(false), quadrupleShot(false), splitShot(false), megaShot(false), targetSeeking(false), shieldBreak(false),
playerShield(false), shieldActice(false)
{
	radius = 15;
	posX = 200;
	posY = 100;
	width = 55;
	height = 61;
	NormalSprite = _used;
	DamagedSprite = _damagedUsed;
	Sprite = NormalSprite;
}


Player::~Player()
{
}

void Player::update()
{
	if (lifeCount == 1)
	{
		Sprite = DamagedSprite;
	}
	else {
		Sprite = NormalSprite;
	}
	if (playerShield == true)
	{
		shieldCounter++;
		if (shieldCounter > 360)
		{
			shieldActice = true;
			shieldActieTime++;
			if (shieldActieTime > 240)
			{
				shieldActice = false;
				shieldCounter = 0;
				shieldActieTime = 0;
			}
		}
	}
	if (thrust == true)
	{
		if (floatingSpeed > Speed)
		{
			floatingSpeed = Speed;
		}
		floatingSpeed += 1;
		keepRotation = rotation;
	}
	else
	{
		if (floatingSpeed > Speed / 60)
		{
			floatingSpeed -= Speed / 60;
		}
	}

	posX += floatingSpeed * (cos((keepRotation*M_PI) / 180));
	posY += floatingSpeed * (sin((keepRotation*M_PI) / 180));

	drawShield = { (int)posX, (int)posY, (height + 30), (height + 30) };
	DrawObject = { (int)posX, (int)posY, width, height };

	DrawObject.x = posX - DrawObject.w / 2;
	DrawObject.y = posY - DrawObject.h / 2;

	drawShield.x = posX - drawShield.w / 2;
	drawShield.y = posY - drawShield.h / 2;
	KeepInField();
}