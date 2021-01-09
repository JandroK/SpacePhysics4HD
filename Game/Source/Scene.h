#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Animation.h"

struct SDL_Texture;

struct AnimationPlatform
{
	iPoint position;
	Animation* laserFront = new Animation();
	Animation* laserBack = new Animation();
	SDL_Texture* texture;
	SDL_Texture* textureLaser;
};

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

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
	SDL_Texture* imgBgSpace;
	SDL_Texture* imgBgEarth;
	
	SDL_Texture* imgClouds;
	SDL_Rect rectPlatform;

	AnimationPlatform propulsionPlatform;
	Animation* idleAnim = new Animation();
};

#endif // __SCENE_H__