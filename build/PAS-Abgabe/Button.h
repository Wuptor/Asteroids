#pragma once
#include "Object.h"
#include "EventManager.h"

class Button : public Object
{
public:
	Button(int _id, float _x, float _y, float _width, float _height, SDL_Texture* _used, SDL_Texture* _buttonHighlight);
	~Button();
	SDL_Texture * usedButton;
	SDL_Texture * currentlyUsed;
	SDL_Texture * Highlighted;
	bool mousepressed;
	int ButtonID;
	void Update();
};
