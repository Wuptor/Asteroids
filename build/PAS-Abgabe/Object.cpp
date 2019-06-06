#include "pch.h"
#include "Object.h"

int Object::ObjectID = 0;

Object::Object() : ID(ObjectID), alive(true), posX(0), posY(0), width(0), height(0), rotation(0), radius(0)
{
	ObjectID++;
}


Object::~Object()
{
}

void Object::KeepInField() //sets position for objects that exit screen
{
	if (posX < -width)
		posX = SCREEN_WIDTH;
	if (posY < -height)
		posY = SCREEN_HEIGHT;
	if (posX > SCREEN_WIDTH)
		posX = 0;
	if (posY > SCREEN_HEIGHT)
		posY = 0;
}

void Object::DeleteOnScreenExit() //sets life = false for objects that exit screen
{
	if (posX < -width || posX > SCREEN_WIDTH || posY < -height || posY > SCREEN_HEIGHT) {
		alive = false;
	}
}

bool Object::CheckCollision(Object* _other)
{
	if ((posX - _other->posX)*(posX - _other->posX) + (posY - _other->posY)* (posY - _other->posY) <= (radius + _other->radius)*(radius + _other->radius))
	{
		return true;
	}
	return false;
}