#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Font.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Player.h"
#include "HUD.h"
#include "SceneManager.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : SceneControl()
{
	active = true;
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
	// Active Motor Engine
	app->physics->Init();
	app->physics->Start();

	// Create new Bodies
	platform = new Body;
	platformMoon = new Body;
	moon = new Body;
	wallLeft = new Body;
	wallRight = new Body;
	wallDown = new Body;
	sea = new Body;

	// Create new Animations
	idleAsteroidAnim = new Animation();
	explosionAsteroidAnim = new Animation();
	propulsionPlatform.laserFront = new Animation();
	propulsionPlatform.laserBack = new Animation();
	waterAnim = new Animation();

	// Load textures
	imgBgEarth = app->tex->Load("Assets/Textures/bg_earth.png");
	imgBgSpace = app->tex->Load("Assets/Textures/bg_space.png");
	imgPlatformMoon = app->tex->Load("Assets/Textures/platformMoon.png");
	imgClouds = app->tex->Load("Assets/Textures/clouds.png");
	imgAsteroids = app->tex->Load("Assets/Textures/asteroids_explosion.png");
	propulsionPlatform.texture= app->tex->Load("Assets/Textures/platform.png");
	propulsionPlatform.textureLaser= app->tex->Load("Assets/Textures/laser_platform.png");
	imgWater = app->tex->Load("Assets/Textures/water_anim.png");

	// Load music anf Fx
	app->audio->PlayMusic("Assets/Audio/Music/galactic_empire.ogg");
	explosionFx = app->audio->LoadFx("Assets/Audio/Fx/explosion_asteroid.wav");

	// Get dimensions of texture
	SDL_QueryTexture(propulsionPlatform.texture, NULL, NULL, &rectPlatform.w, &rectPlatform.h);
	propulsionPlatform.position = { WINDOW_W / 2 - (rectPlatform.w >> 1), 10316 };

	// Animation of Asteroids 
	idleAsteroidAnim->PushBack({0,495,165,165});
	idleAsteroidAnim->loop = true;

	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			explosionAsteroidAnim->PushBack({ 0 + (i * 165),0 + (j * 165),165,165 });
		}
	}
	explosionAsteroidAnim->loop = false;

	// Animations of the propulsion of the air platform
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

	// Animation Water
	for (int  i = 0; i < 14; i++)
	{
		waterAnim->PushBack({0, 0 + (371*i), 1920,371});
	}
	waterAnim->loop = true;
	waterAnim->speed = 0.2;

	// Set values of the bodies
	CreateWalls(platform, { 742, 10400 }, 440, 150, BodyClass::PLATFORMS);
	CreateWalls(platformMoon, { WINDOW_W/2-(390/2), 415 }, 390, 190, BodyClass::PLATFORMS);
	CreateWalls(moon, { 0, 0 }, WINDOW_W, 414, BodyClass::PLATFORMS);
	CreateWalls(wallLeft, { -150, 0 }, 150, 11081, BodyClass::PLATFORMS);
	CreateWalls(wallRight, { WINDOW_W, 0 }, 150, 11081, BodyClass::PLATFORMS);
	CreateWalls(wallDown, { 0, 11081 }, WINDOW_W, 150, BodyClass::PLATFORMS);
	CreateWalls(sea, {0,10720}, WINDOW_W, 360, BodyClass::SEA);
	CreateEntity();

	// Active Palyer and HUD
	app->player->Init();
	app->player->Start();

	app->hud->Init();
	app->hud->Start();

	app->sceneManager->scene->win = 0;
	app->sceneManager->scene->lose = false;

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
	ComprobeStateAsteroids();

	// Update de animations
	explosionAsteroidAnim->speed = dt * 3;
	propulsionPlatform.laserFront->Update();
	propulsionPlatform.laserBack->Update();
	waterAnim->Update();

	// Update all animations asteroids
	ListItem<Asteroids*>* item;
	for (item = asteroids.start; item != NULL; item = item->next)
	{
		item->data->currentAnimAsteroid->Update();
	}

	// Check win or lose
	if(app->sceneManager->scene->win == 2)TransitionToScene(SceneType::WIN);
	if(app->sceneManager->scene->lose == true)TransitionToScene(SceneType::LOSE);

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	// Get current frame of the texture
	SDL_Rect rect = propulsionPlatform.laserFront->GetCurrentFrame();
	SDL_Rect rect2 = propulsionPlatform.laserBack->GetCurrentFrame();
	SDL_Rect rect3; // Asteroid anim
	SDL_Rect rect4 = waterAnim->GetCurrentFrame();

	// Draw textures
	app->render->DrawTexture(imgBgEarth, 0,7346);
	app->render->DrawTexture(imgBgSpace, 0,0);
	app->render->DrawTexture(imgPlatformMoon, 647, 390);
	app->player->PostUpdate();
	app->render->DrawTexture(imgClouds, 0,6620);
	app->render->DrawTexture(imgWater, 0,10711, &rect4);
	app->render->DrawTexture(propulsionPlatform.texture, propulsionPlatform.position.x, propulsionPlatform.position.y);
	app->render->DrawTexture(propulsionPlatform.textureLaser, 390, 10504, &rect,1,23.5);
	app->render->DrawTexture(propulsionPlatform.textureLaser, 505, 10544, &rect2,1,22);
	app->render->DrawTextureFlip(propulsionPlatform.textureLaser, 1412, 10504, &rect,1,-23.5);
	app->render->DrawTextureFlip(propulsionPlatform.textureLaser, 1315, 10542, &rect2,1,-22);

	// Draw collision Box
	if (app->sceneManager->GetViewCollisions())
	{
		DrawStaticBodies(platform);
		DrawStaticBodies(platformMoon);
		DrawStaticBodies(moon);
		DrawStaticBodies(wallLeft);
		DrawStaticBodies(wallDown);
		DrawStaticBodies(wallDown);
		DrawStaticBodies(sea);
	}

	// Draw all asteroids
	ListItem<Asteroids*>* item;
	for (item = asteroids.start; item != NULL; item = item->next)
	{
		if (item->data->asteroid->GetBodyState() != BodyState::IDLE)
		{
			item->data->currentAnimAsteroid = explosionAsteroidAnim;
		}

		rect3 = item->data->currentAnimAsteroid->GetCurrentFrame();
		if (item->data->asteroid->GetBodyState() != BodyState::DEAD)
		{
			if (app->sceneManager->GetViewCollisions())
			{
				app->render->DrawCircle2(METERS_TO_PIXELS(item->data->asteroid->GetAxis().x),
					METERS_TO_PIXELS(item->data->asteroid->GetAxis().y), METERS_TO_PIXELS(item->data->asteroid->GetRadio()));
			}
			app->render->DrawTexture(imgAsteroids, METERS_TO_PIXELS(item->data->asteroid->GetAxis().x) - METERS_TO_PIXELS(item->data->asteroid->GetRadio()) - 31,
				METERS_TO_PIXELS(item->data->asteroid->GetAxis().y) - METERS_TO_PIXELS(item->data->asteroid->GetRadio()) - 33, &rect3, 1, item->data->asteroid->GetRotation());
		}
	}

	return ret;
}

