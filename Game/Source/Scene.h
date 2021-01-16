#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Animation.h"
#include "Physics.h"
#include "SceneControl.h"

struct SDL_Texture;

struct AnimationPlatform
{
	iPoint position;
	Animation* laserFront;
	Animation* laserBack;
	SDL_Texture* texture;
	SDL_Texture* textureLaser;
};

struct Asteroids
{
	Body* asteroid;
	Animation* currentAnimAsteroid;
};

class Scene : public SceneControl
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

	void ComprobeStateAsteroids();

	// Called before all Updates
	bool PostUpdate();


	// Create Entities
	void CreateWalls(Body* body, fPoint position, float w, float h);
	void CreateEntity();
	void DrawStaticBodies(Body* wallDown);

	// Get and Set Win
	int GetWin() { return win; };
	int GetLose() { return lose; };
	void SetWin(int _win) { win = _win; };
	void SetLose(int _lose) { lose = _lose; };

	// Called before quitting
	bool CleanUp();

private:
	Body* platform;
	Body* platformMoon;
	Body* moon;
	Body* wallLeft;
	Body* wallRight;
	Body* wallDown;

	int numAsteroids = 40;
	List<Body*> asteroids;

	static const int numPoints = 4;
	fPoint pointsCollision[numPoints];

	SDL_Texture* imgBgSpace;
	SDL_Texture* imgBgEarth;
	
	SDL_Texture* imgClouds;
	SDL_Texture* imgPlatformMoon;
	SDL_Texture* imgAsteroids;
	SDL_Rect rectPlatform;

	AnimationPlatform propulsionPlatform;
	Animation* currentAnimAsteroid;
	Animation* idleAsteroidAnim;
	Animation* explosionAsteroidAnim;

	int win = 0;
	bool lose = false;
};

#endif // __SCENE_H__