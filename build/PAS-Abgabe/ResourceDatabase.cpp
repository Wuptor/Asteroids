#include "pch.h"
#include "ResourceDatabase.h"


std::map<std::string, SDL_Texture*> ResourceDatabase::Textures;

SDL_Texture* ResourceDatabase::ILoadImage(const char* _dateiname, SDL_Renderer* _renderer) //ImageImport rename?
{
	SDL_Surface* temp = SDL_LoadBMP(_dateiname);
	if (temp == nullptr)
	{
		std::cout << "picture: " << _dateiname << " was not found" << "\n";
		return nullptr; 
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, temp);
	SDL_FreeSurface(temp);
	return texture;
}