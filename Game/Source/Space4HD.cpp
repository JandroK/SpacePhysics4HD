#include "Space4HD.h"

// Second law of Newtow F = m*a
void PhysicsEngine::CalculateAcceleration(Body* body)
{
	body->SetAcceleration({ body->GetForces().x / body->GetMass(),body->GetForces().y / body->GetMass() });
}
void PhysicsEngine::CalculateAngularAcceleration(Body* body)
{
	float inercia = body->GetMass() * body->GetRadio() * body->GetRadio();
	body->SetAccelerationAngular(body->GetTorque() / inercia);
	// item->data->GetRadio() returns the distance from the CM to the point of application of the force
}

fPoint PhysicsEngine::ForceGrav(float mass, float hight)
{
	// If hight of body is within the gravitational field calculate the garvity
	if (hight > positionPlanetA - rangeRadiusPlanetA)
		gravity = gravityEarth + abs(hight - positionPlanetA) * slopeEarth;
	else if (hight < positionPlanetB + rangeRadiusPlanetB)
		gravity = -gravityMoon + abs(hight - positionPlanetB) * slopeMoon;
	else return { 0,0 };

	fPoint fg;
	fg.x = 0;
	fg.y = mass * gravity;

	return fg;
}

fPoint PhysicsEngine::ForceAeroDrag(fPoint dirVelocity, float density, float velRelative, float surface, float cd)
{
	dirVelocity = NormalizeVector(dirVelocity);
	float fdModule;
	fdModule = 0.5 * density * velRelative * velRelative * surface * cd;
	fPoint fd;
	fd.x = -fdModule * dirVelocity.x;
	fd.y = -fdModule * dirVelocity.y;
	return fd;
}

fPoint PhysicsEngine::ForceHydroBuoy(Body* body, float volume)
{
	fPoint forceBuoyance;
	forceBuoyance.x = 0;
	forceBuoyance.y = 1 * abs(body->GetGravity()) * volume;
	forceBuoyance.y *= -1;
	return forceBuoyance;
}
fPoint PhysicsEngine::ForceHydroDrag(fPoint velBody, fPoint velSea)
{
	fPoint forceDrag;
	fPoint dirVelocity = NormalizeVector(velBody);
	float velRelative = CalculateModule(velBody - velSea);
	float forceDragModule = 0.5 * velRelative;
	forceDrag.x = -forceDragModule * dirVelocity.x;
	forceDrag.y = -forceDragModule * dirVelocity.y;
	return fPoint();
}

// Integrator
void PhysicsEngine::VelocityVerletLinear(Body* body, float dt)
{
	float posX = body->GetPosition().x + body->GetVelocity().x * dt + 0.5 * body->GetAcceleration().x * dt * dt;
	float posY = body->GetPosition().y + body->GetVelocity().y * dt + 0.5 * body->GetAcceleration().y * dt * dt;
	body->SetPosition({posX,posY});

	float velX = body->GetVelocity().x + body->GetAcceleration().x * dt;
	float velY = body->GetVelocity().y + body->GetAcceleration().y * dt;
	body->SetVelocity({velX,velY});
}
void PhysicsEngine::VelocityVerletAngular(Body* body, float dt)
{
	body->SetPositionAngular(body->GetPositionAngular()+body->GetVelocityAngular()*dt + 0.5* body->GetAccelerationAngular()*dt*dt);
	body->SetVelocityAngular(body->GetVelocityAngular()+body->GetAccelerationAngular()*dt);
}

// Resolve physics
void PhysicsEngine::Step(float dt)
{
	ListItem<Body*>* item;
	
	ApplyForcesWorld(item);
	Integrator(item, dt);
	ComprobeCollisions(item);
	ComprobeState(item);
}

void PhysicsEngine::ApplyForcesWorld(ListItem<Body*>*& item)
{
	// Only apply forces to dynamic objects and if they are awake
	for (item = bodies.start; item != NULL; item = item->next)
	{
		if (item->data->GetType() == BodyType::DYNAMIC_BODY && !item->data->GetSleep())
		{
			Body* p = item->data;
			fPoint fGrav = ForceGrav(p->GetMass(), p->GetPosition().y);
			item->data->AddForces(fGrav);
			item->data->SetGravity(fGrav.y);

			// If there isn't gravity neither there is wind, but if body get in the surface of the earth the wind is restored
			if (fGrav.y > 0) p->SetVelocityFluid({ 0,8 });
			else p->SetVelocityFluid({ 0,0 });

			float velRelative = CalculateModule(p->GetVelocity() - p->GetVelocityFluid());
			item->data->AddForces(ForceAeroDrag(p->GetVelocity(), p->GetDensityFluid(), velRelative, p->GetSurface(), p->GetCoeficientDrag()));
			
			// Add ineria to asteroids
			if (item->data->GetVelocity().x < 0 && item->data->GetClassType() == BodyClass::ASTEROIDS) item->data->AddTorque(-10);
			else if (item->data->GetVelocity().x > 0 && item->data->GetClassType() == BodyClass::ASTEROIDS) item->data->AddTorque(10);
		}
		// If body receives an external force thge body is awake
		if (CalculateModule(item->data->GetForces()) != 0)item->data->SetSleep(false);
		if (item->data->GetTorque() != 0)item->data->SetSleep(false);
	}
}

