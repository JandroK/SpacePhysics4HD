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

	return true;
}

Body* Physics::CreateBody()
{
	Body* b = new Body;

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
