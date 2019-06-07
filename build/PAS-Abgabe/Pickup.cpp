#include "pch.h"
#include "Pickup.h"

std::vector<int> Pickup::pickupn;

Pickup::Pickup(float _x, float _y, SDL_Texture* _Used) : lifeTime(600), Object(pickup) //lifetime static und für alle pickups gleich?
{
	SDL_QueryTexture(_Used, NULL, NULL, &TextureWidth, &TextureHeight);
	FrameWidth = TextureWidth / 10;
	FrameHeight = TextureHeight / 2;
	posX = _x;
	posY = _y;
	CreatePickup();
	SpriteSheet.x = FrameWidth * spritePosX;
	SpriteSheet.y = FrameHeight * spritePosY;
	SpriteSheet.w = FrameWidth;
	SpriteSheet.h = FrameHeight;
	radius = 20;
}


Pickup::~Pickup()
{
}


void Pickup::Update()
{
	lifeTime--;
	if (lifeTime <= 0)
	{
		alive = false;
	}
	DrawObject = { (int)posX, (int)posY, 25, 25 };
	DrawObject.x = posX - DrawObject.w / 2;
	DrawObject.y = posY - DrawObject.h / 2;
}
void Pickup::CreatePickup()
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

