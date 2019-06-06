#include "pch.h"
#include "Missile.h"


Missile::Missile(float X, float Y, int ROTATION)
{
	if (X == NULL || Y == NULL) //warum?
	{
		posX = 100;
		posY = 100;
	}
	posX = X + (int)(15 * cos(ROTATION * float(M_PI) / 180.0f));
	posY = Y + (int)(15 * sin(ROTATION * float(M_PI) / 180.0f));
	rotation = ROTATION;
	radius = 15;
	alreadysplit = false;
	homing = true;
	enemyID = 0;
}


void Missile::update() {
	DrawObject = { (int)posX, (int)posY, 15, 20 };
	direcx = (cos((rotation*M_PI) / 180));
	direcy = (sin((rotation*M_PI) / 180));
	posX += missileSpeed * direcx;
	posY += missileSpeed * direcy;
	if (enemyID != 0)
	{
		float difx = eposX - posX;
		float dify = eposY - posY;
		float dif = sqrt(pow(difx, 2) + pow(dify, 2));

		float perpx = -direcy;
		float perpy = direcx;
		mrotation = atan2((perpx * difx + perpy * dify) / dif, (direcx * difx + direcy * dify) / dif);
		mrotation = mrotation * 180 / 3.14;
		rotation += mrotation / 10;
	}

	DrawObject.x = posX - DrawObject.w / 2;
	DrawObject.y = posY - DrawObject.h / 2;
	DeleteOnScreenExit();
}