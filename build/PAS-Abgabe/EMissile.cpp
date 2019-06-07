#include "pch.h"
#include "EMissile.h"

std::vector<EMissile*> EMissile::eMissiles;

EMissile::EMissile(float _x, float _y, int _rotation) : eMissileSpeed(2), Object(emissile)
{
	width = 20;
	height = 20;
	radius = 10;
	posX = _x + (int)(20 * cos(_rotation * float(M_PI) / 180.0f));
	posY = _y + (int)(20 * sin(_rotation * float(M_PI) / 180.0f));
	rotation = _rotation;
}


EMissile::~EMissile()
{
}

void EMissile::update() {

	posX += eMissileSpeed * (cos((rotation*M_PI) / 180));
	posY += eMissileSpeed * (sin((rotation*M_PI) / 180));
	DrawObject = { (int)posX, (int)posY, width, height };

	DrawObject.x = posX - DrawObject.w / 2;
	DrawObject.y = posY - DrawObject.h / 2;
	DeleteOnScreenExit();
}
