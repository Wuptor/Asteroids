#include <cmath> // u.a. Trigonometrie
#include <iostream> // Konsole Ein-/Ausgabe
#include <vector> // dynamischer Array
#include <algorithm>
#include <iterator>
#include <string>

#include "SDL.h"
#include <SDL_ttf.h>

#define SCREEN_WIDTH  640 
#define SCREEN_HEIGHT 480
#define SPRITE_SIZE    32

static int ObjectID = 1; 
int score = 0;
int enemynumber = 1;
std::vector<int> pickupn = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }; 
bool pause;
int temporaryScore = 0;

class Object 
{
public: bool life;
		float posX, posY, radius, rotation;
		const int ID; 
		int width, height;
		SDL_Rect DrawObject;

		Object() : ID(ObjectID), life(true), posX(0), posY(0), width(0), height(0), rotation(0), radius(0)
		{
			ObjectID++;
		}

		void KeepInField() //sets position for objects that exit screen
		{
			if (posX < - width) 
				posX = SCREEN_WIDTH;
			if (posY < - height)
				posY = SCREEN_HEIGHT;
			if (posX > SCREEN_WIDTH)
				posX = 0;
			if (posY > SCREEN_HEIGHT)
				posY = 0;
		}

		void DeleteOnScreenExit() //sets life = false for objects that exit screen
		{
			if (posX < - width || posX > SCREEN_WIDTH || posY < - height || posY > SCREEN_HEIGHT) { 
				life = false;
			}
		}
};

class Animation :public Object
{
public:
	bool loop, playing;
	SDL_Texture* Used;
	SDL_Rect Anim;
	SDL_Rect AnimPosition;
	int FrameWidth, FrameHeight;
	int TextureWidth, TextureHeight;
	int frametime, animSpeed;

	Animation(int startx, int starty, int animwidth, int animheight, SDL_Texture* _used, int Wmultiply, int Hmultiply, int _rotation, int _animspeed)
	{
		AnimPosition.w = animwidth; 
		AnimPosition.h = animheight;
		AnimPosition.x = startx; 
		AnimPosition.y = starty; 

		SDL_QueryTexture(_used, NULL, NULL, &TextureWidth, &TextureHeight);
		FrameWidth = TextureWidth / Wmultiply; 
		FrameHeight = TextureHeight / Hmultiply;

		Anim.x = 0;
		Anim.y = 0;
		Anim.w = FrameWidth;
		Anim.h = FrameHeight;

		frametime = 0;
		rotation = _rotation;
		playing = true;
		loop = false;
		Used = _used;
		animSpeed = _animspeed;
	}

	void update()
	{
		frametime++; 
		if (frametime == animSpeed && playing == true) 
		{
			frametime = 0;
			Anim.x += FrameWidth;
			if (Anim.x >= TextureWidth)
			{
				Anim.x = 0;
				Anim.y += FrameHeight;
				if (Anim.y >= TextureHeight)
				{
					Anim.y = 0;
					if (loop == false)
					{
						playing = false;
					}
				}
			}
		}
	}
	void updatePos(int x, int y, int _rotation)
	{
		AnimPosition.x = x - AnimPosition.w / 2;
		AnimPosition.y = y - AnimPosition.h / 2;
		rotation = _rotation;
	}
};

std::vector<Animation*> animations;
class Asteroid : public Object
{
public: float distance;
		SDL_Rect DrawObject = { (int)posX, (int)posY, width, height }; 
		Animation *anim;
		Object *targeted;
		int targetID, asteroidsInside, fx, fy;
		Asteroid(float _x, float _y, int _wah, SDL_Texture* used) : asteroidsInside(2 + rand() % 3) 
		{
			posX = _x;
			posY = _y;
			width = _wah;
			height = _wah;
			rotation = rand() % 360;
			while (fx == 0 && fy == 0)
			{
				fx = rand() % 4 - 2;
				fy = rand() % 4 - 2;
			}
			radius = _wah * 0.4; 
			anim = new Animation((int)posX, (int)posY, _wah, _wah, used, 7, 1, rotation, 6+fx);
			anim->loop = true;
			animations.push_back(anim);
			targetID = 0;
		}
		void update()
		{
			posX += fx;
			posY += fy;
			DrawObject = { (int)posX, (int)posY, width, height };
			DrawObject.x = posX - DrawObject.w / 2;
			DrawObject.y = posY - DrawObject.h / 2;
			anim->updatePos(posX, posY, rotation);
			KeepInField();
		}
};

class EMissile : public Object
{
public: float eMissileSpeed;
		EMissile(float _x, float _y, int _rotation) : eMissileSpeed(2)
		{
			width = 20;
			height = 20;
			radius = 10; 
			posX = _x + (int)(20 * cos(_rotation * float(M_PI) / 180.0f)); 
			posY = _y + (int)(20 * sin(_rotation * float(M_PI) / 180.0f)); 
			rotation = _rotation;
		}

		void update() {

			posX += eMissileSpeed * (cos((rotation*M_PI) / 180)); 
			posY += eMissileSpeed * (sin((rotation*M_PI) / 180));
			DrawObject = { (int)posX, (int)posY, width, height }; 

			DrawObject.x = posX - DrawObject.w / 2;
			DrawObject.y = posY - DrawObject.h / 2;
			DeleteOnScreenExit();
		}
};

