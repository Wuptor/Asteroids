#include "pch.h"
#include "Asteroid.h"


Asteroid::Asteroid(float _x, float _y, int _wah, SDL_Texture* used) : asteroidsInside(2 + rand() % 3), Object(asteroid)
{
	posX = _x;
	posY = _y;
	width = _wah;
	height = _wah;
	rotation = rand() % 360;
	while (fx == 0 && fy == 0)
	{
		fx = rand() % 4 - 2;
		fy = rand() % 4 - 2;
	}
	radius = _wah * 0.4;
	anim = new Animation((int)posX, (int)posY, _wah, _wah, used, 7, 1, rotation, 6 + fx);
	anim->loop = true;
	//animations.push_back(anim);
	targetID = 0;
}


Asteroid::~Asteroid()
{
}

void Asteroid::update()
{
	posX += fx;
	posY += fy;
	DrawObject = { (int)posX, (int)posY, width, height };
	DrawObject.x = posX - DrawObject.w / 2;
	DrawObject.y = posY - DrawObject.h / 2;
	anim->updatePos(posX, posY, rotation);
	KeepInField();
}