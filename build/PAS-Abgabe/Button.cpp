#include "pch.h"
#include "Button.h"


Button::Button(int _id, float _x, float _y, float _width, float _height, SDL_Texture* _used, SDL_Texture* _buttonHighlight)
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


Button::~Button()
{
}

void Button::Update()
{
	if (EventManager::CheckMousePosition(DrawObject))
	{
		currentlyUsed = Highlighted;
	}
	else
	{
		currentlyUsed = usedButton;
	}
}