class Player : public Object
{
public:
	bool thrust, doubleShot, tripleShot, quadrupleShot, splitShot, megaShot, targetSeeking, shieldBreak, playerShield,shieldActice;
	float maxSpeed, Speed, floatingSpeed, turningSpeed, maxTurningSpeed, killRadius;
	int fireCounter, fireRate, lifeCount, splitCount, shieldCounter, shieldActieTime, maxHealth, maxFireRate, keepRotation;
	SDL_Rect drawShield;
	SDL_Texture* Sprite;
	SDL_Texture* NormalSprite;
	SDL_Texture* DamagedSprite;
	Player(SDL_Texture* _used, SDL_Texture* _damagedUsed) : splitCount(8), lifeCount(3), maxSpeed(5), Speed(1),  floatingSpeed(0), keepRotation(0), turningSpeed(4), maxTurningSpeed(7), fireRate(25), fireCounter(0), shieldCounter(0),
		shieldActieTime(0), maxHealth(10), maxFireRate(5),
		thrust(false), doubleShot(false), tripleShot(false), quadrupleShot(false), splitShot(false), megaShot(false), targetSeeking(false), shieldBreak(false),
		playerShield(false),shieldActice(false)
	{
		radius = 15;
		posX = 200;
		posY = 100;
		width = 55;
		height = 61;
		NormalSprite = _used;
		DamagedSprite = _damagedUsed;
		Sprite = NormalSprite;
	}
	void update()
	{
		if (lifeCount == 1)
		{
			Sprite = DamagedSprite;
		}
		else {
			Sprite = NormalSprite;
		}
		if (playerShield == true)
		{
			shieldCounter++;
			if (shieldCounter > 360)
			{
				shieldActice = true;
				shieldActieTime++;
				if (shieldActieTime > 240)
				{
					shieldActice = false;
					shieldCounter = 0;
					shieldActieTime = 0;
				}
			}
		}
		if (thrust == true) 
		{
			if (floatingSpeed > Speed) 
			{
				floatingSpeed = Speed;
			}
			floatingSpeed += 1;
			keepRotation = rotation;
		}
		else
		{
			if (floatingSpeed > Speed / 60) 
			{
				floatingSpeed -= Speed /60;
			}
		}

		posX += floatingSpeed * (cos((keepRotation*M_PI) / 180));
		posY += floatingSpeed * (sin((keepRotation*M_PI) / 180));

		drawShield = { (int)posX, (int)posY, (height + 30), (height + 30) };
		DrawObject = { (int)posX, (int)posY, width, height };

		DrawObject.x = posX - DrawObject.w / 2;
		DrawObject.y = posY - DrawObject.h / 2;

		drawShield.x = posX - drawShield.w / 2;
		drawShield.y = posY - drawShield.h / 2;
		KeepInField();
	}
};

std::vector<EMissile*> eMissiles;
class Enemy : public Object //lifecount = 2
{
public: bool rotating, shielded, moving, turning, firstTurn, startCalculations; 
		float edirx, ediry, dist, espeed, ehyp, randomRotation, angleCopy, randomSpawnPos, xcounter, ycounter, distToSource, distToEnd, startrot, endrot, movedirecx, movedirecy, randomLength, distance; 
		int movementCounter, firerateCounter, eType, lifecount, targetID; 
		SDL_Texture* Used, *UsedShield, *Damaged;
		SDL_Rect DrawShield;
		Enemy(int EnemyType) : targetID(0), lifecount(2), startCalculations(true), startrot(0), endrot(0), movedirecx(0), movedirecy(0), distToEnd(0), distToSource(0), firstTurn(true), moving(false),
			edirx(0), ediry(0), dist(0), espeed(0), ehyp(0), movementCounter(0), firerateCounter(0), shielded(false), rotating(true), turning(true), randomSpawnPos(0), angleCopy(0), randomRotation(0),
			randomLength(0)
		{
			rotation = 0;
			xcounter = 0;
			ycounter = 0;
			width = 65;
			height = 65;
			eType = EnemyType;
			radius = 25;
		}

		void update(Player P ) 
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
			DrawShield = { (int)posX, (int)posY, (width + 30), (height +30)}; 
			DrawObject = {(int)posX, (int)posY, width, height }; 

			DrawObject.x = posX - DrawObject.w / 2;
			DrawObject.y = posY - DrawObject.h / 2;

