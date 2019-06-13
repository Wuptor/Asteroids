#pragma once
#include <algorithm>
#include "Animation.h"

class Asteroid : public Object
{
public:
	static std::vector<Asteroid*> asteroids;
	Asteroid(float _x, float _y, int _wah, SDL_Texture* used);
	~Asteroid();
	float distance;
	SDL_Rect DrawObject = { (int)posX, (int)posY, width, height };
	Animation *anim;
	std::vector<int> targedIDS; // test for multiple homing missiles
	int targetID, asteroidsInside, fx, fy; //asteroids inside evtl unnötig
	void update();
};
