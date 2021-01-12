#include "Space4HD.h"
#define PI 3.14159265


void PhysicsEngine::CalculateAcceleration(Body* body)
{
	body->SetAcceleration({ body->GetForces().x / body->GetMass(),body->GetForces().y / body->GetMass() });
}
void PhysicsEngine::CalculateAngularAcceleration(Body* body)
{
	float inercia = body->GetMass() * body->GetRadio() * body->GetRadio();
	body->SetAccelerationAngular(body->GetTorque() / inercia);
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
void PhysicsEngine::CollisionFlatSurface(Body* bodyA)
{
	fPoint bodyVelocity = bodyA->GetVelocity();
	float lostEnergy = 1;
	bodyA->SetVelocity({bodyVelocity.x * lostEnergy, -bodyVelocity.y * lostEnergy });
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
	float dotProduct = subtractionVel.x * subtractionAxis.x + subtractionVel.y * subtractionAxis.y;
	float k = jointMass * dotProduct / pow(CalculateModule(subtractionAxis), 2);

	fPoint newVelA = velBodyA - (subtractionAxis * k);

	jointMass = 2 * masBodyA / (masBodyA + masBodyB);
	subtractionVel = velBodyB - velBodyA;
	subtractionAxis = axisBodyB - axisBodyA;
	dotProduct = subtractionVel.x * subtractionAxis.x + subtractionVel.y * subtractionAxis.y;
	k = jointMass * dotProduct / pow(CalculateModule(subtractionAxis), 2);

	fPoint newVelB = velBodyB - (subtractionAxis * k);

	if (bodyA->GetType() == BodyType::DYNAMIC_BODY)
	{
		bodyA->SetVelocity(newVelA);
		if (CalculateModule(newVelA) < 0.5)bodyA->SetSleep(true);
	}
	if (bodyB->GetType() == BodyType::DYNAMIC_BODY)
	{
		bodyB->SetVelocity(newVelB);
		if (CalculateModule(newVelB) < 0.5)bodyB->SetSleep(true);
	}
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
		if (item->data->GetType() == BodyType::DYNAMIC_BODY && !item->data->GetSleep())
		{
			Body* p = item->data;
			fPoint fGrav = ForceGrav(p->GetMass(), p->GetPosition().y);
			item->data->AddForces(fGrav);
			if (CalculateModule(fGrav) == 0) p->SetVelocityFluid({0,0});
			float velRelative = CalculateModule(p->GetVelocity() - p->GetVelocityFluid());
			item->data->AddForces(ForceAeroDrag(p->GetVelocity(), p->GetDensityFluid(), velRelative, p->GetSurface(), p->GetCoeficientDrag()));
		}
		if (CalculateModule(item->data->GetForces()) != 0)item->data->SetSleep(false);
	}

	for (item = bodies.start; item !=NULL; item=item->next)
	{
		if (item->data->GetType() == BodyType::DYNAMIC_BODY && !item->data->GetSleep())
		{
			CalculateAcceleration(item->data);
			CalculateAngularAcceleration(item->data);
			VelocityVerletLinear(item->data, dt);
			VelocityVerletAngular(item->data, dt);
			item->data->SetAxisCM({ item->data->GetPosition().x + (item->data->GetWidth() / 2), item->data->GetPosition().y + (item->data->GetHight() / 2) });
			item->data->RotateBody();
		}
		item->data->ResetForces();
		item->data->ResetTorque();
	}
	ListItem<Body*>* item2;
	bool ret = true;
	for (item = bodies.start; item != NULL; item = item->next)
	{
		while (item->data->GetSleep())
		{
			if (item->next != NULL)item = item->next;
			else
			{
				ret = false;
				break;
			}
		}
		if (ret == false) break;
		for (item2 = item->next; item2 != NULL; item2 = item2->next)
		{
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
			if (IsInsidePolygons(item->data->GetPointsCollisionWorld(), item->data->GetNumPoints(), 
				item2->data->GetPointsCollisionWorld(), item2->data->GetNumPoints()))
			{
				Collision(item->data,item2->data);
			}
		}
	}
}
/*fPoint velBodyA = bodyA->GetVelocity();
	fPoint velBodyB = bodyB->GetVelocity();
	fPoint axisBodyA = { bodyA->GetAxis().x,bodyA->GetAxis().y };
	fPoint axisBodyB = { bodyB->GetAxis().x,bodyB->GetAxis().y };
	float masBodyA = bodyA->GetMass();
	float masBodyB = bodyB->GetMass();

	float jointMass = 2 * masBodyB / (masBodyA + masBodyB);
	fPoint subtractionVel = velBodyA - velBodyB;
	fPoint subtractionAxis = axisBodyA - axisBodyB;
	fPoint dotProduct = { subtractionVel.x * subtractionAxis.x,subtractionVel.y * subtractionAxis.y };
	float k = jointMass * (dotProduct.x + dotProduct.y) / pow(CalculateModule(subtractionAxis),2);
	
	fPoint newVelA = velBodyA - (subtractionAxis * k);

	jointMass = 2 * masBodyA / (masBodyA + masBodyB);
	subtractionVel = velBodyB - velBodyA;
	subtractionAxis = axisBodyB - axisBodyA;
	dotProduct = { subtractionVel.x * subtractionAxis.x,subtractionVel.y * subtractionAxis.y };
	k = jointMass * (dotProduct.x + dotProduct.y) / pow(CalculateModule(subtractionAxis),2);

	fPoint newVelB = velBodyB - (subtractionAxis * k);

	if(bodyA->GetType() == BodyType::DYNAMIC_BODY)bodyA->SetVelocity(newVelA);
	if(bodyB->GetType() == BodyType::DYNAMIC_BODY)bodyB->SetVelocity(newVelB);*/


		//fPoint vecDir = bodyA->GetAxis() - bodyB->GetAxis();
		//float rad = atan2(vecDir.y, vecDir.x) ; // 1.5708 = 90*PI/180
		//float lostEnergy = 1;

		//if (bodyA->GetType() == BodyType::DYNAMIC_BODY)
		//{
		//	fPoint vBodyA = bodyA->GetVelocity();
		//	
		//	float vXa = vBodyA.x * cos(rad) * lostEnergy;
		//	float vYa = vBodyA.y * sin(rad) * -lostEnergy;

		//	float vXaRotate = vXa * cos(rad + PI * 2);
		//	float vYaRotate = vYa * sin(rad + PI * 2) ;

		//	bodyA->SetVelocity({ vXaRotate, vYaRotate });
		//}
		//
		//if (bodyB->GetType() == BodyType::DYNAMIC_BODY)
		//{
		//	fPoint vBodyB = bodyB->GetVelocity();

		//	float vXb = vBodyB.x * cos(rad) * lostEnergy;
		//	float vYb = vBodyB.y * sin(rad) * -lostEnergy;

		//	float vXbRotate = vXb * cos(rad + PI * 2);
		//	float vYbRotate = vYb * sin(rad + PI * 2);

		//	bodyB->SetVelocity({ vXbRotate, vYbRotate });
		//}