
#include "pch.h"
#include <cmath> // u.a. Trigonometrie
#include <iostream> // Konsole Ein-/Ausgabe
#include <vector> // dynamischer Array
#include <algorithm>
#include <iterator>
#include <string>

#include "SDL.h"
#include <SDL_ttf.h>
#include "ResourceDatabase.h"
//#include "Asteroid.h"
#include "Player.h"
//#include "Missile.h"
#include "Pickup.h"
#include "Button.h"
//#include "Enemy.h"

//missing game manager
//missile manager usw?
//alle funktionen in die jeweilig passenden klassen auslagern
//timescale = 1 auf alle timebasierten sachen drauf multiplizieren --> für timeslow
//frameanzahl unabhängig arbeiten
//enemy manager?
//überlegen wie des mit den ganzen listen am besten gelöst werden kann
//als erste lösung: alle listen static machen
//big bugs:: asteroid spawn place, automatic restart, //life canister --> done aber noch nicht optimal
//alle variablen leichter resetbar machen
//double life loss bug lösen --> fürs erste denke ich gelöst
//gleichzeitig gas und lenken --> am anfang des frames prüfen welche knöpfe gedrückt sind
//links lenken und schießen geht wegen ghosting nicht --> evlt versuchen zu tricksen oder alternative tastaturbelegung erlauben#
//file read and write
//überlegen ob/wie es sinn macht alle objects über eine list zu verwalten
//bei aktivem shield den kollission radius größer machen

#define SCREEN_WIDTH  640 
#define SCREEN_HEIGHT 480
#define SPRITE_SIZE    32

static int ObjectID = 1;
int score = 0;
int enemynumber = 1;
bool pause;
int temporaryScore = 0;

std::vector<Asteroid*> asteroids;
std::vector<Pickup*> pickups;
std::vector<Enemy*> enemies;

bool CheckCollisionHCircle(Object* one, Object *second) //auch in object was ist one und was ist second?
{

	float b1 = second->posX - one->posX;
	float b2 = second->posY - one->posY;
	float nx = -cos(second->rotation*M_PI / 180);
	float ny = -sin(second->rotation*M_PI / 180);
	float o1 = b1;
	float o2 = b2;
	float result = nx * o1 + ny * o2;
	if (result < 0)
		return true;
	return false;

}


void TriggerKillRadius(std::vector<Asteroid*> list, Player P, float killradius)
{
	Object Deathcircle(Object::Type::neutral);
	Deathcircle.posX = P.posX;
	Deathcircle.posY = P.posY;
	Deathcircle.radius = killradius;
	for (Asteroid* o : list)
	{
		if (o->CheckCollision(&Deathcircle))
		{
			o->alive = false;
			o->anim->playing = false;
		}
	}
}

void TriggerKillRadius(std::vector<EMissile*> list, Player P, float killradius)
{
	Object Deathcircle(Object::Type::neutral);
	Deathcircle.posX = P.posX;
	Deathcircle.posY = P.posY;
	Deathcircle.radius = killradius;
	for (EMissile* o : list)
	{
		if (o->CheckCollision(&Deathcircle))
			o->alive = false;
	}

}

void TriggerKillRadius(std::vector<Enemy*> list, Player P, float killradius)
{
	Object Deathcircle(Object::Type::neutral);
	Deathcircle.posX = P.posX;
	Deathcircle.posY = P.posY;
	Deathcircle.radius = killradius;
	for (Enemy* o : list)
	{
		if (o->CheckCollision(&Deathcircle))
			o->alive = false;
	}

}