void Scene::DrawStaticBodies(Body* body)
{
	int x1, y1, x2, y2;
	for (int i = 0; i < numPoints; i++)
	{
		x1 = body->GetPointsCollisionWorld()[i].x;
		y1 = body->GetPointsCollisionWorld()[i].y;
		if (i == 3)i = -1;
		x2 = body->GetPointsCollisionWorld()[i + 1].x;
		y2 = body->GetPointsCollisionWorld()[i + 1].y;

		app->render->DrawLine(x1, y1, x2, y2, 255, 0, 0);
		if (i == -1)break;
	}
}

void Scene::CreateWalls(Body* body, fPoint position, float w, float h, BodyClass bodyClass)
{
	// Set the values to new body
	body->SetMass(100000);
	body->SetBodyType(BodyType::STATIC_BODY);
	body->SetClassType(bodyClass);
	body->SetPosition({ PIXEL_TO_METERS(position.x), PIXEL_TO_METERS(position.y) });
	body->SetDimension(PIXEL_TO_METERS(w), PIXEL_TO_METERS(h));
	fPoint axis = { METERS_TO_PIXELS(body->GetPosition().x) + w / 2, METERS_TO_PIXELS(body->GetPosition().y) + h / 2 };

	float width = w / 2;
	float hight = h / 2;

	// Fill the matrix of points in clockwise
	for (int i = 0; i < numPoints; i++)
	{
		if (i == 0 || i == 3)width = -abs(width);
		else width = abs(width);
		if (i == 0 || i == 1) hight = -abs(hight);
		else hight = abs(hight);
		pointsCollision[i].x = axis.x + width;
		pointsCollision[i].y = axis.y + hight;
	}

	body->SetCollisions(pointsCollision, pointsCollision, numPoints);
	body->SetAxisCM({ PIXEL_TO_METERS(axis.x) , PIXEL_TO_METERS(axis.y) });

	// Add body of the world
	app->physics->CreateBody(body);
}

