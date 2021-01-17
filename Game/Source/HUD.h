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
	
	char hudText[64]={0};
	Font* font;
	SDL_Color ColorLerp;
	Uint8 colorValue;
	float velocityLerp = 9;
	bool lerp;
	int gravity = 0;
};


#endif // !__HUD_H__
