#ifndef __SCENELOSE_H__
#define __SCENELOSE_H__

#include "SceneControl.h"

struct SDL_Texture;

class SceneLose : public SceneControl
{
public:

	SceneLose();

	// Destructor
	virtual ~SceneLose();

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

	SDL_Texture* img;
	SDL_Texture* imgTex;
	int speed = 65;
	int imgX = 0, imgY = 0, imgW = 0, imgH = 0;
	bool transition;
	bool keyPressed;
};

#endif // __SCENELOSE_H__