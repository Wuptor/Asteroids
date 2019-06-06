#include "pch.h"
#include "Object.h"


Object::Object() : ID(0), alive(true), posX(0), posY(0), width(0), height(0), rotation(0), radius(0)
{
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