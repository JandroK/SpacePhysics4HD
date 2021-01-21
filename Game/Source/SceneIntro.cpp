#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "SceneIntro.h"
#include "SceneManager.h"

#include <SDL_mixer\include\SDL_mixer.h>

#include "Defs.h"
#include "Log.h"

SceneIntro::SceneIntro()
{
	active = true;
	name.Create("sceneIntro");
}

SceneIntro::~SceneIntro()
{
}

bool SceneIntro::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

bool SceneIntro::Start()
{
	// GUI: Initialize required controls for the screen
	int margin = 7;
	int padding = 98;
	int yPosition = 330 + (margin * 1);

	app->SetLastScene((Module*)this);
	transition = false;

	// Load textures and Music
	bgIntro = app->tex->Load("Assets/Textures/scene_intro.png");
	bgText = app->tex->Load("Assets/Textures/burn_mission.png");
	app->audio->PlayMusic("Assets/Audio/Music/imperial_march.ogg");

	// Get dimensions of texture
	SDL_QueryTexture(bgText, NULL, NULL, &imgW, &imgH);
	app->render->camera.x = app->render->camera.y = 0;
	imgX = WINDOW_W / 2 - imgW / 2;
	imgY = WINDOW_H;

	return true;
}

bool SceneIntro::PreUpdate()
{
	return true;
}

bool SceneIntro::Update(float dt)
{
	bool ret = true;

	// If player press the sapce or the text has left the secene transition to Level1
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || imgY + imgH < 0)
	{
		TransitionToScene(SceneType::LEVEL1);
	}
	imgY -= dt * speed;
	return ret;
}

bool SceneIntro::PostUpdate()
{
	bool ret = true;
	SDL_Rect rectIntro;

	app->render->DrawTexture(bgIntro, app->render->camera.x, app->render->camera.y);
	app->render->DrawTexture(bgText, imgX, imgY);

	return ret;
}

bool SceneIntro::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");
	Mix_HaltMusic();
	app->tex->UnLoad(bgIntro);
	app->tex->UnLoad(bgText);

	bgIntro = nullptr;
	active = false;

	return true;
}



