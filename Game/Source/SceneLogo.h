#ifndef __SCENELOGO_H__
#define __SCENELOGO_H__

#include "SceneControl.h"

struct SDL_Texture;

class SceneLogo : public SceneControl
{
public:

	SceneLogo();

	// Destructor
	virtual ~SceneLogo();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();
	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();


private:

	SDL_Rect logo;
	SDL_Texture* img;
	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;
	uint introFx;

	int state;
	float timeCounter;
	float logoAlpha;


};

#endif // __SCENELOGO_H__

