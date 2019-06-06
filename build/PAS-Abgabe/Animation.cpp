#include "pch.h"
#include "Animation.h"

std::vector<Animation*> Animation::animations;

Animation::Animation(int startx, int starty, int animwidth, int animheight, SDL_Texture* _used, int Wmultiply, int Hmultiply, int _rotation, int _animspeed)
{
	AnimPosition.w = animwidth;
	AnimPosition.h = animheight;
	AnimPosition.x = startx;
	AnimPosition.y = starty;

	SDL_QueryTexture(_used, NULL, NULL, &TextureWidth, &TextureHeight);
	FrameWidth = TextureWidth / Wmultiply;
	FrameHeight = TextureHeight / Hmultiply;

	Anim.x = 0;
	Anim.y = 0;
	Anim.w = FrameWidth;
	Anim.h = FrameHeight;

	frametime = 0;
	rotation = _rotation;
	playing = true;
	loop = false;
	Used = _used;
	animSpeed = _animspeed;
}


Animation::~Animation()
{
}



void Animation::update()
{
	frametime++;
	if (frametime == animSpeed && playing == true)
	{
		frametime = 0;
		Anim.x += FrameWidth;
		if (Anim.x >= TextureWidth)
		{
			Anim.x = 0;
			Anim.y += FrameHeight;
			if (Anim.y >= TextureHeight)
			{
				Anim.y = 0;
				if (loop == false)
				{
					playing = false;
				}
			}
		}
	}
}

void Animation::updatePos(int x, int y, int _rotation)
{
	AnimPosition.x = x - AnimPosition.w / 2;
	AnimPosition.y = y - AnimPosition.h / 2;
	rotation = _rotation;
}