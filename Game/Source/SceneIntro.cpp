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

	app->audio->PlayMusic("Assets/Audio/Music/imperial_march.ogg");
	bgIntro = app->tex->Load("Assets/Textures/scene_intro.png");
	bgText = app->tex->Load("Assets/Textures/burn_mission.png");


	SDL_QueryTexture(bgText, NULL, NULL, &imgW, &imgH);
	app->render->camera.x = app->render->camera.y = 0;
	imgX = WINDOW_W / 2 - imgW / 2;
	imgY = WINDOW_H;
	//ComprobeState(2);

	//app->sceneManager->SetPause(false);

	return true;
}

bool SceneIntro::PreUpdate()
{
	return true;
}

bool SceneIntro::Update(float dt)
{
	bool ret = true;

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

bool SceneIntro::LoadState(pugi::xml_node& data)
{
	lastLevel = data.child("level").attribute("lvl").as_int(0);
	//if (app->removeGame)lastLevel = 0;
	return true;
}

bool SceneIntro::SaveState(pugi::xml_node& data) const
{
	data.child("level").attribute("lvl").set_value(lastLevel);

	return true;
}

void SceneIntro::ComprobeState(int id)
{
	bool ret = true;
	pugi::xml_parse_result result = sceneFile.load_file("save_game.xml");

	// Check result for loading errors
	if (result == NULL)
	{
		LOG("Could not load map xml file savegame.xml. pugi error: %s", result.description());
		ret = false;
	}
	else
	{
		sceneStateFile = sceneFile.first_child();
		sceneStateFile = sceneStateFile.child("scene_manager");
		if (id == 2)LoadState(sceneStateFile);
	}
	sceneFile.reset();
}



