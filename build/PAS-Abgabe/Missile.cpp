#include "pch.h"
#include "Missile.h"

std::vector<Missile*> Missile::missiles;

Missile::Missile(float X, float Y, int ROTATION) : Object(missile)
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
	homing = true;  //homing ist irgendwie sinnlos
	enemyID = 0;
	testLeftoverCounter = 0;
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
			/*
			Object _missile(missile);
			_missile.SetPosAndRot((int)posX, (int)posY, rand() % 360);
			Object::Entities->push_back(_missile);
			*/
		}
	}
}

/*
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
*/

void Missile::SearchForTarget(std::vector<Asteroid*> list) //fehler wenn liste size == 0 und alle verfolgen das nächste ziel
{
	if (list.size() == 0)
	{
		return;
	}
	if (enemyID == 0)
	{
		int index = -1;
		float distance = FLT_MAX;

		if (CheckAsteroids(list))
		{
			std::cout << "Checkpoint 1" << "\n";
			for (int i = 0; i < list.size(); i++)
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
		else
		{
			std::cout << "Checkpoint 2" << "\n";
			int ListSize = BiggestTargetList(list);
			std::cout << "Listsize ist : " << ListSize << "\n";
			for (int i = 0; i < list.size(); i++)
			{
				if (list.at(i)->targedIDS.size() < ListSize)
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
		}
		if (list.size() != 0 && index >= 0)
		{
			enemyID = list.at(index)->ID;
			//list.at(index)->targetID = ID;
			list.at(index)->targedIDS.push_back(ID);
		}
	}
}

bool Missile::CheckAsteroids(std::vector<Asteroid*> list)
{
	bool test = false;
	for (int i = 0; i < list.size()-1; i++)
	{
		if (list.at(i)->targedIDS.size() == list.at(i + 1)->targedIDS.size())
		{
			test = true;
			//std::cout << "hier" << "\n";
		}
		else
		{
			test = false;
			//std::cout << "test" << "\n";
		}
	}
	return test;
}

int Missile::BiggestTargetList(std::vector<Asteroid*> list)
{
	int biggestSize = -1;
	for (int i = 0; i < list.size(); i++)
	{
		std::cout << "list size: " << list.at(i)->targedIDS.size() << "\n";
		if ((int)list.at(i)->targedIDS.size() > biggestSize)
		{
			biggestSize = list.at(i)->targedIDS.size();
  			std::cout << "test" << "\n";
		}
	}
	std::cout << "biggest size: " << biggestSize << "\n";
	return biggestSize;
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
