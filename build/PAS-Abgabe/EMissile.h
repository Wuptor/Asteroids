#pragma once
#include "Object.h"

class EMissile : public Object //basically missile --> zusammenführen //todo missile == emissile
{
public:
	EMissile(float _x, float _y, int _rotation);
	~EMissile();
	float eMissileSpeed;
	static std::vector<EMissile*> eMissiles;
	void update();
};
