#include "SceneManager.h"

#include "SceneLogo.h"
#include "SceneIntro.h"
#include "Scene.h"
//#include "SceneLose.h"
//#include "SceneWin.h"

#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"

#include "Defs.h"
#include "Log.h"

#include "SDL/include/SDL.h"

#define FADEOUT_TRANSITION_SPEED	2.0f
#define FADEIN_TRANSITION_SPEED		2.0f

SceneManager::SceneManager(Input* input, Render* render, Textures* tex) : Module()
{
	name.Create("scene_manager");

	sceneLogo = new SceneLogo();
	sceneIntro = new SceneIntro();
	scene = new Scene();
	//sceneWin = new SceneWin();
	//sceneLose = new SceneLose();

	AddScene(sceneLogo, false);
	AddScene(sceneIntro, false);
	AddScene(scene, false);
	//AddScene(sceneWin, false);
	//AddScene(sceneLose, false);

	onTransition = false;
	fadeOutCompleted = false;
	transitionAlpha = 0.0f;;

	this->input = input;
	this->render = render;
	this->tex = tex;
}

// Destructor
SceneManager::~SceneManager()
{}

// Called before render is available
bool SceneManager::Awake()
{
	LOG("Loading Scene manager");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool SceneManager::Start()
{
	current = new SceneLogo();
	current->Start();
	next = nullptr;

	//guiFont = app->fonts->Load("Assets/Textures/GUI/font_gui.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZ ", 2, 195, 48);

	return true;
}

// Called each loop iteration
bool SceneManager::PreUpdate()
{
	if (input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN) ViewRectangles = !ViewRectangles;

	return true;
}

// Called each loop iteration
bool SceneManager::Update(float dt)
{
	bool ret = true;
	if (!pause && (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) && (current->name == "scene" || current->name == "sceneLevel2"))
		pause = !pause;

	if (!onTransition)
	{
		ret = current->Update(dt);
	}
	else
	{
		if (!fadeOutCompleted)
		{
			transitionAlpha += (FADEOUT_TRANSITION_SPEED * dt);

			// NOTE: Due to float internal representation, condition jumps on 1.0f instead of 1.05f
			// For that reason we compare against 1.01f, to avoid last frame loading stop
			if (transitionAlpha > 1.01f)
			{
				transitionAlpha = 1.0f;

				current->CleanUp();	// Unload current screen
				next->Start();	// Load next screen
				//if (current->isContinue)app->LoadGameRequest();
				//else if (next->name == "scene" )// Save
				//{
				//	app->SaveGameRequest();
				//}
				RELEASE(current);	// Free current pointer
				current = next;		// Assign next pointer
				next = nullptr;

				// Activate fade out effect to next loaded screen
				fadeOutCompleted = true;
			}
		}
		else  // Transition fade out logic
		{
			transitionAlpha -= (FADEIN_TRANSITION_SPEED * dt);

			if (transitionAlpha < -0.01f)
			{
				transitionAlpha = 0.0f;
				fadeOutCompleted = false;
				onTransition = false;
			}
		}
	}

	if (current->transitionRequired)
	{
		onTransition = true;
		fadeOutCompleted = false;
		transitionAlpha = 0.0f;

		switch (current->nextScene)
		{
		case SceneType::LOGO: next = new SceneLogo(); break;
		case SceneType::INTRO: next = new SceneIntro(); break;
		case SceneType::LEVEL1: next = new Scene(); break;
		//case SceneType::WIN: next = new SceneWin(); break;
		//case SceneType::LOSE: next = new SceneLose(); break;
		default: break;
		}

		current->transitionRequired = false;
	}

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		current->TransitionToScene(SceneType::LEVEL1);
		lastLevel = 1;
	}
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called each loop iteration
bool SceneManager::PostUpdate()
{
	bool ret = true;
	// Draw current scene
	current->PostUpdate();
	// Draw full screen rectangle in front of everything
	if (onTransition)
	{
		render->DrawRectangle({ -app->render->camera.x, -app->render->camera.y, WINDOW_W, WINDOW_H }, 0, 0, 0, (unsigned char)(255.0f * transitionAlpha));
	}
	return ret;
}

void SceneManager::AddScene(SceneControl* scene, bool active)
{
	scene->active = active;
	scenes.Add(scene);
}

// Called before quitting
bool SceneManager::CleanUp()
{
	LOG("Freeing scene");
	if (current != nullptr) current->CleanUp();
	//app->fonts->UnLoad(guiFont);

	return true;
}

bool SceneManager::LoadState(pugi::xml_node& data)
{
	if (current->lastLevel == 1)current = scene;
	current->LoadState(data);
	return true;
}

bool SceneManager::SaveState(pugi::xml_node& data) const
{
	current->SaveState(data);
	return true;
}

