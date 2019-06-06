#pragma once
#include "Object.h"

class Player : public Object
{
public:
	Player(SDL_Texture* _used, SDL_Texture* _damagedUsed);
	~Player();
	bool thrust, doubleShot, tripleShot, quadrupleShot, splitShot, megaShot, targetSeeking, shieldBreak, playerShield, shieldActice;
	float maxSpeed, Speed, floatingSpeed, turningSpeed, maxTurningSpeed, killRadius;
	int fireCounter, fireRate, lifeCount, splitCount, shieldCounter, shieldActieTime, maxHealth, maxFireRate, keepRotation;
	SDL_Rect drawShield;
	SDL_Texture* Sprite;
	SDL_Texture* NormalSprite;
	SDL_Texture* DamagedSprite;
	void update();
};
