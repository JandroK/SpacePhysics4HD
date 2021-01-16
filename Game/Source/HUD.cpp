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
	font = new Font("Assets/Fonts/londrina.xml");
	
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

	//Funcion para devolver la fuerza G que le afecta a la nave 
	/*
	sprintf_s(hudText, 64, "G-Force: %.2f", 34.93113f);
    app->render->DrawText(font, hudText, drawPosX, drawPosY, 32, 0, { 255, 0, 255, 255 });
	*/
	drawPosY += 32;
	sprintf_s(hudText, 64, "Vertical Velocity: %.2f", abs(app->player->GetBody()->GetVelocity().y));
    app->render->DrawText(font, hudText, drawPosX, drawPosY, 32, 0, { 0, 0, 0, 255 });
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
