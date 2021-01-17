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
	
	void Lerp(float min, float max, Uint8 &value, bool &onOff, float velocity);

	void Chronometer();


	SDL_Texture *miniShipTex;

	SDL_Rect miniShipRect = {0,0,0,0};

	char hudText[64]={0};
	Font* font;

	Uint8 missionColorValue = 0;
	bool missionLerp = false;

	Uint8 shieldColorValue = 0;
	bool shieldLerp = false;

	Uint8 fuelColorValue = 0;
	bool fuelLerp = false;

	float velocityLerp = 9;

	float gravity = 0;
	bool startFly = false;

	Timer timer;
	uint miliseconds = 0;
	uint miliseconds2 = 0;
	uint seconds = 0;
	uint minuts = 0;
	Timer auxTimePause;
};
#endif // !__HUD_H__