void Scene::CreateEntity()
{
	Asteroids* b = new Asteroids;
	asteroids.Add(b);

	ListItem<Asteroids*>* item = asteroids.start;
	ListItem<Asteroids*>* itemPrev;

	fPoint pos;
	fPoint posM;
	float distanceBetweenAxis;
	
	// Create as many bodies as	numAsteroids 
	for (int i = 0; i < numAsteroids; item = item->next, i++)
	{
		Body* newAsteroid = new Body;
		item->data->asteroid = newAsteroid;
		item->data->currentAnimAsteroid = idleAsteroidAnim;
		// Randomize the massa of bodies for everyone is different
		int m = rand() % 100 + 200;
		item->data->asteroid->SetMass(m);
		item->data->asteroid->SetClassType(BodyClass::ASTEROIDS);
		item->data->asteroid->SetRadio(PIXEL_TO_METERS(50));

		// Randomize position of spawn
		pos.x = rand() % WINDOW_W - 100 + 100;
		pos.y = rand() % 3500 + 2000;
		// But this position must be different to the rest asteroids
		for (itemPrev = item; itemPrev != NULL; itemPrev = itemPrev->prev)
		{
			posM = { itemPrev->data->asteroid->GetAxis().x - PIXEL_TO_METERS(pos.x), itemPrev->data->asteroid->GetAxis().y - PIXEL_TO_METERS(pos.y) };
			distanceBetweenAxis = app->physics->CalculateModule(posM);
			// If this asteroid is collision with other calculate again the position of the new asteroid
			if (itemPrev->data->asteroid->GetRadio() + item->data->asteroid->GetRadio() > distanceBetweenAxis - 1)
			{
				pos.x = rand() % WINDOW_W - 100 + 100;
				pos.y = rand() % 3500 + 2000;
				itemPrev = item;
			}
			// In this way, never there will be asteroids in the same position and can buid a procedural level 
		}
		
		item->data->asteroid->SetPosition({ PIXEL_TO_METERS(pos.x), PIXEL_TO_METERS(pos.y) });
		item->data->asteroid->SetAxisCM(item->data->asteroid->GetPosition());

		// The asteroids have inertia and collide with each other
		float velX = rand() % 4 - 2;
		if (velX < 0) item->data->asteroid->AddTorque(-60);
		if (velX > 0) item->data->asteroid->AddTorque(60);
		item->data->asteroid->SetCoeficientDrag(0.47);
		item->data->asteroid->SetSurface(2);

		app->physics->CreateBody(item->data->asteroid);
		// This  conditional is so that at the end of the loop it doesn't create an empty body 
		if (i < numAsteroids-1)
		{
			Asteroids* b = new Asteroids;
			asteroids.Add(b);
		}
	}
}

void Scene::ComprobeStateAsteroids()
{
	ListItem<Asteroids*>* item = asteroids.start;
	for (int i = 0; i < numAsteroids; item = item->next, i++)
	{
		switch (item->data->asteroid->GetBodyState())
		{
		case BodyState::HIT:
			item->data->asteroid->SetBodyState(BodyState::DEADING);
			break;

		case BodyState::DEADING:
			item->data->asteroid->SetBodyType(BodyType::STATIC_BODY);
			item->data->asteroid->SetSleep(true);
			app->audio->PlayFx(-1,explosionFx);
			if (item->data->currentAnimAsteroid->HasFinished())
			{
				item->data->asteroid->SetBodyState(BodyState::DEAD);
				item->data->currentAnimAsteroid->Reset();
			}
			break;
		}
	}
}

// Called before quitting
bool Scene::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing scene");
	app->tex->UnLoad(imgBgSpace);
	app->tex->UnLoad(imgBgEarth);
	app->tex->UnLoad(imgPlatformMoon);
	app->tex->UnLoad(imgClouds);
	app->tex->UnLoad(imgAsteroids);
	app->tex->UnLoad(imgWater);
	app->tex->UnLoad(propulsionPlatform.texture);
	app->tex->UnLoad(propulsionPlatform.textureLaser);
	app->audio->Unload1Fx(explosionFx);

	delete idleAsteroidAnim;
	delete explosionAsteroidAnim;
	delete waterAnim;
	delete propulsionPlatform.laserBack;
	delete propulsionPlatform.laserFront;

	asteroids.Clear();
	app->player->CleanUp();
	app->physics->CleanUp();
	app->hud->CleanUp();

	active = false;

	return true;
}
