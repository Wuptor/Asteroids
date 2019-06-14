#pragma once
#include <algorithm>
#include "Animation.h"
//#include "Missile.h"


class Asteroid : public Object
{
public:
	static std::vector<Asteroid*> asteroids;
	Asteroid(float _posX, float _posY, int _wah);
	~Asteroid();
	float distance;
	SDL_Rect DrawObject = { (int)posX, (int)posY, width, height };
	Animation *anim;
	bool alreadySplit;
	std::vector<int> targedIDS; // test for multiple homing missiles
	int targetID, asteroidsInside, fx, fy; //asteroids inside evtl unnötig
	void update() override;
	void Split();
	void CheckCollissionWithMissile();
	void SpawnAsteroid(float _posX, float _posY);
};