			DrawShield.x = posX - DrawShield.w / 2;
			DrawShield.y = posY - DrawShield.h / 2;

		}
		void SpawnPlayerTargetAlien(SDL_Texture* _used, SDL_Texture* _useds, SDL_Texture* _damaged) 
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
				posY = 37+ rand() % (SCREEN_HEIGHT - 75);
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
		void SpawnRapidFireAlien(SDL_Texture* _used, SDL_Texture* _useds, SDL_Texture* _damaged) 
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
		void SpawnRapidRandomShotAlien(SDL_Texture* _used, SDL_Texture* _useds, SDL_Texture* _damaged) 
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
		void SpawnHalfCircleShotAlien(SDL_Texture* _used, SDL_Texture* _useds, SDL_Texture* _damaged) 
		{
			randomSpawnPos = rand() % 2;
			if (randomSpawnPos == 0)
			{
				posX =37 + rand() % (SCREEN_WIDTH - 75); 
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
		void EnemyMovementPattern1(Object P)
		{
			rotation = atan2(P.posY - posY, P.posX - posX);
			rotation = rotation * 180 / 3.14;
			firerateCounter++;
			if (firerateCounter > 60)
			{
				EMissile *em = new EMissile(posX, posY, rotation);
				eMissiles.push_back(em);
				firerateCounter = 0;
			}
		}
		void EnemyMovementPattern2()
		{
			shielded = true;
			firerateCounter++;
			if (firerateCounter > 90) {
				int randomMissileCount = rand() % 8 + 3; 
				for (int i = 0; i < randomMissileCount; i++)
				{
					EMissile* em = new EMissile(posX, posY, rand() % 360);
					eMissiles.push_back(em);
				}
				firerateCounter = 0;
			}
			rotation += randomRotation * 0.5;
		}
		void EnemyMovementPattern3()
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
					eMissiles.push_back(em);
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
		void EnemyMovementPattern4(Object P)
		{
			if (P.life == true)
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
		void EnemyMovementPattern5()
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
						EMissile* em = new EMissile(posX, posY, (rotation + i * 20)-85);
						eMissiles.push_back(em);
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
};

class Missile : public Object
{
public: bool alreadysplit, homing;
		float direcx, direcy, mhyp, dist, mrotation, length, eposX, eposY;
		static int missileSpeed, maxMissileSpeed;
		int enemyID;

		Missile(float X, float Y, int ROTATION)
		{
			if (X == NULL || Y == NULL)
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

		void update() {
			DrawObject = { (int)posX, (int)posY, 15, 20}; 
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
};


class Pickup : public Object 
{
public:
	int spritePosX, spritePosY, lifeTime, TextureWidth, TextureHeight, FrameWidth, FrameHeight;
	float rarity;
	static float luck;
	SDL_Rect SpriteSheet;
	Pickup(float _x, float _y, SDL_Texture* _Used) : lifeTime(600)
	{
		SDL_QueryTexture(_Used, NULL, NULL, &TextureWidth, &TextureHeight);
		FrameWidth = TextureWidth / 10; 
		FrameHeight = TextureHeight / 2;
		posX = _x;
		posY = _y;
		SpawnPickup();
		SpriteSheet.x = FrameWidth * spritePosX;
		SpriteSheet.y = FrameHeight * spritePosY;
		SpriteSheet.w = FrameWidth;
		SpriteSheet.h = FrameHeight;
		radius = 20;
	}
	void Update()
	{
		lifeTime--;
		if (lifeTime <= 0)
		{
			life = false;
		}
		DrawObject = { (int)posX, (int)posY, 25, 25 }; 
		DrawObject.x = posX - DrawObject.w / 2;
		DrawObject.y = posY - DrawObject.h / 2;
	}
	void SpawnPickup()
	{
		rarity = rand() % 10;
		if (rarity <= luck)
		{
			spritePosX = (int)rand() % 6;
			spritePosY = 1;
		}
		else
		{
			spritePosX = pickupn.at((int)rand() % pickupn.size());
			spritePosY = 0;
		}
	}
};

class Button : public Object 
{
public:
	SDL_Texture * usedButton;
	SDL_Texture * currentlyUsed;
	SDL_Texture * Highlighted;
	bool mousepressed;
	int ButtonID;
	Button(int _id, float _x, float _y, float _width, float _height, SDL_Texture* _used, SDL_Texture* _buttonHighlight)
	{
		ButtonID = _id;
		posX = _x;
		posY = _y;
		width = _width;
		height = _height;
		currentlyUsed = _used;
		usedButton = _used;
		Highlighted = _buttonHighlight;
		DrawObject = { (int)posX, (int)posY, width, height };
	}
	void handleEvent(SDL_Event* e)
	{
		if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			mousepressed = false;
			bool inside = true;
			if (x < posX)
			{
				inside = false;
			}
			else if (x > posX + width)
			{
				inside = false;
			}
			else if (y < posY)
			{
				inside = false;
			}
			else if (y > posY + height)
			{
				inside = false;
			}
			if (inside)
			{
				currentlyUsed = Highlighted;
				switch (e->type)
				{
				case SDL_MOUSEBUTTONDOWN:
					mousepressed = true;
					break;
				}
			}
			else
			{
				currentlyUsed = usedButton;
			}
		}
	}
};

std::vector<Asteroid*> asteroids;
std::vector<Missile*> missiles;
std::vector<Pickup*> pickups; 
std::vector<Enemy*> enemies;

bool CheckCollisionHCircle(Object* one, Object *second) 
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

bool CheckCollision(Object *one, Object *second)
{
	if ((one->posX - second->posX)*(one->posX - second->posX) + (one->posY - second->posY)* (one->posY - second->posY) <= (one->radius + second->radius)*(one->radius + second->radius)) 
	{
		return true;
	}
	return false;
}

SDL_Texture* ImageImport(const char* _dateiname, SDL_Renderer* _renderer) 
{
	SDL_Surface *temp = SDL_LoadBMP(_dateiname);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, temp);
	SDL_FreeSurface(temp);
	return texture;
}

void TriggerKillRadius(std::vector<Asteroid*> list, Player P, float killradius)
{
	Object Deathcircle;
	Deathcircle.posX = P.posX;
	Deathcircle.posY = P.posY;
	Deathcircle.radius = killradius;
	for (Asteroid* o : list)
	{
		if (CheckCollision(o, &Deathcircle))
		{
			o->life = false;
			o->anim->playing = false;
		}
	}

}

void TriggerKillRadius(std::vector<EMissile*> list, Player P, float killradius)
{
	Object Deathcircle;
	Deathcircle.posX = P.posX;
	Deathcircle.posY = P.posY;
	Deathcircle.radius = killradius;
	for (EMissile* o : list)
	{
		if (CheckCollision(o, &Deathcircle))
			o->life = false;
	}

}

void TriggerKillRadius(std::vector<Enemy*> list, Player P, float killradius)
{
	Object Deathcircle;
	Deathcircle.posX = P.posX;
	Deathcircle.posY = P.posY;
	Deathcircle.radius = killradius;
	for (Enemy* o : list)
	{
		if (CheckCollision(o, &Deathcircle))
			o->life = false;
	}

}

void TriggerSplitshot(Player P, Missile* M)
{
	if (P.splitShot == true && M->alreadysplit == false)
	{
		int ra = (int)rand() % P.splitCount + 4; 
		for (int i = 0; i < ra; i++)
		{
			Missile *m1 = new Missile((int)M->posX, (int)M->posY, rand() % 360);  
			m1->alreadysplit = true;
			missiles.push_back(m1);
		}
	}
}



void SearchForTarget(std::vector<Asteroid*> list) 
{
	for (int j = 0; j < missiles.size(); j++) 
	{
		if (missiles.at(j)->enemyID == 0)
		{
			int index = -1;
			float distance = FLT_MAX;
			for (int i = 0; i < list.size(); i++)
			{
				if (list.at(i)->targetID == 0) 
				{
					list.at(i)->distance = sqrt(pow(missiles.at(j)->posX - list.at(i)->posX, 2) + pow(missiles.at(j)->posY - list.at(i)->posY, 2));
					if (list.at(i)->distance <= distance)
					{
						index = i;
						list.at(index)->distance = sqrt(pow(missiles.at(j)->posX - list.at(index)->posX, 2) + pow(missiles.at(j)->posY - list.at(index)->posY, 2));
						distance = list.at(index)->distance;
					}
				}
			}
			if (list.size() != 0 && index >= 0) 
			{
				missiles.at(j)->enemyID = list.at(index)->ID;
				list.at(index)->targetID = missiles.at(j)->ID;
			}
		}
	}
}

void SearchForTarget(std::vector<Enemy*> list) 
{
	for (int j = 0; j < missiles.size(); j++) 
	{
		if (missiles.at(j)->enemyID == 0)
		{
			int index = -1;
			float distance = FLT_MAX;
			for (int i = 0; i < list.size(); i++)
			{
				if (list.at(i)->targetID == 0) 
				{
					list.at(i)->distance = sqrt(pow(missiles.at(j)->posX - list.at(i)->posX, 2) + pow(missiles.at(j)->posY - list.at(i)->posY, 2));
					if (list.at(i)->distance <= distance)
					{
						index = i;
						list.at(index)->distance = sqrt(pow(missiles.at(j)->posX - list.at(index)->posX, 2) + pow(missiles.at(j)->posY - list.at(index)->posY, 2));
						distance = list.at(index)->distance;
					}
				}
			}
			if (list.size() != 0 && index >= 0) 
			{
				missiles.at(j)->enemyID = list.at(index)->ID;
				list.at(index)->targetID = missiles.at(j)->ID;
			}
		}
	}
}

void SpawnPickup(float _x, float _y, SDL_Texture* _used)
{
	if (pickups.size() <= 10)
	{
		if (rand()% 3 == 1) 
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

void SpawnAsteroids(Player P,int _aCount, float _posX, float _posY, int _wah, SDL_Texture * _used1, SDL_Texture* _used2)
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
			while (x == P.posX || y == P.posY)
			{
				x = rand() % SCREEN_WIDTH;
			    y = rand() % SCREEN_HEIGHT;
			}
			asteroids.push_back(a);
		}
	}
	else
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
			asteroids.push_back(a);
		}
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
	std::string fontpath = "dat/arial.ttf"; //arial irgendwas
	std::string text = "test";
	TTF_Font* font = TTF_OpenFont(fontpath.c_str(), fontsize);
	int textPosX = 0;
	int textPosY = 0;
	SDL_Rect dst = { textPosX , textPosY, t_width, t_height };


	SDL_Window * Window = SDL_CreateWindow("Asteroides", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0); 
	SDL_Renderer * Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_PRESENTVSYNC); 

	SDL_Texture* enemyHShield = ImageImport("dat/HalfShield.bmp", Renderer);
	SDL_Texture* enemyShield = ImageImport("dat/EnemyShield.bmp", Renderer);

	SDL_Texture* playerShield = ImageImport("dat/PlayerShield.bmp", Renderer);
	SDL_Texture* missile = ImageImport("dat/PlayerMissile.bmp", Renderer);

	SDL_Texture* enemy1 = ImageImport("dat/Enemy_01.bmp", Renderer);
	SDL_Texture* enemy2 = ImageImport("dat/Enemy_02.bmp", Renderer);
	SDL_Texture* enemy3 = ImageImport("dat/Enemy_03.bmp", Renderer);
	SDL_Texture* enemy4 = ImageImport("dat/Enemy_04.bmp", Renderer);
	SDL_Texture* enemy5 = ImageImport("dat/Enemy_05.bmp", Renderer);

	SDL_Texture* damagedEnemy1 = ImageImport("dat/Enemy_01Damaged.bmp", Renderer);
	SDL_Texture* damagedEnemy2 = ImageImport("dat/Enemy_02Damaged.bmp", Renderer);
	SDL_Texture* damagedEnemy3 = ImageImport("dat/Enemy_03Damaged.bmp", Renderer);
	SDL_Texture* damagedEnemy4 = ImageImport("dat/Enemy_04Damaged.bmp", Renderer);
	SDL_Texture* damagedEnemy5 = ImageImport("dat/Enemy_05Damaged.bmp", Renderer);

	SDL_Texture* damagedPlayer = ImageImport("dat/DamagedPlayer.bmp", Renderer);

	SDL_Texture* eMissile = ImageImport("dat/EnemyMissile.bmp", Renderer);

	SDL_Texture* ExplosionAnim = ImageImport("dat/Explosion.bmp", Renderer); 
	SDL_Texture* AsteroidAnim1 = ImageImport("dat/Asteroid_01.bmp", Renderer);
	SDL_Texture* AsteroidAnim2 = ImageImport("dat/Asteroid_02.bmp", Renderer);
	SDL_Texture* AsteroidDestruction = ImageImport("dat/AsteroidDestructionAnim.bmp", Renderer);

	SDL_Texture* playerShip = ImageImport("dat/PlayerSprite.bmp", Renderer);

	SDL_Texture* shockWaveAnim = ImageImport("dat/ShockWave.bmp", Renderer);

	SDL_Texture* Pickups = ImageImport("dat/PickupsSpritesheet.bmp", Renderer);
	SDL_Texture* background = ImageImport("dat/Background.bmp", Renderer);

	SDL_Texture* ButtonHighlight = ImageImport("dat/GreyOverlay.bmp", Renderer);

	SDL_Texture* QuitButton = ImageImport("dat/QuitButton.bmp", Renderer);
	SDL_Texture* PlayButton = ImageImport("dat/PlayButton.bmp", Renderer);
	SDL_Texture* RestartButton = ImageImport("dat/RestartButton.bmp", Renderer);
	SDL_Texture* startscreen = ImageImport("dat/startscreen.bmp", Renderer);

	Button* a = new Button(1, 220, 250, 200, 50, PlayButton, ButtonHighlight);
	buttons.push_back(a);
	Button* b = new Button(2, 220, 330, 200, 50, QuitButton, ButtonHighlight);
	buttons.push_back(b);
	Button* c = new Button(3, 220, 250, 200, 50, RestartButton, ButtonHighlight);
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
			SDL_Event Event;
			while (SDL_PollEvent(&Event)) 
			{
				for (Button* b : buttons)
				{
					b->handleEvent(&Event);
					if (b->ID == 1 && b->mousepressed == true)
					{
						PreGame = true;
						MainMenu = false;
					}
					if (b->ID == 2 && b->mousepressed == true)
					{
						Running = false;
						MainMenu = false;
					}
				}
				switch (Event.type)
				{
				case SDL_QUIT:
					Running = false;
					MainMenu = false;
					break;
				}
			}
			SDL_RenderCopy(Renderer, startscreen, NULL, NULL);
			for (Button* b : buttons)
			{
				if (b->ID == 1 || b->ID == 2)
				SDL_RenderCopy(Renderer, b->currentlyUsed, NULL, &b->DrawObject);
			}
			SDL_RenderPresent(Renderer);
		}
		while (RestartMenu)
		{
			SDL_Event Event;
			while (SDL_PollEvent(&Event))
			{
				for (Button* b : buttons)
				{
					b->handleEvent(&Event);
					if (b->ID == 3 && b->mousepressed == true)
					{
						PreGame = true;
						RestartMenu = false;
					}
					if (b->ID == 2 && b->mousepressed == true)
					{
						Running = false;
						RestartMenu = false; 
					}
				}
				switch (Event.type)
				{
				case SDL_QUIT:
					Running = false;
					RestartMenu = false;
					break;
				}
			}
			SDL_RenderCopy(Renderer, startscreen, NULL, NULL);
			for (Button* b : buttons)
			{
				if(b->ID == 2 || b->ID == 3)
				SDL_RenderCopy(Renderer, b->currentlyUsed, NULL, &b->DrawObject);
			}
			SDL_RenderPresent(Renderer);
		}
		if (PreGame) //resets all values
		{
			asteroids.clear();
			missiles.clear();
			enemies.clear();
			pickups.clear();
			eMissiles.clear();
			animations.clear();
			pickupn.clear();
			pickupn = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			temporaryScore = 0;
			Player player(playerShip, damagedPlayer);
			Missile::missileSpeed = 4;
			Missile::maxMissileSpeed = 8;
			Pickup::luck = 8;
			score = 0;
			int endGamgeCounter = 0;
			bool TriggerEndGame = false;
			int asteroidSpawnCounter = 0;
			int asteroidspawnTime = 0;
			enemynumber = 0; 
			SpawnAsteroids(player, 5, NULL, NULL, 75, AsteroidAnim1, AsteroidAnim2);
			MainGame = true;
			PreGame = false;
			while (MainGame)
			{
				player.update();
				for (EMissile *em : eMissiles)
				{
					em->update();
				}
				for (Missile *m : missiles)
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
				for (Animation* y : animations)
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
					if (asteroidspawnTime > 15)
					{
						SpawnAsteroids(player, 1, NULL, NULL, 75, AsteroidAnim1, AsteroidAnim2);
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
							e->SpawnPlayerTargetAlien(enemy1, enemyShield, damagedEnemy1); //enemy that shoots at the player position
						if (EnemyType == 1)
							e->SpawnRapidRandomShotAlien(enemy2, enemyHShield, damagedEnemy2); //enemy that shoots in random directions
						if (EnemyType == 2)
							e->SpawnRapidFireAlien(enemy3, enemyShield, damagedEnemy3); //enemy that shoots a rapid bullet stream
						if (EnemyType == 3)
							e->SpawnPlayerTargetAlien(enemy4, enemyShield, damagedEnemy4); //enemy that chases the player
						if (EnemyType == 4)
							e->SpawnHalfCircleShotAlien(enemy5, enemyShield, damagedEnemy5); //enemy that fires in a half circle

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
						break;
					}
					}
				}
				Keystate = SDL_GetKeyboardState(NULL);
				if (Keystate[SDL_SCANCODE_LEFT]) { 
					player.rotation -= player.turningSpeed; 
				}
				if (Keystate[SDL_SCANCODE_RIGHT]) {
					player.rotation += player.turningSpeed; 
				}
				if (Keystate[SDL_SCANCODE_UP]) {
					player.thrust = true;
				}			
				else
				{
					player.thrust = false;
				}
				player.fireCounter++;
				if (player.fireCounter > player.fireRate && player.life == true) { 
					if (Keystate[SDL_SCANCODE_SPACE]) {
						if (player.doubleShot == true)
						{
							Missile *m = new Missile(player.posX, player.posY, player.rotation + 7);
							missiles.push_back(m);
							Missile *m1 = new Missile(player.posX, player.posY, player.rotation - 7); 
							missiles.push_back(m1);
						}
						else if (player.tripleShot == true)
						{
							Missile *m = new Missile(player.posX, player.posY, player.rotation + 7);
							missiles.push_back(m);
							Missile *m1 = new Missile(player.posX, player.posY, player.rotation);
							missiles.push_back(m1);
							Missile *m2 = new Missile(player.posX, player.posY, player.rotation - 7);
							missiles.push_back(m2);
						}
						else if (player.quadrupleShot == true)
						{
							Missile *m = new Missile(player.posX, player.posY, player.rotation - 7);
							missiles.push_back(m);
							Missile *m1 = new Missile(player.posX, player.posY, player.rotation - 2);
							missiles.push_back(m1);
							Missile *m2 = new Missile(player.posX, player.posY, player.rotation + 2);
							missiles.push_back(m2);
							Missile *m3 = new Missile(player.posX, player.posY, player.rotation + 7);
							missiles.push_back(m3);
						}
						else
						{
							Missile *m = new Missile(player.posX, player.posY, player.rotation);
							missiles.push_back(m);
						}
						player.fireCounter = 0;
					}
				}

				while (pause)
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
							break;
						}
						case SDL_KEYDOWN:
						{
							Keystate = SDL_GetKeyboardState(NULL);
							if (Keystate[SDL_SCANCODE_P])
							{
								pause = false;
							}
							break;
						}
						}
					}
				}

				if (player.targetSeeking == true) 
				{
					SearchForTarget(enemies); 
					SearchForTarget(asteroids);
				}

				for (int i = 0; i < missiles.size(); i++) 
				{
					for (int j = 0; j < asteroids.size(); j++)
					{
						if (missiles.at(i)->life == true && asteroids.at(j)->life == true) 
						{
							if (CheckCollision(missiles.at(i), asteroids.at(j))) 
							{
								missiles.at(i)->life = false;
								asteroids.at(j)->life = false;
								score += 10; 
								temporaryScore += 10;
								asteroids.at(j)->anim->playing = false; 
								Animation* a = new Animation((int)asteroids.at(j)->posX - ((asteroids.at(j)->width + 30) / 2), (int)asteroids.at(j)->posY - ((asteroids.at(j)->width + 30) / 2), asteroids.at(j)->width + 30, asteroids.at(j)->height + 30, AsteroidDestruction, 4, 1, 0, 5);
								animations.push_back(a);

								TriggerSplitshot(player, missiles.at(i));
								SpawnPickup(asteroids.at(j)->posX, asteroids.at(j)->posY, Pickups);
								if (asteroids.at(j)->width > 40 && player.megaShot == false) { 
									SpawnAsteroids(player, asteroids.at(j)->asteroidsInside, asteroids.at(j)->posX, asteroids.at(j)->posY, 30, AsteroidAnim1, AsteroidAnim2); 
								}
							}
						}
					}
				}
				for (int i = 0; i < missiles.size(); i++)
				{
					for (Enemy* e : enemies)
					{
						if (missiles.at(i)->life == true && e->life == true) 
						{
							if (CheckCollision(missiles.at(i), e)) 
							{
								missiles.at(i)->life = false;
								if (e->eType != 1)
								{
									if (e->shielded == false || player.shieldBreak == true)
									{
										e->lifecount -= 1;
										if (e->lifecount <= 0)
										{
											e->life = false; 
											score += 20;
											temporaryScore += 20;
											Animation* a = new Animation((int)e->posX - 62, (int)e->posY - 62, 125, 125, ExplosionAnim, 6, 1, 0, 6);
											animations.push_back(a);
											TriggerSplitshot(player, missiles.at(i));
										}
									}
								}
								else
								{
									if (CheckCollisionHCircle(missiles.at(i), e) || player.shieldBreak == true)
									{
										e->lifecount -= 1;
										if (e->lifecount <= 0) 
										{
											e->life = false;
											score += 20; 
											temporaryScore += 20;
											Animation* a = new Animation((int)e->posX - 62, (int)e->posY - 62, 125, 125, ExplosionAnim, 6, 1, 0, 6);
											animations.push_back(a);
											TriggerSplitshot(player, missiles.at(i));
											SpawnPickup(e->posX, e->posY, Pickups);
										}
									}
								}
							}
						}
					}
				}

				
				if (player.life == true)
				{
					for (Asteroid *a : asteroids) 
					{
						if (CheckCollision(a, &player)) 
						{
							a->life = false; 
							a->anim->playing = false;
							if (player.lifeCount > 1)
							{
								Animation* t = new Animation((int)player.posX - 150, (int)player.posY - 150, 300, 300, shockWaveAnim, 9, 1, 0, 3);
								animations.push_back(t);
							}
							if (player.shieldActice == false)
							{
								player.lifeCount -= 1; 
								if (player.lifeCount <= 0)
								{
									player.life = false;
									TriggerEndGame = true;
									Animation* t = new Animation((int)player.posX - 50, (int)player.posY - 50, 100, 100, ExplosionAnim, 6, 1, 0, 6);
									animations.push_back(t);
								}
							}
							TriggerKillRadius(asteroids, player, 150);
							TriggerKillRadius(enemies, player, 150);
							TriggerKillRadius(eMissiles, player, 150);
						}
					}


					for (Enemy *e : enemies) 
					{
						if (CheckCollision(e, &player))
						{
							e->life = false; 
							if (player.lifeCount > 1)
							{
								Animation* t = new Animation((int)player.posX - 150, (int)player.posY - 150, 300, 300, shockWaveAnim, 9, 1, 0, 3);
								animations.push_back(t);
							}
							if (player.shieldActice == false)
							{
								player.lifeCount -= 1; 
								if (player.lifeCount <= 0)
								{
									player.life = false;
									TriggerEndGame = true;
									Animation* t = new Animation((int)player.posX - 50, (int)player.posY - 50, 100, 100, ExplosionAnim, 6, 1, 0, 6);
									animations.push_back(t);
								}
							}
							TriggerKillRadius(asteroids, player, 150);
							TriggerKillRadius(enemies, player, 150);
							TriggerKillRadius(eMissiles, player, 150);
						}
					}

					for (EMissile *em : eMissiles) 
					{
						if (CheckCollision(em, &player)) 
						{
							em->life = false; 
							if (player.lifeCount > 1)
							{
								Animation* t = new Animation((int)player.posX - 150, (int)player.posY - 150, 300, 300, shockWaveAnim, 9, 1, 0, 3);
								animations.push_back(t);
							}
							if (player.shieldActice == false)
							{
								player.lifeCount -= 1; 
								if (player.lifeCount <= 0)
								{
									player.life = false;
									TriggerEndGame = true;
									Animation* t = new Animation((int)player.posX - 50, (int)player.posY - 50, 100, 100, ExplosionAnim, 6, 1, 0, 6);
									animations.push_back(t);
								}
							}
							TriggerKillRadius(asteroids, player, 150);
							TriggerKillRadius(enemies, player, 150);
							TriggerKillRadius(eMissiles, player, 150);
						}
					}

				}
				
				for (Pickup *p : pickups) 
				{
					p->Update();
					if (CheckCollision(p, &player))
					{
						p->life = false;
						if (p->spritePosX == 0 && p->spritePosY == 0) 
						{
							player.shieldBreak = true;
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 0), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 7), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 8), pickupn.end());
						}
						if (p->spritePosX == 1 && p->spritePosY == 0)
						{
							player.doubleShot = true;
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 1), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 2), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 3), pickupn.end());
						}
						if (p->spritePosX == 2 && p->spritePosY == 0)
						{
							player.tripleShot = true;
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 1), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 2), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 3), pickupn.end());
						}
						if (p->spritePosX == 3 && p->spritePosY == 0)
						{
							player.quadrupleShot = true;
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 1), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 2), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 3), pickupn.end());
						}
						if (p->spritePosX == 4 && p->spritePosY == 0)
						{
							player.targetSeeking = true;
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 4), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 9), pickupn.end());
						}
						if (p->spritePosX == 5 && p->spritePosY == 0)
						{
							player.megaShot = true;
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 4), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 5), pickupn.end());
						}
						if (p->spritePosX == 6 && p->spritePosY == 0)
						{
							TriggerKillRadius(asteroids, player, 500);
							TriggerKillRadius(enemies, player, 500);
							TriggerKillRadius(eMissiles, player, 500);
							Animation* a = new Animation((int)player.posX - 250, (int)player.posY - 250, 500, 500, ExplosionAnim, 6, 1, 0, 6);
							animations.push_back(a);
						}
						if (p->spritePosX == 7 && p->spritePosY == 0)
						{
							//timeslow
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 0), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 7), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 8), pickupn.end());
						}
						if (p->spritePosX == 8 && p->spritePosY == 0)
						{
							player.playerShield = true;
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 0), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 7), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 8), pickupn.end());
						}
						if (p->spritePosX == 9 && p->spritePosY == 0)
						{
							player.splitShot = true;
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 5), pickupn.end());
							pickupn.erase(std::remove(pickupn.begin(), pickupn.end(), 9), pickupn.end());
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
							if (player.lifeCount < player.maxHealth)
							{
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

				for (Missile *m : missiles) 
				{
					if (m->homing == true && m->enemyID != 0) 
					{
						for (int i = 0; i < asteroids.size(); i++)
						{
							if (asteroids.at(i)->ID == m->enemyID) 
							{
								m->eposX = asteroids.at(i)->posX;
								m->eposY = asteroids.at(i)->posY;
								if (asteroids.at(i)->life == false) 
								{
									m->enemyID = 0;
								}
							}
						}
					}
				}
				for (Missile *m : missiles) 
				{
					if (m->homing == true && m->enemyID != 0) 
					{
						for (int i = 0; i < enemies.size(); i++)
						{
							if (enemies.at(i)->ID == m->enemyID) 
							{
								m->eposX = enemies.at(i)->posX;
								m->eposY = enemies.at(i)->posY;
								if (enemies.at(i)->life == false) 
								{
									m->enemyID = 0;
								}
							}
						}
					}
				}
				for (Asteroid *a : asteroids)
				{
					for (int i = 0; i < missiles.size(); i++)
					{
						if (missiles.at(i)->ID == a->targetID)
						{
							if (missiles.at(i)->life == false) 
							{
								a->targetID = 0;
							}
						}
					}
				}

				for (Enemy *e : enemies)
				{
					for (int i = 0; i < missiles.size(); i++)
					{
						if (missiles.at(i)->ID == e->targetID)
						{
							if (missiles.at(i)->life == false) 
							{
								e->targetID = 0;
							}
						}
					}
				}
				for (std::vector<Asteroid*>::iterator itr = asteroids.begin(); itr != asteroids.end(); )
				{
					if ((*itr)->life == false)
					{
						itr = asteroids.erase(itr);
					}
					else
						++itr;
				}

				for (std::vector<Missile*>::iterator itr = missiles.begin(); itr != missiles.end(); )
				{
					if ((*itr)->life == false)
						itr = missiles.erase(itr);
					else
						++itr;
				}

				for (std::vector<Pickup*>::iterator itr = pickups.begin(); itr != pickups.end(); )
				{
					if ((*itr)->life == false)
						itr = pickups.erase(itr);
					else
						++itr;
				}

				for (std::vector<Enemy*>::iterator itr = enemies.begin(); itr != enemies.end(); )
				{
					if ((*itr)->life == false)
						itr = enemies.erase(itr);
					else
						++itr;
				}

				for (std::vector<EMissile*>::iterator itr = eMissiles.begin(); itr != eMissiles.end(); )
				{
					if ((*itr)->life == false)
						itr = eMissiles.erase(itr);
					else
						++itr;
				}

				for (std::vector<Animation*>::iterator itr = animations.begin(); itr != animations.end(); )
				{
					if ((*itr)->playing == false)
						itr = animations.erase(itr);
					else
						++itr;
				}

				SDL_RenderCopy(Renderer, background, NULL, NULL);

				for (Missile* a : missiles)
				{
					SDL_RenderCopyEx(Renderer, missile, NULL, &a->DrawObject, a->rotation + 90, NULL, SDL_FLIP_NONE);
				}

				for (Pickup* p : pickups)
				{
					SDL_RenderCopy(Renderer, Pickups, &p->SpriteSheet, &p->DrawObject); 
				}

				for (Animation* a : animations)
				{
					if (a->playing == true)
						SDL_RenderCopyEx(Renderer, a->Used, &a->Anim, &a->AnimPosition, a->rotation, NULL, SDL_FLIP_NONE);
				}
				for (EMissile* em : eMissiles)
				{
					SDL_RenderCopyEx(Renderer, eMissile, NULL, &em->DrawObject, em->rotation, NULL, SDL_FLIP_NONE);
				}
				for (Enemy* e : enemies) 
				{
					SDL_RenderCopyEx(Renderer, e->Used, NULL, &e->DrawObject, e->rotation + 90, NULL, SDL_FLIP_NONE);
					if (e->shielded == true)
					{
						SDL_RenderCopyEx(Renderer, e->UsedShield, NULL, &e->DrawShield, e->rotation + 90, NULL, SDL_FLIP_NONE);
					}
				}
				if (player.life == true)
				{
					SDL_RenderCopyEx(Renderer, player.Sprite, NULL, &player.DrawObject, player.rotation + 90, NULL, SDL_FLIP_NONE);
					if (player.shieldActice == true)
					{
						SDL_RenderCopyEx(Renderer, playerShield, NULL, &player.drawShield, player.rotation + 90, NULL, SDL_FLIP_NONE);
					}
				}
				text = "Score: " + std::to_string(temporaryScore);
				SDL_Surface* text_surface = TTF_RenderText_Solid(font, text.c_str() , text_color);
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
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();
	return 0;
}


