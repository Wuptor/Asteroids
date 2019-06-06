#pragma once
#include <SDL.h>
#include "Object.h"

class Animation : public Object
{
public:
	Animation(int startx, int starty, int animwidth, int animheight, SDL_Texture* _used, int Wmultiply, int Hmultiply, int _rotation, int _animspeed);
	~Animation();

	static std::vector<Animation*> animations;
	bool loop, playing;
	SDL_Texture* Used;
	SDL_Rect Anim;
	SDL_Rect AnimPosition;
	int FrameWidth, FrameHeight;
	int TextureWidth, TextureHeight;
	int frametime, animSpeed;

	void update();
	void updatePos(int x, int y, int _rotation);

};