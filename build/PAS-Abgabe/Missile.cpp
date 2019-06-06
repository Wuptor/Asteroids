#include "pch.h"
#include "Missile.h"

std::vector<Missile*> Missile::missiles;

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

Missile::~Missile()
{

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

void Missile::TriggerSplitshot()
{
	if (alreadysplit == false)
	{
		int ra = (int)rand() % 4 + 4; //splitcount noch woanders hinpacken //P.splitCount
		for (int i = 0; i < ra; i++)
		{
			Missile *m1 = new Missile((int)posX, (int)posY, rand() % 360);
			m1->alreadysplit = true;
			Missile::missiles.push_back(m1);
		}
	}
}

void Missile::SearchForTarget(std::vector<Asteroid*> list)
{
	if (enemyID == 0)
	{
		int index = -1;
		float distance = FLT_MAX;
		for (int i = 0; i < list.size(); i++)
		{
			if (list.at(i)->targetID == 0)
			{
				list.at(i)->distance = sqrt(pow(posX - list.at(i)->posX, 2) + pow(posY - list.at(i)->posY, 2));
				if (list.at(i)->distance <= distance)
				{
					index = i;
					list.at(index)->distance = sqrt(pow(posX - list.at(index)->posX, 2) + pow(posY - list.at(index)->posY, 2));
					distance = list.at(index)->distance;
				}
			}
		}
		if (list.size() != 0 && index >= 0)
		{
			enemyID = list.at(index)->ID;
			list.at(index)->targetID = ID;
		}
	}
}

void Missile::SearchForTarget(std::vector<Enemy*> list)
{
	if (enemyID == 0)
	{
		int index = -1;
		float distance = FLT_MAX;
		for (int i = 0; i < list.size(); i++)
		{
			if (list.at(i)->targetID == 0)
			{
				list.at(i)->distance = sqrt(pow(posX - list.at(i)->posX, 2) + pow(posY - list.at(i)->posY, 2));
				if (list.at(i)->distance <= distance)
				{
					index = i;
					list.at(index)->distance = sqrt(pow(posX - list.at(index)->posX, 2) + pow(posY - list.at(index)->posY, 2));
					distance = list.at(index)->distance;
				}
			}
		}
		if (list.size() != 0 && index >= 0)
		{
			enemyID = list.at(index)->ID;
			list.at(index)->targetID = ID;
		}
	}
}