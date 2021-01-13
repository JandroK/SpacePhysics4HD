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
	world = new PhysicsEngine();
	world->SetPositionAndRangePlanetA(PIXEL_TO_METERS(11081), PIXEL_TO_METERS(5650));
	world->SetPositionAndRangePlanetB(PIXEL_TO_METERS(0), PIXEL_TO_METERS(2300));
	world->SetGravityPlanetA(10);
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
	bodies.Clear();
	world->CleanUp();

	return true;
}

Body* Physics::CreateBody(Body* b)
{
	//Body* b = new Body;

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
