#pragma once
#include <SDL.h>
#include <vector>

#define SCREEN_WIDTH  640 
#define SCREEN_HEIGHT 480


class Object
{
public:
	Object();
	~Object();

	static int ObjectID;
	float posX, posY, radius, rotation;
	const int ID;
	int width, height;
	bool alive;
	SDL_Rect DrawObject;
	void KeepInField();
	void DeleteOnScreenExit();
	bool CheckCollision(Object* _other);
	//vllt bekommt jedes object gleich ein sprite/animation
};
