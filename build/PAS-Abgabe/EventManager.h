#pragma once
#include <SDL.h>

class EventManager
{
public:
	static bool CheckMousePosition(SDL_Rect _position);
	static bool ButtonClicked(SDL_Rect _position);
};

