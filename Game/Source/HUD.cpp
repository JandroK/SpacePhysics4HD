#include "App.h"
#include "Window.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Player.h"
#include "Font.h"
#include "HUD.h"

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
	font = new Font("Assets/Fonts/cyberverse.xml");
	
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
	
	{
		// Funcion para devolver la fuerza G que le afecta a la nave 
		sprintf_s(hudText, 64, "G-Force: %.2f", 34.21f); 
		app->render->DrawText(font, hudText, drawPosX, drawPosY, size, 0, { 255, 255, 255, 255 });

		drawPosY += size;

		sprintf_s(hudText, size, "Fuel: %3.0f", abs((app->player->playerData.fuel * 100) / 1500));
		app->render->DrawText(font, hudText, drawPosX, drawPosY, size, 0, { 255, 255, 255, 255 });
		app->render->DrawText(font, "%", drawPosX+(7*32), drawPosY, size, 0, { 255, 255, 255, 255 });

		drawPosY += size;
		sprintf_s(hudText, size, "V Velocity: %.2f", abs(app->player->GetBody()->GetVelocity().y));
		app->render->DrawText(font, hudText, drawPosX, drawPosY, size, 0, { 255, 255, 255, 255 });
	}

	{	// Visual altimeter
		int tall = 12;
		float wRect = (10538 - (WINDOW_H / 2)) / tall;
		drawPosY = WINDOW_H /2 - (wRect/2);
		drawPosX = WINDOW_W - 80;
		SDL_Rect travel = { drawPosX , -app->render->camera.y + drawPosY ,30,(10538-(WINDOW_H/2)) / tall };
		app->render->DrawRectangle(travel, 255, 255, 255, 255);

		SDL_Rect shypCos = { drawPosX - 5, (-app->render->camera.y + drawPosY) + (-app->render->camera.y / tall), 40, 20 };
		app->render->DrawRectangle(shypCos, 255, 165, 0, 255);
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
