#include "App.h"
#include "Window.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Player.h"
#include "Font.h"
#include "HUD.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Log.h"
#include "Defs.h"

HUD::HUD() : Module()
{
	name.Create("HUD");
}

HUD::~HUD()
{
}

bool HUD::Awake(pugi::xml_node&)
{
	LOG("Loading HUD");
	bool ret = true;

	return ret;
}

bool HUD::Start()
{
	miniShipTex = app->tex->Load("Assets/Textures/mini_ship.png");
	SDL_QueryTexture(miniShipTex,NULL,NULL,&miniShipRect.w,&miniShipRect.h);
	
	font = new Font("Assets/Fonts/cyberverse.xml");
	ColorLerp = { 255,255,255,255 };
	colorValue = ColorLerp.g;
	lerp = true;
	startFly = false;
	return true;
}

bool HUD::PreUpdate()
{
	return true;
}

bool HUD::Update(float dt)
{
	return true;
}

bool HUD::PostUpdate()
{
	int drawPosX = 10;
	int drawPosY = 20;
	int size = 64;
	float velocityY = 0;

	// Gravity
	{
		gravity = app->player->GetBody()->GetGravity() / app->player->GetBody()->GetMass();
		if (gravity == 0.f && startFly == false)gravity = 8.11f;
		else startFly = true;
		if (gravity < -0.5f) gravity += 4;
		sprintf_s(hudText, 64, "G-Force: %.2f", gravity);
		app->render->DrawText(font, hudText, drawPosX, drawPosY, size, 0, { 255, 255, 255, 255 });
	}

	// Vertical Velocity
	{
		drawPosY += size;
		velocityY = abs(app->player->GetBody()->GetVelocity().y);
		if (app->player->GetBody()->GetSleep())velocityY = 0;
		sprintf_s(hudText, size, "V Velocity: %.2f", velocityY);
		app->render->DrawText(font, hudText, drawPosX, drawPosY, size, 0, { 255, 255, 255, 255 });
	}

	// FUEL
	{
		drawPosY += size;
		float fuel = abs((app->player->playerData.fuel * 100) / 1500);
		sprintf_s(hudText, size, "Fuel: %3.0f", fuel);
		if (fuel > 15)
		{
			app->render->DrawText(font, hudText, drawPosX, drawPosY, size, 0, { 255, 255, 255, 255 });
			app->render->DrawText(font, "%", drawPosX + (7 * 32), drawPosY, size, 0, { 255, 255, 255, 255 });
		}
		else
		{
			Lerp(15, 235, colorValue, lerp, velocityLerp);

			LOG("%d", colorValue);

			app->render->DrawText(font, hudText, drawPosX, drawPosY, size, 0, { 255, colorValue, colorValue, 255 });
			app->render->DrawText(font, "%", drawPosX + (7 * 32), drawPosY, size, 0, { 255, colorValue, colorValue, 255 });
		}
	}

	// Lives / Shield
	{
		drawPosY += size;
		sprintf_s(hudText, size, "Shields: %3d", app->player->GetBody()->GetLives() * 10);
		app->render->DrawText(font, hudText, drawPosX, drawPosY, size, 0, { 255, colorValue, colorValue, 255 });
		app->render->DrawText(font, "%", drawPosX + (9 * 32), drawPosY, size, 0, { 255, colorValue, colorValue, 255 });

	}

	// Visual altimeter
	{	
		int tall = 12;
		float wRect = (10538 - (WINDOW_H / 2)) / tall;
		drawPosY = WINDOW_H /2 - (wRect/2);
		drawPosX = WINDOW_W - 80;
		SDL_Rect travel = { drawPosX , -app->render->camera.y + drawPosY ,30,(10538-(WINDOW_H/2)) / tall };
		app->render->DrawRectangle(travel, 255, 255, 255, 255);

		SDL_Rect shypCos = { drawPosX - 5, (-app->render->camera.y + drawPosY) + (-app->render->camera.y / tall), 40, 20 };
		app->render->DrawTexture(miniShipTex, drawPosX - 7, (-app->render->camera.y + drawPosY) + (-app->render->camera.y / tall) - (miniShipRect.h/2),&miniShipRect,1,180 * app->sceneManager->scene->GetWin());
	}

	return true;
}

bool HUD::CleanUp()
{
	if (!active)
		return true;
	active = false;

	delete font;

	return true;
}

void HUD::Lerp(float min, float max, Uint8 &value, bool &onOff, float &velocity)
{
	if (onOff)
	{
		value -= velocity;
		if (value <= min)onOff = false;
	}
	else
	{
		value += velocity;
		if (value >= max)onOff = true;
	}
}
