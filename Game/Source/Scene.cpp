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
	// Create new Bodies
	platform = new Body;
	moon = new Body;

	// Load textures and music
	imgBgEarth = app->tex->Load("Assets/Textures/bg_earth.png");
	imgBgSpace = app->tex->Load("Assets/Textures/bg_space.png");
	imgClouds = app->tex->Load("Assets/Textures/clouds.png");
	imgAsteroids = app->tex->Load("Assets/Textures/asteroid.png");
	propulsionPlatform.texture= app->tex->Load("Assets/Textures/platform.png");
	propulsionPlatform.textureLaser= app->tex->Load("Assets/Textures/laser_platform.png");
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	SDL_QueryTexture(propulsionPlatform.texture, NULL, NULL, &rectPlatform.w, &rectPlatform.h);
	propulsionPlatform.position = { WINDOW_W / 2 - (rectPlatform.w >> 1), 10572 };

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

	// Set values of the bodies
	platform->SetBodyType(BodyType::STATIC_BODY);
	platform->SetMass(100000);
	platform->SetPosition({ PIXEL_TO_METERS(742), PIXEL_TO_METERS(10656) });
	float w = 440;
	float h = 150;
	platform->SetDimension(PIXEL_TO_METERS(w), PIXEL_TO_METERS(h));
	fPoint axis = { WINDOW_W / 2, METERS_TO_PIXELS(platform->GetPosition().y) + h / 2 };

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
	
	platform->SetCollisions(pointsCollision, pointsCollision, numPoints);
	platform->SetAxisCM({ PIXEL_TO_METERS(axis.x) , PIXEL_TO_METERS (axis.y)});

	moon->SetBodyType(BodyType::STATIC_BODY);
	moon->SetMass(100000);
	//moon->SetIsShpere(true);
	//moon->SetRadio(PIXEL_TO_METERS(1000));
	//moon->SetPosition({ 0, PIXEL_TO_METERS (-400) });
	//moon->SetAxisCM({ PIXEL_TO_METERS(WINDOW_W/2) , PIXEL_TO_METERS(-400) });
	moon->SetPosition({ PIXEL_TO_METERS(0), PIXEL_TO_METERS(0) });
	w = WINDOW_W;
	h = 414;
	moon->SetDimension(PIXEL_TO_METERS(w), PIXEL_TO_METERS(h));
	axis = { WINDOW_W / 2, METERS_TO_PIXELS(moon->GetPosition().y) + h / 2 };

	width = w / 2;
	hight = h / 2;

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

	moon->SetCollisions(pointsCollision, pointsCollision, numPoints);
	moon->SetAxisCM({ PIXEL_TO_METERS(axis.x) , PIXEL_TO_METERS(axis.y) });

	// Add bodies of the world
	app->physics->CreateBody(platform);
	app->physics->CreateBody(moon);
	CreateEntity();

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

	int x1, y1, x2, y2;
	for (int i = 0; i < numPoints; i++)
	{
		x1 = platform->GetPointsCollisionWorld()[i].x;
		y1 = platform->GetPointsCollisionWorld()[i].y;
		if (i == 3)i = -1;
		x2 = platform->GetPointsCollisionWorld()[i+1].x;
		y2 = platform->GetPointsCollisionWorld()[i+1].y;

		app->render->DrawLine(x1, y1, x2, y2, 255, 0, 0);
		if (i == -1)break;
	}
	for (int i = 0; i < numPoints; i++)
	{
		x1 = moon->GetPointsCollisionWorld()[i].x;
		y1 = moon->GetPointsCollisionWorld()[i].y;
		if (i == 3)i = -1;
		x2 = moon->GetPointsCollisionWorld()[i + 1].x;
		y2 = moon->GetPointsCollisionWorld()[i + 1].y;

		app->render->DrawLine(x1, y1, x2, y2, 255, 0, 0);
		if (i == -1)break;
	}

	//app->render->DrawCircle2(METERS_TO_PIXELS(moon->GetAxis().x), 
		//METERS_TO_PIXELS(moon->GetAxis().y), METERS_TO_PIXELS(moon->GetRadio()));
	
	ListItem<Body*>* item;
	// Draw all asteroids
	for (item = asteroids.start; item != NULL; item = item->next)
	{
		app->render->DrawCircle2(METERS_TO_PIXELS(item->data->GetAxis().x), 
			METERS_TO_PIXELS(item->data->GetAxis().y), METERS_TO_PIXELS(item->data->GetRadio()));

		app->render->DrawTexture(imgAsteroids, METERS_TO_PIXELS(item->data->GetAxis().x) - METERS_TO_PIXELS(item->data->GetRadio()),
			METERS_TO_PIXELS(item->data->GetAxis().y) - METERS_TO_PIXELS(item->data->GetRadio()), NULL, 1, item->data->GetRotation() );
	}
	return ret;
}

void Scene::CreateEntity()
{
	Body* b = new Body;
	asteroids.Add(b);

	ListItem<Body*>* item = asteroids.start;
	ListItem<Body*>* itemPrev;

	fPoint pos;
	fPoint posM;
	float distanceBetweenAxis;
	// Create as many bodies as	numAsteroids 
	for (int i = 0; i < numAsteroids; item = item->next, i++)
	{
		// Randomize the massa of bodies for everyone is different
		int m = rand() % 100 + 200;
		item->data->SetMass(m);
		item->data->SetIsShpere(true);
		item->data->SetRadio(PIXEL_TO_METERS(50));

		// Randomize position of spawn
		pos.x = rand() % WINDOW_W - 100 + 100;
		pos.y = rand() % 2600 + 2200;
		// But this position must be different to the rest asteroids
		for (itemPrev = item; itemPrev != NULL; itemPrev = itemPrev->prev)
		{
			posM = { itemPrev->data->GetAxis().x - PIXEL_TO_METERS(pos.x), itemPrev->data->GetAxis().y - PIXEL_TO_METERS(pos.y) };
			distanceBetweenAxis = app->physics->CalculateModule(posM);
			// If this asteroid is collision with other calculate again the position of the new asteroid
			if (itemPrev->data->GetRadio() + item->data->GetRadio() > distanceBetweenAxis)
			{
				pos.x = rand() % WINDOW_W - 100 + 100;
				pos.y = rand() % 2400 + 2200;
				itemPrev = item;
			}
			// In this way, never there will be asteroids in the same position and can buid a procedural level 
		}
		
		item->data->SetPosition({ PIXEL_TO_METERS(pos.x), PIXEL_TO_METERS(pos.y) });
		item->data->SetAxisCM(item->data->GetPosition());

		// The asteroids also move and collide with each other
		float velX = rand() % 2;
		float velY = rand() % 2;
		item->data->SetVelocity({ velX,velY });
		if (velX < 0) item->data->AddTorque(-60);
		if (velX > 0) item->data->AddTorque(60);
		item->data->SetCoeficientDrag(0.47);
		item->data->SetSurface(2);

		app->physics->CreateBody(item->data);
		Body* b = new Body;
		asteroids.Add(b);
	}
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	app->tex->UnLoad(imgBgSpace);
	app->tex->UnLoad(imgBgEarth);
	app->tex->UnLoad(imgClouds);
	app->tex->UnLoad(imgAsteroids);
	app->tex->UnLoad(propulsionPlatform.texture);
	app->tex->UnLoad(propulsionPlatform.textureLaser);

	delete idleAnim;
	delete propulsionPlatform.laserBack;
	delete propulsionPlatform.laserFront;

	delete platform;
	delete moon;
	asteroids.Clear();

	return true;
}
