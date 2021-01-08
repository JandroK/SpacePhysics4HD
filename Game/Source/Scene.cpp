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
	imgPlatform = app->tex->Load("Assets/Textures/platform.png");
	SDL_QueryTexture(imgPlatform, NULL, NULL, &rectPlatform.w, &rectPlatform.h);
	//playerData.position = { WINDOW_W / 2 - (playerData.rectPlayer.w >> 1), 10530 };
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

	app->render->camera.y = -(app->player->playerData.position.y-WINDOW_H/2);

	app->render->DrawTexture(imgBgEarth, 0,7346);
	app->render->DrawTexture(imgBgSpace, 0,0);
	app->player->PostUpdate();
	app->render->DrawTexture(imgPlatform, WINDOW_W / 2 - (rectPlatform.w>>1), 10572);

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
	app->tex->UnLoad(imgPlatform);

	return true;
}
