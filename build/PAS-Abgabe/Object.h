#pragma once
#include <SDL.h>
#include <vector>
#include <deque>


#define SCREEN_WIDTH  640 
#define SCREEN_HEIGHT 480


class Object
{
public:
	enum Type {player, asteroid, enemy, missile, emissile, pickup, neutral}; //Emissile mit Missile verschmelzen //neutral noch besser aufdröslen //evtl reicht es bei type --> player, lethal, nonlethal, nocollission oder so ähnlich
	Object(Type _type);
	~Object();

	static int ObjectID;
	static std::vector<Object>* Entities;
	Type mObjectType;
	float posX, posY, radius, rotation; //vllt postion usw auf int umstellen
	const int ID;
	int width, height;
	bool alive;
	SDL_Rect DrawObject;
	SDL_Texture* texture;
	void KeepInField();
	void DeleteOnScreenExit();
	bool CheckCollision(Object* _other);
	virtual void update();
	void SetPosAndRot(int _posX, int _posY, int _rotation);
	//vllt bekommt jedes object gleich ein sprite/animation
	//vllt ein enum Type mit enemy missile player usw --> alle objekte in eine einzige liste und type als unterscheidungsmöglichkeit
};
