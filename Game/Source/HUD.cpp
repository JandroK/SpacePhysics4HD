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
	
	missionColorValue = 255;
	missionLerp = false;
	
	fuelColorValue = 255;
	fuelLerp = true;

	shieldColorValue = 255;
	shieldLerp = true;

	startFly = false;

	miliseconds = 0;
	miliseconds2 = 0;
	seconds = 0;
	minuts = 0;

	return true;
}

bool HUD::PreUpdate()
{
	return true;
}

bool HUD::Update(float dt)
{
	if (startFly)
	{
		miliseconds = timer.Read() - minuts * 60000;
		Chronometer();
	}
	
	return true;
}

bool HUD::PostUpdate()
{
	int drawPosX = 10;
	int drawPosY = 20;
	int size = 64;
	float velocityY = 0;
	int letersN = 0;

	//---------- MISSION
	{
		velocityY = abs(app->player->GetBody()->GetVelocity().y);
		if (app->player->GetBody()->GetSleep())velocityY = 0;

		if (app->sceneManager->scene->GetWin() == 0) {
			sprintf_s(hudText, 30, "Go to the Moon");
			letersN = 13;
		}
		else {
			if (missionColorValue <= 235 || !missionLerp) Lerp(15, 235, missionColorValue, missionLerp, 5);
			else missionColorValue = 255;
			
			sprintf_s(hudText, 30, "Returns to Earth");
			letersN = 14;
		}
		app->render->DrawText(font, hudText, WINDOW_W - (32 * letersN), drawPosY, size, 0, { 255, missionColorValue, missionColorValue, 255 });
	}

	//---------- INFORMATION
	drawPosY -= size;
	// Gravity
	{
		drawPosY += size;
		gravity = abs(app->player->GetBody()->GetGravity());
		if (gravity == 0.f && startFly == false)gravity = 770;
		else if (startFly == false) { 
			startFly = true; 
			timer.Start();
		}
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

	// Time
	{
		drawPosY += size;
		sprintf_s(hudText, size, "Time: %d:%02d:%02d", minuts, miliseconds / 100, miliseconds2);
		app->render->DrawText(font, hudText, drawPosX, drawPosY, size, 0, { 255, 255, 255, 255 });

	}

	// FUEL
	{
		drawPosY += size;
		float fuel = abs((app->player->playerData.fuel * 100) / 1500);
		sprintf_s(hudText, size, "Fuel: %3.0f", fuel);
		if (fuel < 15) Lerp(15, 235, fuelColorValue, fuelLerp, velocityLerp);
		app->render->DrawText(font, hudText, drawPosX, drawPosY, size, 0, { 255, fuelColorValue, fuelColorValue, 255 });
		app->render->DrawText(font, "%", drawPosX + (7 * 32), drawPosY, size, 0, { 255, fuelColorValue, fuelColorValue, 255 });
	}

	// Lives / Shield
	{
		drawPosY += size;
		int shield = app->player->GetBody()->GetLives() * 10;
		sprintf_s(hudText, size, "Shields: %3d", shield);
		if (shield <= 20) Lerp(15, 235, shieldColorValue, shieldLerp, velocityLerp);
		app->render->DrawText(font, hudText, drawPosX, drawPosY, size, 0, { 255, shieldColorValue, shieldColorValue, 255 });
		app->render->DrawText(font, "%", drawPosX + (9 * 32), drawPosY, size, 0, { 255, shieldColorValue, shieldColorValue, 255 });

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


void HUD::Lerp(float min, float max, Uint8 &value, bool &onOff, float velocity)
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

void HUD::Chronometer()
{
	if (miliseconds >= 60000)
	{
		minuts++;
	}
	miliseconds = miliseconds * 0.1;

	int centenas = 0;
	if (miliseconds >= 100)
	{
		centenas = miliseconds / 100;
		miliseconds2 = miliseconds - (centenas * 100);
	}
}

bool HUD::CleanUp()
{
	if (!active)
		return true;
	active = false;

	delete font;

	return true;
}