void PhysicsEngine::Integrator(ListItem<Body*>*& item, float dt)
{
	for (item = bodies.start; item != NULL; item = item->next)
	{
		if (item->data->GetType() == BodyType::DYNAMIC_BODY && !item->data->GetSleep())
		{
			CalculateAcceleration(item->data);
			CalculateAngularAcceleration(item->data);
			VelocityVerletLinear(item->data, dt);
			VelocityVerletAngular(item->data, dt);

			// If body isn't circular, update his axis 
			if (item->data->GetClassType() != BodyClass::ASTEROIDS)
			{
				item->data->SetAxisCM({ item->data->GetPosition().x + (item->data->GetWidth() / 2),
					item->data->GetPosition().y + (item->data->GetHight() / 2) });
			}
			else item->data->SetAxisCM(item->data->GetPosition()); // Else the axis follow de position
			item->data->RotateBody();
		}
		item->data->ResetForces();
		item->data->ResetTorque();
	}
}

void PhysicsEngine::ComprobeCollisions(ListItem<Body*>*& item)
{
	ListItem<Body*>* item2;
	bool ret = true;
	for (item = bodies.start; item != NULL; item = item->next)
	{
		// Find a body that is awake
		while (item->data->GetSleep())
		{
			if (item->next != NULL)item = item->next;
			else
			{
				ret = false;
				break;
			}
		}
		if (ret == false) break; // If all bodies are sleep the collisions aren't checked
		for (item2 = item->next; item2 != NULL; item2 = item2->next)
		{
			// Find a body that is awake starting since the next body
			while (item2->data->GetSleep())
			{
				if (item2->next != NULL)item2 = item2->next;
				else
				{
					ret = false;
					break;
				}
			}
			if (ret == false) break;
			float distanceBetweenAxis = CalculateModule(item->data->GetAxis() - item2->data->GetAxis());
			bool sphere = false;
			bool staticBody = false;

			// If one of thw two is circular the collisions are checked using their radius
			// Else the collisions are checked using their matrix of points
			if (item->data->GetClassType()==BodyClass::ASTEROIDS || item2->data->GetClassType() == BodyClass::ASTEROIDS) sphere = true;
			
			// No ckeck collison if two bodies are static Body
			if (item->data->GetType() == BodyType::STATIC_BODY && item2->data->GetType() == BodyType::STATIC_BODY) staticBody = true;
			if (item->data->GetRadio() + item2->data->GetRadio() > distanceBetweenAxis && sphere)
			{
				Collision(item->data, item2->data);
			}
			else if (!sphere && !staticBody)
			{
				float volume = 0;
				volume = CollisionSquare(item->data, item2->data);
				if (volume != 0)
				{
					item2->data->AddForces(ForceHydroBuoy(item2->data, volume));
					item2->data->AddForces({ 300,0 }); //Force wave to right
				}
					
			}
		}
	}
}

void PhysicsEngine::ComprobeState(ListItem<Body*>*& item)
{
	for (item = bodies.start; item != NULL; item = item->next)
	{
		switch (item->data->GetBodyState())
		{
		case BodyState::HIT:
			item->data->SetLives(item->data->GetLives() - 1);
			if (item->data->GetLives() <= 0) item->data->SetBodyState(BodyState::DEADING);

			break;
		case BodyState::DEAD:
			item->data->SetPendingToDelete(true);
		default:
			break;
		}
		if (item->data->GetPendingToDelete()) DeleteBody(item->data);
	}
}

void PhysicsEngine::AddBody(Body* body)
{
	bodies.Add(body);
}

void PhysicsEngine::DeleteBody(Body* body)
{
	ListItem<Body*>* item;

	for (item = bodies.start; item != NULL; item = item->next)
	{
		if (item->data == body)
		{
			bodies.Del(item);
		}
	}
}

bool PhysicsEngine::CleanUp()
{
	bodies.Clear();

	return true;
}