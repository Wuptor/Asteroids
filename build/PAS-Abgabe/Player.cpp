#include "pch.h"
#include "Player.h"
//maxhealth funktion anpassen

Player::Player(SDL_Texture* _used, SDL_Texture* _damagedUsed) : splitCount(8), lifeCount(3), maxSpeed(5), Speed(1), floatingSpeed(0), keepRotation(0), turningSpeed(4), maxTurningSpeed(7), fireRate(25), fireCounter(0), shieldCounter(0),
shieldActieTime(0), maxHealth(3), maxFireRate(5),
thrust(false), splitShot(false), megaShot(false), targetSeeking(true), shieldBreak(false),
playerShield(false), shieldActice(false), hitByEnemy(false), Object(player)
{
	mWeaponType = Normal;
	radius = 15;
	posX = 200;
	posY = 100;
	width = 55;
	height = 61;
	NormalSprite = _used;
	DamagedSprite = _damagedUsed;
	Sprite = NormalSprite;
	for (int i = 0; i < maxHealth; i++)
	{
		Object health(Object::Type::neutral);
		health.texture = ResourceDatabase::Textures["pHealthFull"];
		health.DrawObject = {615 - (i*25),5,20,40};
		playerHealth.push_back(health);
	}
}


Player::~Player()
{
}

void Player::update() 
{
	handleInput();
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
			floatingSpeed -= Speed / 60;
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

void Player::handleInput()
{
	const Uint8  *Keystate = SDL_GetKeyboardState(NULL);
	if (Keystate[SDL_SCANCODE_LEFT]) { //umstellen auf extra buttons pro spieler --> für evlt 2 spieler modus
		rotation -= turningSpeed;
	}
	if (Keystate[SDL_SCANCODE_RIGHT]) {
		rotation += turningSpeed;
	}
	if (Keystate[SDL_SCANCODE_UP]) {
		thrust = true;
	}
	else
	{
		thrust = false;
	}
	fireCounter++;
	if (fireCounter > fireRate && alive == true) {
		if (Keystate[SDL_SCANCODE_SPACE]) {
			if (mWeaponType == DoubleShot)
			{
				Missile *m = new Missile(posX, posY, rotation + 7);
				Missile::missiles.push_back(m);
				Missile *m1 = new Missile(posX, posY, rotation - 7);
				Missile::missiles.push_back(m1);
			}
			else if (mWeaponType == TripleShot)
			{
				Missile *m = new Missile(posX, posY, rotation + 7);
				Missile::missiles.push_back(m);
				Missile *m1 = new Missile(posX, posY, rotation);
				Missile::missiles.push_back(m1);
				Missile *m2 = new Missile(posX, posY, rotation - 7);
				Missile::missiles.push_back(m2);
			}
			else if (mWeaponType == QuadrupleShot)
			{
				Missile *m = new Missile(posX, posY, rotation - 7);
				Missile::missiles.push_back(m);
				Missile *m1 = new Missile(posX, posY, rotation - 2);
				Missile::missiles.push_back(m1);
				Missile *m2 = new Missile(posX, posY, rotation + 2);
				Missile::missiles.push_back(m2);
				Missile *m3 = new Missile(posX, posY, rotation + 7);
				Missile::missiles.push_back(m3);
			}
			else //auf switch case umstellen default == exception
			{
				Missile *m = new Missile(posX, posY, rotation);
				Missile::missiles.push_back(m);
			}
			fireCounter = 0;
		}
	}
}

void Player::TriggerKillRadius(std::vector<Object*> list, float killradius)
{
	Object Deathcircle(Object::Type::neutral);
	Deathcircle.posX = posX;
	Deathcircle.posY = posY;
	Deathcircle.radius = killradius;
	for (Object* object : list)
	{
		if (object->CheckCollision(&Deathcircle) && object->mObjectType != player)
		{
			object->alive = false;
			//object->anim->playing = false; //evtl anim an object adden
		}
	}
}