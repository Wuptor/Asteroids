#include "pch.h"
#include "Object.h"

int Object::ObjectID = 1; //object id usw fixen
std::vector<Object>* Object::Entities;

Object::Object(Type _type) : ID(ObjectID), alive(true), posX(0), posY(0), width(0), height(0), rotation(0), radius(0), mObjectType(_type)
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

void Object::update() //evtl in dem base update auchw as ausführen
{

}

void Object::SetPosAndRot(int _posX, int _posY, int _rotation)
{
	posX = _posX;
	posY = _posY;
	rotation = _rotation;
}