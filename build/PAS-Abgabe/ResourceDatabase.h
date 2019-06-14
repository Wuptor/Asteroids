#pragma once
#include <SDL.h>
#include <map>
#include <string>
#include <vector>
#include <iostream>
//#include "Animation.h"

class ResourceDatabase
{
public:
	static std::map<std::string, SDL_Texture*> Textures;
	//static std::map<std::string, Animation> Animations; //überlegen ob pointer evtl besser wären //copy constructor usw beachten
	//static std::vector<Text*> Texts;
	static SDL_Texture* ILoadImage(const char* _dateiname, SDL_Renderer* _renderer);
};

