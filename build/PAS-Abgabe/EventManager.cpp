#include "pch.h"
#include "EventManager.h"


bool EventManager::CheckMousePosition(SDL_Rect _position)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	bool inside = true;
	if (x < _position.x)
	{
		inside = false;
	}
	else if (x > _position.x + _position.w)
	{
		inside = false;
	}
	else if (y < _position.y)
	{
		inside = false;
	}
	else if (y > _position.y + _position.h)
	{
		inside = false;
	}
	return inside;
}

bool EventManager::ButtonClicked(SDL_Rect _position)
{
	if (CheckMousePosition(_position))
	{
		if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT))
		{
			return true;
		}
	}
	return false;
}