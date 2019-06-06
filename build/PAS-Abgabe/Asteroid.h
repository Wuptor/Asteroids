#pragma once
#include <algorithm>
#include "Animation.h"

class Asteroid : public Object
{
public:
	Asteroid(float _x, float _y, int _wah, SDL_Texture* used);
	~Asteroid();
	float distance;
	SDL_Rect DrawObject = { (int)posX, (int)posY, width, height };
	Animation *anim;
	Object *targeted;
	int targetID, asteroidsInside, fx, fy;
	void update();
};