void SpawnAsteroids(Player P, int _aCount, float _posX, float _posY, int _wah, SDL_Texture * _used1, SDL_Texture* _used2)
{
	SDL_Texture* UsedAnim;

	if (_posX == NULL || _posY == NULL)
	{
		for (int i = 0; i < _aCount; i++)
		{
			int randomAnim = rand() % 2;
			if (randomAnim == 0)
			{
				UsedAnim = _used1;
			}
			else
			{
				UsedAnim = _used2;
			}
			float x = rand() % SCREEN_WIDTH;
			float y = rand() % SCREEN_HEIGHT;
			Asteroid *a = new Asteroid(x, y, _wah, UsedAnim);
			if (a->fx > 0 && a->fy == 0)
			{
				a->posX = -a->width;
			}
			else if(a->fx < 0 && a->fy == 0)
			{
				a->posX = SCREEN_WIDTH; //müsste eigentlich keinen unterschied machen
			}
			else if (a->fy > 0 && a->fx == 0)
			{
				a->posY = -a->width;
			}
			else if(a->fy < 0 && a->fx == 0)
			{
				a->posY = SCREEN_HEIGHT;
			}
			else
			{
				int random = rand() % 4;
				if (random == 0)
				{
					a->posX = -a->width;
				}
				if (random == 1)
				{
					a->posY = -a->width;
				}
				if (random == 2)
				{
					a->posX = SCREEN_WIDTH;
				}
				if (random == 3)
				{
					a->posY = SCREEN_HEIGHT;
				}
			}
			/*
			float x = rand() % SCREEN_WIDTH;
			float y = rand() % SCREEN_HEIGHT;
			Asteroid *a = new Asteroid(x, y, _wah, UsedAnim);
			while (x == P.posX || y == P.posY)
			{
				x = rand() % SCREEN_WIDTH;
				y = rand() % SCREEN_HEIGHT;
			}
			*/
			a->anim->updatePos(a->posX, a->posY, a->rotation);
			Animation::animations.push_back(a->anim);
			asteroids.push_back(a);
		}
	}
	else //für die kleinen asteroids oder?
	{
		for (int i = 0; i < _aCount; i++)
		{
			int randomAnim = rand() % 2;
			if (randomAnim == 0)
			{
				UsedAnim = _used1;
			}
			else
			{
				UsedAnim = _used2;
			}
			Asteroid *a = new Asteroid(_posX, _posY, _wah, UsedAnim);
			Animation::animations.push_back(a->anim);
			asteroids.push_back(a);
		}
	}
}

void SpawnPickup(float _x, float _y, SDL_Texture* _used)
{
	if (pickups.size() <= 10)
	{
		if (rand() % 3 == 1)
		{
			Pickup *p = new Pickup(_x, _y, _used);
			pickups.push_back(p);
		}
	}
	else
	{
		pickups.erase(pickups.begin());
	}
}

int Missile::missileSpeed = 0;
int Missile::maxMissileSpeed = 0;
float Pickup::luck = 0;

