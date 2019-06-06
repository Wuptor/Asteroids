#pragma once
#include "Player.h"
#include "EMissile.h"

class Enemy : public Object //lifecount = 2
{
public:
	Enemy(int EnemyType);
	~Enemy();
	bool rotating, shielded, moving, turning, firstTurn, startCalculations;
	float edirx, ediry, dist, espeed, ehyp, randomRotation, angleCopy, randomSpawnPos, xcounter, ycounter, distToSource, distToEnd, startrot, endrot, movedirecx, movedirecy, randomLength, distance;
	int movementCounter, firerateCounter, eType, lifecount, targetID;
	SDL_Texture* Used, *UsedShield, *Damaged;
	SDL_Rect DrawShield;
	void update(Player P);
	void SpawnPlayerTargetAlien(SDL_Texture* _used, SDL_Texture* _useds, SDL_Texture* _damaged);
	void SpawnRapidFireAlien(SDL_Texture* _used, SDL_Texture* _useds, SDL_Texture* _damaged);
	void SpawnRapidRandomShotAlien(SDL_Texture* _used, SDL_Texture* _useds, SDL_Texture* _damaged);
	void SpawnHalfCircleShotAlien(SDL_Texture* _used, SDL_Texture* _useds, SDL_Texture* _damaged);
	void EnemyMovementPattern1(Object P);
	void EnemyMovementPattern2();
	void EnemyMovementPattern3();
	void EnemyMovementPattern4(Object P);
	void EnemyMovementPattern5();
};

