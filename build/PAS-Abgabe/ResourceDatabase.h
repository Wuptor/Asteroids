#pragma once
#include <SDL.h>
#include <map>
#include <string>
#include <vector>
#include <iostream>

class ResourceDatabase
{
public:
	static std::map<std::string, SDL_Texture*> Textures;
	//static std::vector<Text*> Texts;
	static SDL_Texture* ILoadImage(const char* _dateiname, SDL_Renderer* _renderer);
};

