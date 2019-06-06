#pragma once
#include <SDL.h>

#define SCREEN_WIDTH  640 
#define SCREEN_HEIGHT 480


class Object
{
public:
	Object();
	~Object();

	float posX, posY, radius, rotation;
	const int ID;
	int width, height;
	bool alive;
	SDL_Rect DrawObject;
	void KeepInField();
	void DeleteOnScreenExit();
};
