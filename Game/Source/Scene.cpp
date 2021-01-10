#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Player.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	imgBgEarth = app->tex->Load("Assets/Textures/bg_earth.png");
	imgBgSpace = app->tex->Load("Assets/Textures/bg_space.png");
	imgClouds = app->tex->Load("Assets/Textures/clouds.png");

	propulsionPlatform.texture= app->tex->Load("Assets/Textures/platform.png");
	propulsionPlatform.textureLaser= app->tex->Load("Assets/Textures/laser_platform.png");
	SDL_QueryTexture(propulsionPlatform.texture, NULL, NULL, &rectPlatform.w, &rectPlatform.h);
	propulsionPlatform.position = { WINDOW_W / 2 - (rectPlatform.w >> 1), 10572 };

	for (int i = 0; i < 8; i++)
	{
		propulsionPlatform.laserFront->PushBack({118*i,0,118,208});
	}
	propulsionPlatform.laserFront->loop = true;
	propulsionPlatform.laserFront->speed = 0.2;
	for (int i = 0; i < 8; i++)
	{
		propulsionPlatform.laserBack->PushBack({ 101 * i,208,100,180 });
	}
	propulsionPlatform.laserBack->loop = true;
	propulsionPlatform.laserBack->speed = 0.2;
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//if(app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	//	app->render->camera.y += 10;

	//if(app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	//	app->render->camera.y -= 10;
	propulsionPlatform.laserFront->Update();
	propulsionPlatform.laserBack->Update();
	SDL_Rect rect = propulsionPlatform.laserFront->GetCurrentFrame();
	SDL_Rect rect2 = propulsionPlatform.laserBack->GetCurrentFrame();


	app->render->DrawTexture(imgBgEarth, 0,7346);
	app->render->DrawTexture(imgBgSpace, 0,0);
	app->player->PostUpdate();
	app->render->DrawTexture(imgClouds, 0,6620);
	app->render->DrawTexture(propulsionPlatform.texture, propulsionPlatform.position.x, propulsionPlatform.position.y);
	app->render->DrawTexture(propulsionPlatform.textureLaser, 390, 10760, &rect,1,23.5);
	app->render->DrawTexture(propulsionPlatform.textureLaser, 505, 10800, &rect2,1,22);
	app->render->DrawTextureFlip(propulsionPlatform.textureLaser, 1412, 10760, &rect,1,-23.5);
	app->render->DrawTextureFlip(propulsionPlatform.textureLaser, 1315, 10798, &rect2,1,-22);

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	app->tex->UnLoad(imgBgSpace);
	app->tex->UnLoad(imgBgEarth);
	app->tex->UnLoad(imgClouds);
	app->tex->UnLoad(propulsionPlatform.texture);

	return true;
}
