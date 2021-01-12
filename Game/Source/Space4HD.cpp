#include "Space4HD.h"
#define PI 3.14159265


void PhysicsEngine::CalculateAcceleration()
{
	ListItem<Body*>* item;

	for (item = bodies.start; item != NULL; item = item->next)
	{
		item->data->SetAcceleration({ item->data->GetForces().x / item->data->GetMass(), item->data->GetForces().y / item->data->GetMass() });
	}
}
void PhysicsEngine::CalculateAngularAcceleration()
{
	ListItem<Body*>* item;

	for (item = bodies.start; item != NULL; item = item->next)
	{
		float inercia = item->data->GetMass() * item->data->GetRadio() * item->data->GetRadio();
		item->data->SetAccelerationAngular(item->data->GetTorque() / inercia);
	}
	// item->data->GetRadio() es la distancia del CM al punto de aplicación de la fuerza
}

fPoint PhysicsEngine::ForceGrav(float mass, float hight)
{
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
void PhysicsEngine::CollisionFlatSurface(Body bodyA)
{
	fPoint bodyVelocity = bodyA.GetVelocity();
	float lostEnergy = 0.8;
	bodyA.SetVelocity({bodyVelocity.x * lostEnergy, -bodyVelocity.y * lostEnergy });
}
void PhysicsEngine::Collision(Body *bodyA, Body *bodyB)
{
	fPoint velBodyA = bodyA->GetVelocity();
	fPoint velBodyB = bodyB->GetVelocity();
	fPoint axisBodyA = { bodyA->GetAxis().x,bodyA->GetAxis().y };
	fPoint axisBodyB = { bodyB->GetAxis().x,bodyB->GetAxis().y };
	float masBodyA = bodyA->GetMass();
	float masBodyB = bodyB->GetMass();

	float jointMass = 2 * masBodyB / (masBodyA + masBodyB);
	fPoint subtractionVel = velBodyA - velBodyB;
	fPoint subtractionAxis = axisBodyA - axisBodyB;
	fPoint dotProduct = subtractionVel * subtractionAxis;
	float k = jointMass * (CalculateModule(dotProduct)) / pow(CalculateModule(subtractionAxis),2);

	fPoint newVelA = velBodyA - (subtractionAxis * k);

	jointMass = 2 * masBodyA / (masBodyA + masBodyB);
	subtractionVel = velBodyB - velBodyA;
	subtractionAxis = axisBodyB - axisBodyA;
	dotProduct = subtractionVel * subtractionAxis;
	k = jointMass * (CalculateModule(dotProduct)) / pow(CalculateModule(subtractionAxis),2);

	fPoint newVelB = velBodyB - (subtractionAxis * k);

	bodyA->SetVelocity(newVelA);
	bodyB->SetVelocity(newVelB);
}
float PhysicsEngine::CalculateModule(fPoint distance)
{
	return sqrt((distance.x * distance.x) + (distance.y * distance.y));
}
fPoint PhysicsEngine::NormalizeVector(fPoint distance)
{
	float module = CalculateModule(distance);
	if (module == 1 || module == 0) return distance;

	fPoint normalize;
	normalize.x = distance.x / module;
	normalize.y = distance.y / module;
	return normalize;
}

void PhysicsEngine::AddBody(Body* body)
{
	bodies.Add(body);
}

void PhysicsEngine::deleteBody(Body* body)
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
	ListItem<Body*>* item;

	for (item = bodies.start; item != NULL; item = item->next)
	{
		delete[] item->data->GetPointsCollision();
		delete[] item->data->GetPointsCollisionWorld();
	}

	return true;
}

void PhysicsEngine::Step(float dt)
{
	ListItem<Body*>* item;

	for (item = bodies.start; item != NULL; item = item->next)
	{
		if (item->data->GetType() == BodyType::DYNAMIC_BODY)
		{
			Body* p = item->data;
			fPoint fGrav = ForceGrav(p->GetMass(), p->GetPosition().y);
			item->data->AddForces(fGrav);
			if (CalculateModule(fGrav) == 0) p->SetVelocityFluid({0,0});
			float velRelative = CalculateModule(p->GetVelocity() - p->GetVelocityFluid());
			item->data->AddForces(ForceAeroDrag(p->GetVelocity(), p->GetDensityFluid(), velRelative, p->GetSurface(), p->GetCoeficientDrag()));
		}
	}

	CalculateAcceleration();
	CalculateAngularAcceleration();

	for (item = bodies.start; item !=NULL; item=item->next)
	{
		if (item->data->GetType() == BodyType::DYNAMIC_BODY)
		{
			VelocityVerletLinear(item->data, dt);
			VelocityVerletAngular(item->data, dt);
			item->data->SetAxisCM({ item->data->GetPosition().x + (item->data->GetWidth() / 2), item->data->GetPosition().y + (item->data->GetHight() / 2) });
			item->data->RotateBody();
		}
		item->data->ResetForces();
		item->data->ResetTorque();
	}
	ListItem<Body*>* item2;
	for (item = bodies.start; item != NULL; item = item->next)
	{
		for (item2 = item->next; item2 != NULL; item2 = item2->next)
		{
			if (IsInsidePolygons(item->data->GetPointsCollisionWorld(), item->data->GetNumPoints(), item2->data->GetPointsCollisionWorld(), item2->data->GetNumPoints()))
			{
				//if(superficie plana == true) CollisionFlatSurface(item->data);
				Collision(item->data,item2->data);
			}
		}
	}
}