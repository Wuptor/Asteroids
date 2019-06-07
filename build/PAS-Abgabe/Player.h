#pragma once
#include "Object.h"
#include "ResourceDatabase.h"
#include "Missile.h"

class Player : public Object
{
public:
	enum WeaponType {Normal, DoubleShot, TripleShot, QuadrupleShot };
	Player(SDL_Texture* _used, SDL_Texture* _damagedUsed);
	~Player();
	bool thrust, splitShot, megaShot, targetSeeking, shieldBreak, playerShield, shieldActice, hitByEnemy;
	float maxSpeed, Speed, floatingSpeed, turningSpeed, maxTurningSpeed, killRadius;
	int fireCounter, fireRate, lifeCount, splitCount, shieldCounter, shieldActieTime, maxHealth, maxFireRate, keepRotation;
	SDL_Rect drawShield;
	SDL_Texture* Sprite;
	SDL_Texture* NormalSprite;
	SDL_Texture* DamagedSprite;
	WeaponType mWeaponType;
	std::vector<Object> playerHealth;
    void update() override;
	void handleInput(); //irgendwann auf inputmanager erweitern
	void TriggerKillRadius(std::vector<Object*> list, float killradius);
};
