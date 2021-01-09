#pragma once

#include "Module.h"

#include "Space4HD.h"

class Physics : public Module
{
public:

	Physics();
	~Physics();

	bool Start() override;

	bool PreUpdate() override;
	bool Update(float dt) override;
	bool PostUpdate() override;

	bool CleanUp() override;

	Body* CreateBody();
	void DestroyBody(Body* b);

private:

	List<Body*> bodies;

	PhysicsEngine* world;
};
