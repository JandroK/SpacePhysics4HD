#include "App.h"
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
	LOG("Loading Player Parser");
	bool ret = true;

	return ret;
}

bool HUD::Strart()
{
	return false;
}

bool HUD::PreUpdate()
{
	return false;
}

bool HUD::Update(float dt)
{
	return false;
}

bool HUD::PostUpdate()
{
	return false;
}

bool HUD::CleanUp()
{
	return false;
}
