#include "pch.h"
#include "Enemy.h"


Enemy::Enemy(int EnemyType) : targetID(0), lifecount(2), startCalculations(true), startrot(0), endrot(0), movedirecx(0), movedirecy(0), distToEnd(0), distToSource(0), firstTurn(true), moving(false),
edirx(0), ediry(0), dist(0), espeed(0), ehyp(0), movementCounter(0), firerateCounter(0), shielded(false), rotating(true), turning(true), randomSpawnPos(0), angleCopy(0), randomRotation(0),
randomLength(0), Object(enemy)
{
	rotation = 0;
	xcounter = 0;
	ycounter = 0;
	width = 65;
	height = 65;
	eType = EnemyType;
	radius = 25;
}


Enemy::~Enemy()
{
}


void Enemy::update(Object P)
{
	if (lifecount == 1)
	{
		Used = Damaged;
	}
	if (eType == 0)
	{
		EnemyMovementPattern1(P);
	}
	if (eType == 1)
	{
		EnemyMovementPattern2();
	}
	if (eType == 2)
	{
		EnemyMovementPattern3();
	}
	if (eType == 3)
	{
		EnemyMovementPattern4(P);
	}
	if (eType == 4)
	{
		EnemyMovementPattern5();
	}
	DrawShield = { (int)posX, (int)posY, (width + 30), (height + 30) };
	DrawObject = { (int)posX, (int)posY, width, height };

	DrawObject.x = posX - DrawObject.w / 2;
	DrawObject.y = posY - DrawObject.h / 2;

	DrawShield.x = posX - DrawShield.w / 2;
	DrawShield.y = posY - DrawShield.h / 2;

}
void Enemy::SpawnPlayerTargetAlien(SDL_Texture* _used, SDL_Texture* _useds, SDL_Texture* _damaged)
{
	randomSpawnPos = rand() % 4;
	if (randomSpawnPos == 0) //obere linie
	{
		posX = 37 + rand() % (SCREEN_WIDTH - 75);
		posY = 0 + 37;
	}
	if (randomSpawnPos == 1) // linke linie
	{
		posX = 0 + 37;
		posY = 37 + rand() % (SCREEN_HEIGHT - 75);
	}
	if (randomSpawnPos == 2) //untere linie
	{
		posX = 37 + rand() % (SCREEN_WIDTH - 75);
		posY = 37 + (SCREEN_HEIGHT - 75);
	}
	if (randomSpawnPos == 3) //rechte linie
	{
		posX = 37 + (SCREEN_WIDTH - 75);
		posY = 37 + rand() % (SCREEN_HEIGHT - 75);
	}
	Used = _used;
	UsedShield = _useds;
	Damaged = _damaged;
	while (randomRotation == 0)
	{
		randomRotation = rand() % 4 - 2;
	}
}
void Enemy::SpawnRapidFireAlien(SDL_Texture* _used, SDL_Texture* _useds, SDL_Texture* _damaged)
{
	randomSpawnPos = rand() % 4;
	if (randomSpawnPos == 0)
	{
		posX = 37 + rand() % (SCREEN_WIDTH - 75);
		rotation = 90;
		posY = 0 + 12;
	}
	if (randomSpawnPos == 1)
	{
		posX = 0 + 12;
		rotation = 0;
		posY = 37 + rand() % (SCREEN_HEIGHT - 75);
	}
	if (randomSpawnPos == 2)
	{
		posX = 37 + rand() % (SCREEN_WIDTH - 75);
		rotation = 270;
		posY = SCREEN_HEIGHT - 13;
	}
	if (randomSpawnPos == 3)
	{
		posX = SCREEN_WIDTH - 13;
		rotation = 180;
		posY = 37 + rand() % (SCREEN_HEIGHT - 75);
	}
	Used = _used;
	UsedShield = _useds;
	Damaged = _damaged;
	rotation += 180;
}
void Enemy::SpawnRapidRandomShotAlien(SDL_Texture* _used, SDL_Texture* _useds, SDL_Texture* _damaged)
{
	posX = 162 + rand() % 315;
	posY = 112 + rand() % 255;
	while (randomRotation == 0)
	{
		randomRotation = rand() % 4 - 2;
	}

	Used = _used;
	UsedShield = _useds;
	Damaged = _damaged;
}
void Enemy::SpawnHalfCircleShotAlien(SDL_Texture* _used, SDL_Texture* _useds, SDL_Texture* _damaged)
{
	randomSpawnPos = rand() % 2;
	if (randomSpawnPos == 0)
	{
		posX = 37 + rand() % (SCREEN_WIDTH - 75);
		posY = 187 + rand() % 105;
	}
	if (randomSpawnPos == 1)
	{
		posX = 287 + rand() % 65;
		posY = 37 + rand() % (SCREEN_HEIGHT - 75);
	}
	Used = _used;
	UsedShield = _useds;
	Damaged = _damaged;
}
void Enemy::EnemyMovementPattern1(Object P)
{
	rotation = atan2(P.posY - posY, P.posX - posX);
	rotation = rotation * 180 / 3.14;
	firerateCounter++;
	if (firerateCounter > 60)
	{
		EMissile *em = new EMissile(posX, posY, rotation);
		EMissile::eMissiles.push_back(em);
		firerateCounter = 0;
	}
}
void Enemy::EnemyMovementPattern2()
{
	shielded = true;
	firerateCounter++;
	if (firerateCounter > 90) {
		int randomMissileCount = rand() % 8 + 3;
		for (int i = 0; i < randomMissileCount; i++)
		{
			EMissile* em = new EMissile(posX, posY, rand() % 360);
			EMissile::eMissiles.push_back(em);
		}
		firerateCounter = 0;
	}
	rotation += randomRotation * 0.5;
}
void Enemy::EnemyMovementPattern3()
{
	if (moving == true)
	{
		shielded = true;
		movementCounter++;
		if (startCalculations == true)
		{
			if (randomSpawnPos == 0) //oben
			{
				distToSource = sqrt(pow(posX - 0, 2) + pow(posY - 0, 2));
				distToEnd = sqrt(pow(posX - SCREEN_WIDTH, 2) + pow(posY - 0, 2));
			}
			if (randomSpawnPos == 1) //linke
			{
				distToSource = sqrt(pow(posX - 0, 2) + pow(posY - 0, 2));
				distToEnd = sqrt(pow(posX - 0, 2) + pow(posY - SCREEN_HEIGHT, 2));
			}
			if (randomSpawnPos == 2) // unten
			{
				distToSource = sqrt(pow(posX - 0, 2) + pow(posY - SCREEN_HEIGHT, 2));
				distToEnd = sqrt(pow(posX - SCREEN_WIDTH, 2) + pow(posY - SCREEN_HEIGHT, 2));
			}
			if (randomSpawnPos == 3) //rechts
			{
				distToSource = sqrt(pow(posX - SCREEN_WIDTH, 2) + pow(posY - 0, 2));
				distToEnd = sqrt(pow(posX - SCREEN_WIDTH, 2) + pow(posY - SCREEN_HEIGHT, 2));
			}
			if (randomSpawnPos == 1 || randomSpawnPos == 2)
			{
				if (distToSource > distToEnd)
				{
					startrot = -1;
					endrot = 1;
				}
				else
				{
					startrot = 1;
					endrot = -1;
				}
			}
			if (randomSpawnPos == 0 || randomSpawnPos == 3)
			{
				if (distToSource > distToEnd)
				{
					startrot = +1;
					endrot = -1;
				}
				else
				{
					startrot = -1;
					endrot = +1;
				}
			}
			if (randomSpawnPos == 0 || randomSpawnPos == 2)
			{
				randomLength = rand() % (SCREEN_WIDTH - 75);
				if (distToSource > distToEnd)
				{
					movedirecx = -1;
					randomLength = rand() % (int)(distToSource - 75);
				}
				else
				{
					movedirecx = 1;
					randomLength = rand() % (int)(distToEnd - 75);
				}
			}
			if (randomSpawnPos == 1 || randomSpawnPos == 3)
			{
				if (distToSource > distToEnd)
				{
					movedirecy = -1;
					randomLength = rand() % (int)(distToSource - 75);
				}
				else
				{
					movedirecy = 1;
					randomLength = rand() % (int)(distToEnd - 75);
				}
			}
			startCalculations = false;
		}
		if (movementCounter < (255 + randomLength))
		{
			if (turning == true && angleCopy < 90 && firstTurn == true)
			{
				angleCopy += 1;
				rotation += startrot;
			}
			else
			{
				turning = false;
				firstTurn = false;
				ycounter++;
			}
			if (turning == false && ycounter < (75 + randomLength))
			{
				posX += movedirecx;
				posY += movedirecy;
				angleCopy = 0;
			}
			else
			{
				turning = true;
			}
			if (turning == true && angleCopy < 90 && firstTurn == false)
			{
				rotation += endrot;
				angleCopy += 1;
			}
		}
		else
		{
			moving = false;
			angleCopy = 0;
			ycounter = 0;
			firstTurn = true;
			movementCounter = 0;
			shielded = false;
		}
	}
	if (moving == false)
	{
		firerateCounter++;
		if (firerateCounter > 10)
		{
			EMissile* em = new EMissile(posX, posY, rotation + 180);
			EMissile::eMissiles.push_back(em);
			firerateCounter = 0;
			xcounter++;
		}
		if (xcounter > 30)
		{
			xcounter = 0;
			moving = true;
			startCalculations = true;
		}
	}
}
void Enemy::EnemyMovementPattern4(Object P)
{
	if (P.alive == true)
	{
		edirx = P.posX - posX;
		ediry = P.posY - posY;

		ehyp = sqrt(edirx*edirx + ediry * ediry);
		edirx /= ehyp;
		ediry /= ehyp;

		posX += edirx * espeed;
		posY += ediry * espeed;

		dist = sqrt(pow(posX - P.posX, 2) + pow(posY - P.posY, 2));
		if (dist < 200)
		{
			if (espeed < 7)
			{
				espeed += 0.05;
			}
			rotation += 2 * espeed;
			shielded = false;
		}
		else
		{
			espeed = 2;
			rotation += 2;
			shielded = true;
		}
	}
}
void Enemy::EnemyMovementPattern5()
{
	randomRotation = (rand() % 720 + 240);
	if (rotating == true)
	{
		if (angleCopy < randomRotation)
		{
			rotation += 1;
			angleCopy += 1;
			shielded = true;
			movementCounter = 0;
		}
		else
		{
			rotating = false;
			shielded = false;
		}
	}
	if (rotating == false)
	{
		firerateCounter++;
		if (firerateCounter > 30)
		{
			for (int i = 0; i < 9; i++)
			{
				EMissile* em = new EMissile(posX, posY, (rotation + i * 20) - 85);
				EMissile::eMissiles.push_back(em);
			}
			firerateCounter = 0;
			movementCounter++;
		}
		if (movementCounter > 5)
		{
			rotating = true;
			angleCopy = 0;
		}
	}
}