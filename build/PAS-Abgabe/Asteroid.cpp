#include "pch.h"
#include "Asteroid.h"

std::vector<Asteroid*> Asteroid::asteroids;

Asteroid::Asteroid(float _posX, float _posY, int _wah) : asteroidsInside(2 + rand() % 3), Object(asteroid)
{
	posX = _posX;
	posY = _posY;
	width = _wah;
	height = _wah;
	rotation = rand() % 360;
	while (fx == 0 && fy == 0)
	{
		fx = rand() % 4 - 2;
		fy = rand() % 4 - 2;
	}
	radius = 75 * 0.4;
	SDL_Texture* AnimationSheet;
	int randomAnim = rand() % 2;
	if (randomAnim == 0)
	{
		AnimationSheet = ResourceDatabase::Textures["AsteroidAnim1"];
	}
	else
	{
		AnimationSheet = ResourceDatabase::Textures["AsteroidAnim2"];
	}
	alreadySplit = false;
	anim = new Animation((int)posX, (int)posY, width, height, AnimationSheet, 7, 1, rotation, 6 + fx);
	anim->loop = true;
	//Animation::animations.push_back(anim);
	//animations.push_back(anim);
	targetID = 0;
	SpawnAsteroid(_posX, _posY);
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
	CheckCollissionWithMissile();
}

void Asteroid::CheckCollissionWithMissile()
{
	if (alive == true)
	{
		for (Object *object : Object::Entities)
		{
			if (object->mObjectType == missile && object->alive == true && CheckCollision(object))
			{
				alive = false;
				object->alive = false;
				//score increase an anderer stelle --> jedes mal wenn ein object am ende gelöscht wird
				anim->playing = false; //geht bestimmt auch noch besser
				Animation* a = new Animation((int)posX - ((width + 30) / 2), (int)posY - ((width + 30) / 2), width + 30, height + 30, ResourceDatabase::Textures["AsteroidDestruction"], 4, 1, 0, 5);
				Animation::animations.push_back(a);
				//triggersplitshot --> in missile
			}
		}
	}
	if(alive == false)
	{
		if (alreadySplit == false)
		{
			Split();
		}
	}
}

void Asteroid::Split()
{
	for (int i = 0; i < asteroidsInside; i++)
	{
		Asteroid *a = new Asteroid(posX, posY, 40);
		a->alreadySplit = true; //nur testweise
		Object::Entities.push_back(a);
	}
}

void  Asteroid::SpawnAsteroid(float _posX = NULL, float _posY = NULL) //muss noch überarbeitet werden --> sehr bad
{
	if (_posX == NULL || _posY == NULL)
	{
		float x = rand() % SCREEN_WIDTH;
		float y = rand() % SCREEN_HEIGHT;
		if (fx > 0 && fy == 0)
		{
			posX = -width;
		}
		else if (fx < 0 && fy == 0)
		{
			posX = SCREEN_WIDTH; //müsste eigentlich keinen unterschied machen
		}
		else if (fy > 0 && fx == 0)
		{
			posY = -width;
		}
		else if (fy < 0 && fx == 0)
		{
			posY = SCREEN_HEIGHT;
		}
		else
		{
			int random = rand() % 4;
			if (random == 0)
			{
				posX = -width;
			}
			if (random == 1)
			{
				posY = -width;
			}
			if (random == 2)
			{
				posX = SCREEN_WIDTH;
			}
			if (random == 3)
			{
				posY = SCREEN_HEIGHT;
			}
		}
		anim->updatePos(posX, posY, rotation);
		Animation::animations.push_back(anim);
	}
	else
	{
		posX = _posX;
		posY = _posY;
		anim->updatePos(posX, posY, rotation);
		Animation::animations.push_back(anim);
	}
}