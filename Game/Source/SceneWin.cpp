#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "SceneWin.h"
#include "SceneManager.h"

#include <SDL_mixer\include\SDL_mixer.h>

#include "Defs.h"
#include "Log.h"

SceneWin::SceneWin()
{
	active = true;
	name.Create("sceneWin");
}

SceneWin::~SceneWin()
{
}

bool SceneWin::Awake()
{
	LOG("Loading SceneWin");
	bool ret = true;
	return ret;
}

bool SceneWin::Start()
{
	app->SetLastScene((Module*)this);
	transition = false;

	img = app->tex->Load("Assets/Textures/win_screen.png");
	bgText = app->tex->Load("Assets/Textures/tex_win.png");
	app->audio->PlayMusic("Assets/Audio/Music/music_win.ogg");

	SDL_QueryTexture(bgText, NULL, NULL, &imgW, &imgH);
	app->render->camera.x = app->render->camera.y = 0;
	imgX = WINDOW_W / 2 - imgW / 2;
	imgY = WINDOW_H;

	return true;
}

bool SceneWin::PreUpdate()
{
	return true;
}

bool SceneWin::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN || imgY + imgH < 0)
	{
		TransitionToScene(SceneType::LEVEL1);
	}
	imgY -= dt * speed;
	return true;
}

bool SceneWin::PostUpdate()
{
	bool ret = true;

	app->render->DrawTexture(img, app->render->camera.x, app->render->camera.y);
	app->render->DrawTexture(bgText, imgX, imgY);

	return ret;
}

bool SceneWin::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");
	Mix_HaltMusic();
	app->tex->UnLoad(img);
	app->tex->UnLoad(bgText);
	img = nullptr;
	bgText = nullptr;
	active = false;
	return true;
}

