#pragma once
#include "Object.h" 
class Missile : public Object //evtl eMissile und Missile zusammenführen --> Missile ist basically eMissile aber trifft den spieler und hat ne andere animation/sprite
{
public:
	Missile(float X, float Y, int ROTATION);
	~Missile();
	bool alreadysplit, homing;
	float direcx, direcy, mhyp, dist, mrotation, length, eposX, eposY;
	static int missileSpeed, maxMissileSpeed;
	int enemyID;
	void update();
};
