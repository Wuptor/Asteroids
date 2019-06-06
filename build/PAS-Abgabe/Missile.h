#pragma once
#include "Object.h" 
#include "Asteroid.h"
#include "Enemy.h"

class Missile : public Object //evtl eMissile und Missile zusammenführen --> Missile ist basically eMissile aber trifft den spieler und hat ne andere animation/sprite
{
public:
	Missile(float X, float Y, int ROTATION);
	~Missile();
	static std::vector<Missile*> missiles;
	bool alreadysplit, homing;
	float direcx, direcy, mhyp, dist, mrotation, length, eposX, eposY;
	static int missileSpeed, maxMissileSpeed;
	int enemyID;
	void update();
	void TriggerSplitshot();
	void SearchForTarget(std::vector<Asteroid*> list);
	void SearchForTarget(std::vector<Enemy*> list);
};
