#include "Physics.h"
#include "Defs.h"

Physics::Physics() : Module()
{
}

Physics::~Physics()
{
}

bool Physics::Start()
{
	active = true;
	// Definimos un nuevo mundo
	world = new PhysicsEngine();
	world->SetPositionAndRangePlanetA(PIXEL_TO_METERS(11081), PIXEL_TO_METERS(5500));
	world->SetPositionAndRangePlanetB(PIXEL_TO_METERS(0), PIXEL_TO_METERS(2000));
	world->SetGravityPlanetA(9);
	world->SetGravityPlanetB(2);

	return true;
}

bool Physics::PreUpdate()
{
	return true;
}

bool Physics::Update(float dt)
{
	world->Step(dt);

	return true;
}

bool Physics::PostUpdate()
{
	return true;
}

bool Physics::CleanUp()
{
	if (active)
	{
		bodies.Clear();
		world->CleanUp();
		delete world;
	}
	
	active = false;
	return true;
}

Body* Physics::CreateBody(Body* b)
{
	// Add the body to the list in module physics
	bodies.Add(b);

	// And also to the world
	world->AddBody(b);

	return b;
}

void Physics::DestroyBody(Body* b)
{
	ListItem<Body*>* item = bodies.start;

	while (item != nullptr)
	{
		if (item->data == b)
			bodies.Del(item);

		item = item->next;
	}
}

float Physics::CalculateModule(fPoint distance)
{
	return sqrt((distance.x * distance.x) + (distance.y * distance.y));
}

fPoint Physics::NormalizeVector(fPoint distance)
{
	float module = CalculateModule(distance);
	if (module == 1 || module == 0) return distance;

	fPoint normalize;
	normalize.x = distance.x / module;
	normalize.y = distance.y / module;
	return normalize;
}

fPoint Physics::RotateVector(fPoint vector, fPoint axis, float angle)
{
	vector = { vector.x - METERS_TO_PIXELS(axis.x), vector.y - METERS_TO_PIXELS(axis.y) };
	float posX = (vector.x * cos(angle)) - (vector.y * sin(angle)) + METERS_TO_PIXELS(axis.x);//Matrix rotation
	float posY = (vector.x * sin(angle)) + (vector.y * cos(angle)) + METERS_TO_PIXELS(axis.y);//Change basis
	return fPoint({ posX, posY });
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           