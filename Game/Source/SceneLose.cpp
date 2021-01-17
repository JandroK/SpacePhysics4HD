#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "SceneLose.h"
#include "SceneManager.h"

#include <SDL_mixer\include\SDL_mixer.h>

#include "Defs.h"
#include "Log.h"

SceneLose::SceneLose()
{
	active = true;
	name.Create("sceneLose");
}

SceneLose::~SceneLose()
{
}

bool SceneLose::Awake()
{
	LOG("Loading SceneLose");
	bool ret = true;
	return ret;
}

bool SceneLose::Start()
{
	app->SetLastScene((Module*)this);
	transition = false;

	app->audio->PlayMusic("Assets/Audio/Music/music_lose.ogg");
	img = app->tex->Load("Assets/Textures/lose_screen.png");
	imgTex = app->tex->Load("Assets/Textures/mission_failed.png");

	SDL_QueryTexture(imgTex, NULL, NULL, &imgW, &imgH);
	app->render->camera.x = app->render->camera.y = 0;
	imgX = WINDOW_W / 2 - imgW / 2;
	imgY = WINDOW_H;

	return true;
}

bool SceneLose::PreUpdate()
{
	return true;
}

bool SceneLose::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || imgY + imgH < 0)
	{
		TransitionToScene(SceneType::LEVEL1);
	}
	imgY -= dt * speed;
	return true;
}

bool SceneLose::PostUpdate()
{
	bool ret = true;

	app->render->DrawTexture(img, app->render->camera.x, app->render->camera.y);
	app->render->DrawTexture(imgTex, imgX, imgY);

	return ret;
}

bool SceneLose::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");

	Mix_HaltMusic();
	app->tex->UnLoad(img);
	app->tex->UnLoad(imgTex);
	img = nullptr;
	imgTex = nullptr;

	active = false;
	return true;
}