int main(int, char **)
{
	const Uint8 *Keystate;
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	std::vector<Button*> buttons;
	pause = false;

	int fontsize = 24;
	int t_width = 0;
	int t_height = 0;
	SDL_Color text_color = { 255,255,255 };
	std::string fontpath = "../../assets/fonts/arialbd.ttf"; //arial irgendwas
	std::string text = "test";
	TTF_Font* font = TTF_OpenFont(fontpath.c_str(), fontsize);
	int textPosX = 0;
	int textPosY = 0;
	SDL_Rect dst = { textPosX , textPosY, t_width, t_height };

	SDL_Window * Window = SDL_CreateWindow("Asteroides", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Renderer * Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_PRESENTVSYNC);

	ResourceDatabase::Textures.insert(std::make_pair("enemyHShield", ResourceDatabase::ILoadImage("../../assets/HalfShield.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("enemyShield", ResourceDatabase::ILoadImage("../../assets/EnemyShield.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("playerShield", ResourceDatabase::ILoadImage("../../assets/PlayerShield.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("missile", ResourceDatabase::ILoadImage("../../assets/PlayerMissile.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("enemy1", ResourceDatabase::ILoadImage("../../assets/Enemy_01.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("enemy2", ResourceDatabase::ILoadImage("../../assets/Enemy_02.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("enemy3", ResourceDatabase::ILoadImage("../../assets/Enemy_03.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("enemy4", ResourceDatabase::ILoadImage("../../assets/Enemy_04.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("enemy5", ResourceDatabase::ILoadImage("../../assets/Enemy_05.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("damagedEnemy1", ResourceDatabase::ILoadImage("../../assets/Enemy_01Damaged.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("damagedEnemy2", ResourceDatabase::ILoadImage("../../assets/Enemy_02Damaged.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("damagedEnemy3", ResourceDatabase::ILoadImage("../../assets/Enemy_03Damaged.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("damagedEnemy4", ResourceDatabase::ILoadImage("../../assets/Enemy_04Damaged.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("damagedEnemy5", ResourceDatabase::ILoadImage("../../assets/Enemy_05Damaged.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("damagedPlayer", ResourceDatabase::ILoadImage("../../assets/DamagedPlayer.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("eMissile", ResourceDatabase::ILoadImage("../../assets/EnemyMissile.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("ExplosionAnim", ResourceDatabase::ILoadImage("../../assets/Explosion.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("AsteroidAnim1", ResourceDatabase::ILoadImage("../../assets/Asteroid_01.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("AsteroidAnim2", ResourceDatabase::ILoadImage("../../assets/Asteroid_02.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("AsteroidDestruction", ResourceDatabase::ILoadImage("../../assets/AsteroidDestructionAnim.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("playerShip", ResourceDatabase::ILoadImage("../../assets/PlayerSprite.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("shockWaveAnim", ResourceDatabase::ILoadImage("../../assets/ShockWave.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("Pickups", ResourceDatabase::ILoadImage("../../assets/PickupsSpritesheet.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("background", ResourceDatabase::ILoadImage("../../assets/Background.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("ButtonHighlight", ResourceDatabase::ILoadImage("../../assets/GreyOverlay.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("QuitButton", ResourceDatabase::ILoadImage("../../assets/QuitButton.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("PlayButton", ResourceDatabase::ILoadImage("../../assets/PlayButton.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("RestartButton", ResourceDatabase::ILoadImage("../../assets/RestartButton.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("startscreen", ResourceDatabase::ILoadImage("../../assets/startscreen.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("pHealthFull", ResourceDatabase::ILoadImage("../../assets/PlayerHealthFull.bmp", Renderer)));
	ResourceDatabase::Textures.insert(std::make_pair("pHealthEmpty", ResourceDatabase::ILoadImage("../../assets/PlayerHealthEmpty.bmp", Renderer)));

	ResourceDatabase::Textures.insert(std::make_pair("TargetSeekingLeftovers", ResourceDatabase::ILoadImage("../../assets/AsteroidsTargetSeeking.bmp", Renderer)));

	//ResourceDatabase::Textures.insert(std::make_pair("PlayerSpriteSheet", ResourceDatabase::ILoadImage("../../assets/PlayerSpriteSheet.bmp", Renderer)));

	Button* a = new Button(1, 220, 250, 200, 50, ResourceDatabase::Textures["PlayButton"], ResourceDatabase::Textures["ButtonHighlight"]);
	buttons.push_back(a);
	Button* b = new Button(2, 220, 330, 200, 50, ResourceDatabase::Textures["QuitButton"], ResourceDatabase::Textures["ButtonHighlight"]);
	buttons.push_back(b);
	Button* c = new Button(3, 220, 250, 200, 50, ResourceDatabase::Textures["RestartButton"], ResourceDatabase::Textures["ButtonHighlight"]);
	buttons.push_back(c);

	bool Running = true;
	bool MainMenu = true;
	bool RestartMenu = false;
	bool MainGame = false;
	bool PreGame = false;
	while (Running)
	{
		while (MainMenu)
		{
			for (Button* b : buttons)
			{
				b->Update();
				if (b->ID == 1 && EventManager::ButtonClicked(b->DrawObject))
				{
					PreGame = true;
					MainMenu = false;
				}
				if (b->ID == 2 && EventManager::ButtonClicked(b->DrawObject))
				{
					Running = false;
					MainMenu = false;
				}
			}
			SDL_Event Event;
			while (SDL_PollEvent(&Event))
			{
				switch (Event.type)
				{
				case SDL_QUIT:
					Running = false;
					MainMenu = false;
					break;
				}
			}
			SDL_RenderCopy(Renderer, ResourceDatabase::Textures["startscreen"], NULL, NULL);
			for (Button* b : buttons)
			{
				if (b->ID == 1 || b->ID == 2)
					SDL_RenderCopy(Renderer, b->currentlyUsed, NULL, &b->DrawObject);
			}
			SDL_RenderPresent(Renderer);
		}
		while (RestartMenu)
		{
			for (Button* b : buttons)
			{
				b->Update();
				if (b->ID == 3 && EventManager::ButtonClicked(b->DrawObject))
				{
					PreGame = true;
					RestartMenu = false;
				}
				if (b->ID == 2 && EventManager::ButtonClicked(b->DrawObject))
				{
					Running = false;
					RestartMenu = false;
				}
			}
			SDL_Event Event;
			while (SDL_PollEvent(&Event))
			{
				switch (Event.type)
				{
				case SDL_QUIT:
					Running = false;
					RestartMenu = false;
					break;
				}
			}
			SDL_RenderCopy(Renderer, ResourceDatabase::Textures["startscreen"], NULL, NULL);
			for (Button* b : buttons)
			{
				if (b->ID == 2 || b->ID == 3)
					SDL_RenderCopy(Renderer, b->currentlyUsed, NULL, &b->DrawObject);
			}
			SDL_RenderPresent(Renderer);
		}
		if (PreGame) //resets all values
		{
			asteroids.clear();
			Missile::missiles.clear();
			enemies.clear();
			pickups.clear();
			EMissile::eMissiles.clear();
			Animation::animations.clear();
			Pickup::pickupn.clear();
			Pickup::pickupn = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			temporaryScore = 0;
			Player player(ResourceDatabase::Textures["playerShip"], ResourceDatabase::Textures["damagedPlayer"]);
			Missile::missileSpeed = 4;
			Missile::maxMissileSpeed = 8;
			Pickup::luck = 8;
			score = 0;
			int endGamgeCounter = 0;
			bool TriggerEndGame = false;
			int asteroidSpawnCounter = 0;
			int asteroidspawnTime = 0;
			enemynumber = 0;
			SpawnAsteroids(player, 5, NULL, NULL, 75, ResourceDatabase::Textures["AsteroidAnim1"], ResourceDatabase::Textures["AsteroidAnim2"]);
			MainGame = true;
			PreGame = false;
			while (MainGame)
			{
				player.update();
				/* 
				for (int i = 0; i < Object::Entities->size(); i++)
				{
					Object::Entities->at(i).update();
				}
				*/
				for (EMissile *em : EMissile::eMissiles)
				{
					em->update();
				}
				for (Missile *m : Missile::missiles)
				{
					m->update();
				}
				for (Asteroid* c : asteroids)
				{
					c->update();
				}
				for (Enemy* e : enemies)
				{
					e->update(player);
				}
				for (Animation* y : Animation::animations)
				{
					y->update();
				}
				

				if (rand() % 150 == 0)
				{
					asteroidSpawnCounter = asteroidSpawnCounter + enemynumber + 1;
				}
				if (asteroidSpawnCounter > 1)
				{
					asteroidspawnTime++;
					if (asteroidspawnTime > (15-enemynumber))
					{
						SpawnAsteroids(player, 1, NULL, NULL, 75, ResourceDatabase::Textures["AsteroidAnim1"], ResourceDatabase::Textures["AsteroidAnim2"]);
						asteroidSpawnCounter--;
						asteroidspawnTime = 0;
					}
				}
				if (score > 300)
				{
					score = 0;
					if (enemynumber < 10)
					{
						enemynumber++;
					}
				}
				if (enemies.size() < enemynumber) {
					if (rand() % 50 == 0)
					{
						int EnemyType = rand() % 5;
						Enemy* e = new Enemy(EnemyType);
						if (EnemyType == 0)
							e->SpawnPlayerTargetAlien(ResourceDatabase::Textures["enemy1"], ResourceDatabase::Textures["enemyShield"], ResourceDatabase::Textures["damagedEnemy1"]); //enemy that shoots at the player position
						if (EnemyType == 1)
							e->SpawnRapidRandomShotAlien(ResourceDatabase::Textures["enemy2"], ResourceDatabase::Textures["enemyHShield"], ResourceDatabase::Textures["damagedEnemy2"]); //enemy that shoots in random directions
						if (EnemyType == 2)
							e->SpawnRapidFireAlien(ResourceDatabase::Textures["enemy3"], ResourceDatabase::Textures["enemyShield"], ResourceDatabase::Textures["damagedEnemy3"]); //enemy that shoots a rapid bullet stream
						if (EnemyType == 3)
							e->SpawnPlayerTargetAlien(ResourceDatabase::Textures["enemy4"], ResourceDatabase::Textures["enemyShield"], ResourceDatabase::Textures["damagedEnemy4"]); //enemy that chases the player
						if (EnemyType == 4)
							e->SpawnHalfCircleShotAlien(ResourceDatabase::Textures["enemy5"], ResourceDatabase::Textures["enemyShield"], ResourceDatabase::Textures["damagedEnemy5"]); //enemy that fires in a half circle

						enemies.push_back(e);
					}
				}

				SDL_Event Event;
				while (SDL_PollEvent(&Event)) // wait until event occured
				{
					switch (Event.type)
					{
					case SDL_QUIT:
					{
						MainGame = false;
						Running = false;
						break;
					}
					case SDL_KEYDOWN:
					{
						Keystate = SDL_GetKeyboardState(NULL);
						if (Keystate[SDL_SCANCODE_P])
						{
							pause = true;
						}
						if (Keystate[SDL_SCANCODE_R])
						{
							MainGame = false;
							PreGame = true;
						}
						if (Keystate[SDL_SCANCODE_ESCAPE])
						{
							MainGame = false;
							Running = false;
						}
						break;
					}
					}
				}
			
				while (pause) //funktioniert noch nicht ganz optimal
				{
					SDL_Event Event;
					while (SDL_PollEvent(&Event)) // wait until event occured
					{
						switch (Event.type)
						{
						case SDL_QUIT:
						{
							MainGame = false;
							Running = false;
							pause = false;
							break;
						}
						case SDL_KEYDOWN:
						{
							Keystate = SDL_GetKeyboardState(NULL);
							if (Keystate[SDL_SCANCODE_P])
							{
								pause = false;
							}
							if (Keystate[SDL_SCANCODE_ESCAPE])
							{
								MainGame = false;
								Running = false;
								pause = false;
							}
							break;
						}
						}
					}
				}

				if (player.targetSeeking == true)
				{
					for (Missile* m : Missile::missiles)
					{
						m->SearchForTarget(asteroids);
						m->SearchForTarget(enemies);
						if (m->homing == true) //da funktioniert nochwas nicht so ganz
						{
							m->testLeftoverCounter++;
							if (m->testLeftoverCounter % 1 == 0)
							{
								Object* o = new Object(Object::Type::neutral);
								o->SetPosAndRot(m->posX, m->posY, m->rotation);
								o->DrawObject = { (int)o->posX,(int)o->posY, 4, 4};
								m->targetSeekingLeftovers.push_back(*o); //evtl könnte hier ein fehler entstehen
							}
							if (m->targetSeekingLeftovers.size() > 30) //-->evlt alte linien ausgrauen? und dann verschwinden
							{
								m->targetSeekingLeftovers.pop_front();
							}
						}
					}
				}

				for (int i = 0; i < Missile::missiles.size(); i++)
				{
					for (int j = 0; j < asteroids.size(); j++)
					{
						if (Missile::missiles.at(i)->alive == true && asteroids.at(j)->alive == true)
						{
							if (Missile::missiles.at(i)->CheckCollision(asteroids.at(j)))
							{
								Missile::missiles.at(i)->alive = false;
								asteroids.at(j)->alive = false;
								score += 10;
								temporaryScore += 10;
								asteroids.at(j)->anim->playing = false;
								Animation* a = new Animation((int)asteroids.at(j)->posX - ((asteroids.at(j)->width + 30) / 2), (int)asteroids.at(j)->posY - ((asteroids.at(j)->width + 30) / 2), asteroids.at(j)->width + 30, asteroids.at(j)->height + 30, ResourceDatabase::Textures["AsteroidDestruction"], 4, 1, 0, 5);
								Animation::animations.push_back(a);

								if (player.splitShot == true)
								{
									Missile::missiles.at(i)->TriggerSplitshot();
								}
								SpawnPickup(asteroids.at(j)->posX, asteroids.at(j)->posY, ResourceDatabase::Textures["Pickups"]);
								if (asteroids.at(j)->width > 40 && player.megaShot == false) {
									SpawnAsteroids(player, asteroids.at(j)->asteroidsInside, asteroids.at(j)->posX, asteroids.at(j)->posY, 30, ResourceDatabase::Textures["AsteroidAnim1"], ResourceDatabase::Textures["AsteroidAnim2"]);
								}
							}
						}
					}
				}
				for (int i = 0; i < Missile::missiles.size(); i++)
				{
					for (Enemy* e : enemies)
					{
						if (Missile::missiles.at(i)->alive == true && e->alive == true)
						{
							if (Missile::missiles.at(i)->CheckCollision(e))
							{
								Missile::missiles.at(i)->alive = false;
								if (e->eType != 1)
								{
									if (e->shielded == false || player.shieldBreak == true)
									{
										e->lifecount -= 1;
										if (e->lifecount <= 0)
										{
											e->alive = false;
											score += 20;
											temporaryScore += 20;
											Animation* a = new Animation((int)e->posX - 62, (int)e->posY - 62, 125, 125, ResourceDatabase::Textures["ExplosionAnim"], 6, 1, 0, 6);
											Animation::animations.push_back(a);
											if (player.splitShot == true)
											{
												Missile::missiles.at(i)->TriggerSplitshot();
											}
										}
									}
								}
								else
								{
									if (CheckCollisionHCircle(Missile::missiles.at(i), e) || player.shieldBreak == true)
									{
										e->lifecount -= 1;
										if (e->lifecount <= 0)
										{
											e->alive = false;
											score += 20;
											temporaryScore += 20;
											Animation* a = new Animation((int)e->posX - 62, (int)e->posY - 62, 125, 125, ResourceDatabase::Textures["ExplosionAnim"], 6, 1, 0, 6);
											Animation::animations.push_back(a);
											if (player.splitShot == true)
											{
												Missile::missiles.at(i)->TriggerSplitshot();
											}
											SpawnPickup(e->posX, e->posY, ResourceDatabase::Textures["Pickups"]);
										}
									}
								}
							}
						}
					}
				}


				if (player.alive == true)
				{
					for (Asteroid *a : asteroids)
					{
						if (a->CheckCollision(&player))
						{
							a->alive = false;
							a->anim->playing = false;
							if (player.lifeCount > 1)
							{
								Animation* t = new Animation((int)player.posX - 150, (int)player.posY - 150, 300, 300, ResourceDatabase::Textures["shockWaveAnim"], 9, 1, 0, 3);
								Animation::animations.push_back(t);
							}
							if (player.shieldActice == false)
							{
								player.hitByEnemy = true;
							}
							TriggerKillRadius(asteroids, player, 150);
							TriggerKillRadius(enemies, player, 150);
							TriggerKillRadius(EMissile::eMissiles, player, 150);
						}
					}


					for (Enemy *e : enemies)
					{
						if (e->CheckCollision(&player))
						{
							e->alive = false;
							if (player.lifeCount > 1)
							{
								Animation* t = new Animation((int)player.posX - 150, (int)player.posY - 150, 300, 300, ResourceDatabase::Textures["shockWaveAnim"], 9, 1, 0, 3);
								Animation::animations.push_back(t);
							}
							if (player.shieldActice == false)
							{
								player.hitByEnemy = true;
							}
							TriggerKillRadius(asteroids, player, 150);
							TriggerKillRadius(enemies, player, 150);
							TriggerKillRadius(EMissile::eMissiles, player, 150);
						}
					}

					for (EMissile *em : EMissile::eMissiles)
					{
						if (em->CheckCollision(&player))
						{
							em->alive = false;
							if (player.lifeCount > 1)
							{
								Animation* t = new Animation((int)player.posX - 150, (int)player.posY - 150, 300, 300, ResourceDatabase::Textures["shockWaveAnim"], 9, 1, 0, 3);
								Animation::animations.push_back(t);
							}
							if (player.shieldActice == false)
							{
								player.hitByEnemy = true;
							}
							TriggerKillRadius(asteroids, player, 150);
							TriggerKillRadius(enemies, player, 150);
							TriggerKillRadius(EMissile::eMissiles, player, 150);
						}
					}

				}

				
				if (player.hitByEnemy)
				{
					player.hitByEnemy = false;
					player.lifeCount -= 1;
					player.playerHealth.at(player.lifeCount).texture = ResourceDatabase::Textures["pHealthEmpty"];
					if (player.lifeCount <= 0)
					{
						player.alive = false;
						TriggerEndGame = true;
						Animation* t = new Animation((int)player.posX - 50, (int)player.posY - 50, 100, 100, ResourceDatabase::Textures["ExplosionAnim"], 6, 1, 0, 6);
						Animation::animations.push_back(t);
					}
				}
				
				for (Pickup *p : pickups)
				{
					p->Update();
					if (p->CheckCollision(&player))
					{
						p->alive = false;
						if (p->spritePosX == 0 && p->spritePosY == 0)
						{
							player.shieldBreak = true;
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 0), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 7), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 8), Pickup::pickupn.end());
						}
						if (p->spritePosX == 1 && p->spritePosY == 0)
						{
							player.mWeaponType = player.DoubleShot;
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 1), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 2), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 3), Pickup::pickupn.end());
						}
						if (p->spritePosX == 2 && p->spritePosY == 0)
						{
							player.mWeaponType = player.TripleShot;
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 1), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 2), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 3), Pickup::pickupn.end());
						}
						if (p->spritePosX == 3 && p->spritePosY == 0)
						{
							player.mWeaponType = player.QuadrupleShot;
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 1), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 2), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 3), Pickup::pickupn.end());
						}
						if (p->spritePosX == 4 && p->spritePosY == 0)
						{
							player.targetSeeking = true;
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 4), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 9), Pickup::pickupn.end());
						}
						if (p->spritePosX == 5 && p->spritePosY == 0)
						{
							player.megaShot = true;
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 4), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 5), Pickup::pickupn.end());
						}
						if (p->spritePosX == 6 && p->spritePosY == 0)
						{
							TriggerKillRadius(asteroids, player, 500);
							TriggerKillRadius(enemies, player, 500);
							TriggerKillRadius(EMissile::eMissiles, player, 500);
							Animation* a = new Animation((int)player.posX - 250, (int)player.posY - 250, 500, 500, ResourceDatabase::Textures["ExplosionAnim"], 6, 1, 0, 6);
							Animation::animations.push_back(a);
						}
						if (p->spritePosX == 7 && p->spritePosY == 0)
						{
							//timeslow
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 0), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 7), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 8), Pickup::pickupn.end());
						}
						if (p->spritePosX == 8 && p->spritePosY == 0)
						{
							player.playerShield = true;
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 0), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 7), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 8), Pickup::pickupn.end());
						}
						if (p->spritePosX == 9 && p->spritePosY == 0)
						{
							player.splitShot = true;
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 5), Pickup::pickupn.end());
							Pickup::pickupn.erase(std::remove(Pickup::pickupn.begin(), Pickup::pickupn.end(), 9), Pickup::pickupn.end());
						}
						if (p->spritePosX == 0 && p->spritePosY == 1)
						{
							if (player.turningSpeed < player.maxTurningSpeed)
							{
								player.turningSpeed += 0.5;
							}

						}
						if (p->spritePosX == 1 && p->spritePosY == 1)
						{
							if (player.Speed < player.maxSpeed)
							{
								player.Speed += 1;
							}
						}
						if (p->spritePosX == 2 && p->spritePosY == 1)
						{
							if (Pickup::luck > 7)
							{
								Pickup::luck -= 0.5;
							}
						}
						if (p->spritePosX == 3 && p->spritePosY == 1)
						{
							if (player.maxHealth < 7)
							{
								player.maxHealth++;
								Object health(Object::Type::neutral);
								health.texture = ResourceDatabase::Textures["pHealthEmpty"];
								health.DrawObject = { 615 - ((int)player.playerHealth.size() * 25),5,20,40 }; 
								player.playerHealth.push_back(health);
							}
							if (player.lifeCount < player.maxHealth)
							{
								player.playerHealth.at(player.lifeCount).texture = ResourceDatabase::Textures["pHealthFull"];
								player.lifeCount += 1;
							}
						}
						if (p->spritePosX == 4 && p->spritePosY == 1)
						{
							if (player.fireRate > player.maxFireRate)
							{
								player.fireRate -= 2;
							}
						}
						if (p->spritePosX == 5 && p->spritePosY == 1)
						{
							if (Missile::missileSpeed < Missile::maxMissileSpeed)
							{
								Missile::missileSpeed += 1;
							}
						}
					}
				}

				/*
				for (Missile *m : Missile::missiles) //muss auch noch wegen neuem homing überarbeitet werden
				{
					if (m->homing == true && m->enemyID != 0)
					{
						for (int i = 0; i < asteroids.size(); i++)
						{
							if (asteroids.at(i)->ID == m->enemyID)
							{
								m->eposX = asteroids.at(i)->posX;
								m->eposY = asteroids.at(i)->posY;
								if (asteroids.at(i)->alive == false)
								{
									m->enemyID = 0;
								}
							}
						}
					}
				}
				*/

				for (Missile *m : Missile::missiles) //muss auch noch wegen neuem homing überarbeitet werden
				{
					if (m->homing == true && m->enemyID != 0)
					{
						for (int i = 0; i < asteroids.size(); i++)
						{
							if (asteroids.at(i)->ID == m->enemyID)
							{
								m->eposX = asteroids.at(i)->posX;
								m->eposY = asteroids.at(i)->posY;
								if (asteroids.at(i)->alive == false)
								{
									m->enemyID = 0;
								}
							}
						}
					}
				}

				for (Missile *m : Missile::missiles)
				{
					if (m->homing == true && m->enemyID != 0)
					{
						for (int i = 0; i < enemies.size(); i++)
						{
							if (enemies.at(i)->ID == m->enemyID)
							{
								m->eposX = enemies.at(i)->posX;
								m->eposY = enemies.at(i)->posY;
								if (enemies.at(i)->alive == false)
								{
									m->enemyID = 0;
								}
							}
						}
					}
				}
				/*
				for (Asteroid *a : asteroids)
				{
					for (int i = 0; i < Missile::missiles.size(); i++)
					{
						if (Missile::missiles.at(i)->ID == a->targetID)
						{
							if (Missile::missiles.at(i)->alive == false)
							{
								a->targetID = 0;
							}
						}
					}
				}
				*/
				/*
				for (Asteroid *a : asteroids)
				{
					for (int i = 0; i < Missile::missiles.size(); i++)
					{
						if (Missile::missiles.at(i)->ID == a->targetID) //falls diese id in target ids drinn ist
						{
							if (Missile::missiles.at(i)->alive == false)
							{
								a->targetID = 0; //aus der liste den wert entfernen
							}
						}
					}
				}
				*/
				for (Enemy *e : enemies)
				{
					for (int i = 0; i < Missile::missiles.size(); i++)
					{
						if (Missile::missiles.at(i)->ID == e->targetID)
						{
							if (Missile::missiles.at(i)->alive == false)
							{
								e->targetID = 0;
							}
						}
					}
				}
				for (std::vector<Asteroid*>::iterator itr = asteroids.begin(); itr != asteroids.end(); )
				{
					if ((*itr)->alive == false)
					{
						itr = asteroids.erase(itr);
					}
					else
						++itr;
				}

				for (std::vector<Missile*>::iterator itr = Missile::missiles.begin(); itr != Missile::missiles.end(); )
				{
					if ((*itr)->alive == false)
						itr = Missile::missiles.erase(itr);
					else
						++itr;
				}

				for (std::vector<Pickup*>::iterator itr = pickups.begin(); itr != pickups.end(); )
				{
					if ((*itr)->alive == false)
						itr = pickups.erase(itr);
					else
						++itr;
				}

				for (std::vector<Enemy*>::iterator itr = enemies.begin(); itr != enemies.end(); )
				{
					if ((*itr)->alive == false)
						itr = enemies.erase(itr);
					else
						++itr;
				}

				for (std::vector<EMissile*>::iterator itr = EMissile::eMissiles.begin(); itr != EMissile::eMissiles.end(); )
				{
					if ((*itr)->alive == false)
						itr = EMissile::eMissiles.erase(itr);
					else
						++itr;
				}

				for (std::vector<Animation*>::iterator itr = Animation::animations.begin(); itr != Animation::animations.end(); )
				{
					if ((*itr)->playing == false)
						itr = Animation::animations.erase(itr);
					else
						++itr;
				}

				SDL_RenderCopy(Renderer, ResourceDatabase::Textures["background"], NULL, NULL);

				for (Missile* a : Missile::missiles)
				{
					if (a->targetSeekingLeftovers.size() > 0)
					{
						for (Object o : a->targetSeekingLeftovers)
						{
							SDL_RenderCopy(Renderer, ResourceDatabase::Textures["TargetSeekingLeftovers"], NULL, &o.DrawObject);
						}
					}
					SDL_RenderCopyEx(Renderer, ResourceDatabase::Textures["missile"], NULL, &a->DrawObject, a->rotation + 90, NULL, SDL_FLIP_NONE);
				}

				for (Pickup* p : pickups)
				{
					SDL_RenderCopy(Renderer, ResourceDatabase::Textures["Pickups"], &p->SpriteSheet, &p->DrawObject);
				}

				for (Animation* a : Animation::animations)
				{
					if (a->playing == true)
						SDL_RenderCopyEx(Renderer, a->Used, &a->Anim, &a->AnimPosition, a->rotation, NULL, SDL_FLIP_NONE);
				}
				for (EMissile* em : EMissile::eMissiles)
				{
					SDL_RenderCopyEx(Renderer, ResourceDatabase::Textures["eMissile"], NULL, &em->DrawObject, em->rotation, NULL, SDL_FLIP_NONE);
				}
				for (Enemy* e : enemies)
				{
					SDL_RenderCopyEx(Renderer, e->Used, NULL, &e->DrawObject, e->rotation + 90, NULL, SDL_FLIP_NONE);
					if (e->shielded == true)
					{
						SDL_RenderCopyEx(Renderer, e->UsedShield, NULL, &e->DrawShield, e->rotation + 90, NULL, SDL_FLIP_NONE);
					}
				}
				if (player.alive == true)
				{
					SDL_RenderCopyEx(Renderer, player.Sprite, NULL, &player.DrawObject, player.rotation + 90, NULL, SDL_FLIP_NONE);
					if (player.shieldActice == true)
					{
						SDL_RenderCopyEx(Renderer, ResourceDatabase::Textures["playerShield"], NULL, &player.drawShield, player.rotation + 90, NULL, SDL_FLIP_NONE);
					}
				}
				for (Object o : player.playerHealth)
				{
					SDL_RenderCopy(Renderer, o.texture, NULL, &o.DrawObject);
				}
				text = "Score: " + std::to_string(temporaryScore);
				SDL_Surface* text_surface = TTF_RenderText_Solid(font, text.c_str(), text_color);
				SDL_Texture* textTexture = SDL_CreateTextureFromSurface(Renderer, text_surface);
				SDL_QueryTexture(textTexture, NULL, NULL, &t_width, &t_height);
				SDL_FreeSurface(text_surface);
				dst = { textPosX , textPosY, t_width, t_height };
				SDL_RenderCopy(Renderer, textTexture, NULL, &dst);

				SDL_RenderPresent(Renderer);
				if (TriggerEndGame == true)
				{
					endGamgeCounter++;
					if (endGamgeCounter >= 180)
					{
						MainGame = false;
						RestartMenu = true;
					}
				}
			}
		}
	}
	/*
	SDL_DestroyTexture(playerShip);
	SDL_DestroyTexture(damagedPlayer);
	SDL_DestroyTexture(playerShield);
	SDL_DestroyTexture(PlayButton);
	SDL_DestroyTexture(QuitButton);
	SDL_DestroyTexture(RestartButton);
	SDL_DestroyTexture(background);
	SDL_DestroyTexture(enemy1);
	SDL_DestroyTexture(enemy2);
	SDL_DestroyTexture(enemy3);
	SDL_DestroyTexture(enemy4);
	SDL_DestroyTexture(enemy5);
	SDL_DestroyTexture(damagedEnemy1);
	SDL_DestroyTexture(damagedEnemy2);
	SDL_DestroyTexture(damagedEnemy3);
	SDL_DestroyTexture(damagedEnemy4);
	SDL_DestroyTexture(damagedEnemy5);
	SDL_DestroyTexture(startscreen);
	SDL_DestroyTexture(enemyHShield);
	SDL_DestroyTexture(shockWaveAnim);
	SDL_DestroyTexture(AsteroidAnim1);
	SDL_DestroyTexture(AsteroidAnim2);
	SDL_DestroyTexture(AsteroidDestruction);
	SDL_DestroyTexture(eMissile);
	SDL_DestroyTexture(Pickups);
	SDL_DestroyTexture(ExplosionAnim);
	SDL_DestroyTexture(missile);
	SDL_DestroyTexture(enemyShield);
	*/
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();
	return 0;
}



