#pragma once
#include "Object.h"
#include <algorithm>

class Pickup : public Object
{
public:
	Pickup(float _x, float _y, SDL_Texture* _Used);
	~Pickup();
	int spritePosX, spritePosY, lifeTime, TextureWidth, TextureHeight, FrameWidth, FrameHeight; //lifetime in sec angeben
	float rarity;
	static float luck;
	static std::vector<int> pickupn;
	SDL_Rect SpriteSheet;
	void Update();
	void CreatePickup();
};
