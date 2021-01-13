#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Player.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	platform = app->physics->CreateBody();
	moon = app->physics->CreateBody();

	imgBgEarth = app->tex->Load("Assets/Textures/bg_earth.png");
	imgBgSpace = app->tex->Load("Assets/Textures/bg_space.png");
	imgClouds = app->tex->Load("Assets/Textures/clouds.png");

	propulsionPlatform.texture= app->tex->Load("Assets/Textures/platform.png");
	propulsionPlatform.textureLaser= app->tex->Load("Assets/Textures/laser_platform.png");
	SDL_QueryTexture(propulsionPlatform.texture, NULL, NULL, &rectPlatform.w, &rectPlatform.h);
	propulsionPlatform.position = { WINDOW_W / 2 - (rectPlatform.w >> 1), 10572 };

	for (int i = 0; i < 8; i++)
	{
		propulsionPlatform.laserFront->PushBack({118*i,0,118,208});
	}
	propulsionPlatform.laserFront->loop = true;
	propulsionPlatform.laserFront->speed = 0.2;
	for (int i = 0; i < 8; i++)
	{
		propulsionPlatform.laserBack->PushBack({ 101 * i,208,100,180 });
	}
	propulsionPlatform.laserBack->loop = true;
	propulsionPlatform.laserBack->speed = 0.2;
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	platform->SetBodyType(BodyType::STATIC_BODY);
	platform->SetMass(1000);
	platform->SetPosition({ PIXEL_TO_METERS(742), PIXEL_TO_METERS(10656) });
	float w = 440;
	float h = 150;
	platform->SetDimension(PIXEL_TO_METERS(w), PIXEL_TO_METERS(h));
	fPoint axis = { WINDOW_W / 2, METERS_TO_PIXELS(platform->GetPosition().y) + h / 2 };

	float width = w / 2;
	float hight = h / 2;

	for (int i = 0; i < numPoints; i++)
	{
		if (i == 0 || i == 3)width = -abs(width);
		else width = abs(width);
		if (i == 0 || i == 1) hight = -abs(hight);
		else hight = abs(hight);
		pointsCollision[i].x = axis.x + width;
		pointsCollision[i].y = axis.y + hight;
	}
	
	platform->SetCollisions(pointsCollision, pointsCollision, numPoints);
	platform->SetAxisCM({ PIXEL_TO_METERS(axis.x) , PIXEL_TO_METERS (axis.y)});

	moon->SetBodyType(BodyType::STATIC_BODY);
	moon->SetMass(1000);
	moon->SetIsShpere(true);
	moon->SetRadio(PIXEL_TO_METERS(1000));
	moon->SetPosition({ 0, PIXEL_TO_METERS (-450) });
	moon->SetAxisCM({ PIXEL_TO_METERS(WINDOW_W/2) , PIXEL_TO_METERS(-450) });

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	//if(app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	//	app->render->camera.y += 10;

	//if(app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	//	app->render->camera.y -= 10;
	propulsionPlatform.laserFront->Update();
	propulsionPlatform.laserBack->Update();

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	SDL_Rect rect = propulsionPlatform.laserFront->GetCurrentFrame();
	SDL_Rect rect2 = propulsionPlatform.laserBack->GetCurrentFrame();
	/*SDL_Rect rectPlatform = {METERS_TO_PIXELS(platform->GetPosition().x),METERS_TO_PIXELS(platform->GetPosition().y),
		METERS_TO_PIXELS(platform->GetWidth()),METERS_TO_PIXELS(platform->GetHight())};*/

	app->render->DrawTexture(imgBgEarth, 0,7346);
	app->render->DrawTexture(imgBgSpace, 0,0);


	app->player->PostUpdate();
	app->render->DrawTexture(imgClouds, 0,6620);
	app->render->DrawTexture(propulsionPlatform.texture, propulsionPlatform.position.x, propulsionPlatform.position.y);
	app->render->DrawTexture(propulsionPlatform.textureLaser, 390, 10760, &rect,1,23.5);
	app->render->DrawTexture(propulsionPlatform.textureLaser, 505, 10800, &rect2,1,22);
	app->render->DrawTextureFlip(propulsionPlatform.textureLaser, 1412, 10760, &rect,1,-23.5);
	app->render->DrawTextureFlip(propulsionPlatform.textureLaser, 1315, 10798, &rect2,1,-22);

	int x1 = platform->GetPointsCollisionWorld()[0].x;
	int y1 = platform->GetPointsCollisionWorld()[0].y;
	int x2 = platform->GetPointsCollisionWorld()[1].x;
	int y2 = platform->GetPointsCollisionWorld()[1].y;
	int x3 = platform->GetPointsCollisionWorld()[2].x;
	int y3 = platform->GetPointsCollisionWorld()[2].y;
	int x4 = platform->GetPointsCollisionWorld()[3].x;
	int y4 = platform->GetPointsCollisionWorld()[3].y;

	app->render->DrawLine(x1, y1, x2, y2, 255, 0, 0);
	app->render->DrawLine(x2, y2, x3, y3, 255, 0, 0);
	app->render->DrawLine(x3, y3, x4, y4, 255, 0, 0);
	app->render->DrawLine(x4, y4, x1, y1, 255, 0, 0);

	app->render->DrawCircle2(METERS_TO_PIXELS(moon->GetAxis().x), METERS_TO_PIXELS(moon->GetAxis().y), METERS_TO_PIXELS(moon->GetRadio()));
	//app->render->DrawCircle2(WINDOW_W/2, -450, 1000);

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	app->tex->UnLoad(imgBgSpace);
	app->tex->UnLoad(imgBgEarth);
	app->tex->UnLoad(imgClouds);
	app->tex->UnLoad(propulsionPlatform.texture);
	app->tex->UnLoad(propulsionPlatform.textureLaser);

	delete idleAnim;
	delete propulsionPlatform.laserBack;
	delete propulsionPlatform.laserFront;

	return true;
}
