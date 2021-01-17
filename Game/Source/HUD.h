#ifndef __HUD_H__
#define __HUD_H__

#include "Textures.h"
#include "p2Point.h"

#include "SDL/include/SDL.h"

class Font;

class HUD : public Module
{
public:
	HUD();
	~HUD();

	bool Awake(pugi::xml_node&);

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

private:
	
	void Lerp(float min, float max, Uint8 &value, bool &onOff, float &velocity);

	SDL_Texture *miniShipTex;

	SDL_Rect miniShipRect = {0,0,0,0};

	char hudText[64]={0};
	Font* font;
	SDL_Color ColorLerp = {0,0,0,0};
	Uint8 colorValue = 0;
	float velocityLerp = 9;
	bool lerp = false;
	float gravity = 0;
	bool startFly = false;
};
#endif // !__HUD_H__
