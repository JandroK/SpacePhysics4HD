#ifndef __SCENEINTRO_H__
#define __SCENEINTRO_H__

#include "SceneControl.h"
#include "Animation.h"

struct SDL_Texture;

struct AnimationIntro
{
	iPoint position;
	Animation* currentAnimation;
	SDL_Texture* texture;
};

class SceneIntro : public SceneControl
{
public:

	SceneIntro();

	// Destructor
	virtual ~SceneIntro();

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

	bool LoadState(pugi::xml_node& data);
	bool SaveState(pugi::xml_node& data)const;

	void ComprobeState(int id);

private:

	SDL_Texture* bgIntro;
	SDL_Texture* bgText;
	int speed = 66;
	float imgX = 0, imgY = 0;
	int imgW = 0, imgH = 0;
	bool transition;

	pugi::xml_document sceneFile;
	pugi::xml_node sceneStateFile;
};

#endif // __SCENEINTRO_H__





