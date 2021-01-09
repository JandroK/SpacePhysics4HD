#include "Space4HD.h"
#define PI 3.14159265

void Body::AddForces(fPoint force)
{
	forces.x += force.x;
	forces.y += force.y;
}
void Body::AddTorque(fPoint force, fPoint distance)// una fuerza que genera un torque
{
	torque += (force.x*distance.y)-(forces.y*distance.x);
}
void Body::AddTorque(float torqueCM)// un torque puro aplicado al centro de masas
{
	torque += torqueCM;
}
void Body::CalculateAcceleration()
{
	acceleration.x = forces.x / mass;
	acceleration.y = forces.y / mass;
}
void Body::CalculateAngularAcceleration(float radio)
{
	float inercia = mass * radio * radio;
	angularAcceleration = torque / inercia;
}
void Body::ResetForces()
{
	forces = { 0,0 };
}
void Body::ResetTorque()
{
	torque = 0;
}

void Body::SetCollisions(int _numPoints, fPoint _bodyPointsCollision[])
{
	numPoints = _numPoints;
	for (int i = 0; i < numPoints; i++)
	{
		bodyPointsCollision[i] = _bodyPointsCollision[i];
	}
}

void Body::RotateBody(fPoint pointsCollision[])
{
	for (int i = 0; i < numPoints; i++)
	{
		float posX = (bodyPointsCollision[i].x * cos(angularPosition)) - (bodyPointsCollision[i].y * sin(angularPosition));//Matrix rotation
		float posY = (bodyPointsCollision[i].x * sin(angularPosition)) + (bodyPointsCollision[i].y * cos(angularPosition));
		pointsCollision[i].x = posX + axisCM.x;//+axisCM change base
		pointsCollision[i].y = posY + axisCM.y;
	}
}

fPoint PhysicsEngine::ForceGrav(float mass, float hight)
{
	if (hight < positionPlanetA + rangeRadiusPlanetA)
		gravity = gravityEarth + abs(hight-positionPlanetA) * slopeEarth;
	else if (hight > positionPlanetB - rangeRadiusPlanetB)
		gravity = gravityMoon  + abs(hight-positionPlanetB) * slopeMoon;

	fPoint fg;
	fg.x = 0;
	fg.y = mass * gravity;

	return fg;
}

fPoint PhysicsEngine::ForceAeroDrag(fPoint dirVelocity, float density, float velRelative, float surface, float cd)
{
	float fdModule;
	fdModule = 0.5 * density * velRelative * velRelative * surface * cd;
	fPoint fd;
	fd.x = -fdModule * dirVelocity.x;
	fd.y = -fdModule * dirVelocity.y;
	return fd;
}

void PhysicsEngine::VelocityVerletLinear(fPoint& position, fPoint& velocity, fPoint acceleration, float dt)
{
	position.x += velocity.x * dt + 0.5 * acceleration.x * dt * dt;
	position.y += velocity.y * dt + 0.5 * acceleration.y * dt * dt;

	velocity.x += acceleration.x * dt;
	velocity.y += acceleration.y * dt;
}
void PhysicsEngine::VelocityVerletAngular(float& angularPosition, float& angularVelocity, float angularAcceleration, float dt)
{
	angularPosition += angularVelocity * dt + 0.5 * angularAcceleration * dt * dt;
	angularVelocity += angularAcceleration * dt;
}
void PhysicsEngine::CollisionFlatSurface(Body bodyA)
{
	fPoint bodyVelocity = bodyA.GetVelocity();
	float lostEnergy = 0.8;
	bodyA.SetVelocity({bodyVelocity.x * lostEnergy, -bodyVelocity.y * lostEnergy });
}
void PhysicsEngine::Collision(Body bodyA, Body bodyB)
{
	fPoint velBodyA = bodyA.GetVelocity();
	fPoint velBodyB = bodyB.GetVelocity();
	fPoint axisBodyA = bodyA.GetAxis();
	fPoint axisBodyB = bodyB.GetAxis();
	float masBodyA = bodyA.GetMass();
	float masBodyB = bodyB.GetMass();

	float jointMass = 2 * masBodyB / (masBodyA + masBodyB);
	fPoint subtractionVel = velBodyA - velBodyB;
	fPoint subtractionAxis = axisBodyA - axisBodyB;
	fPoint dotProduct = subtractionVel * subtractionAxis;
	float k = jointMass * (CalculateModule(dotProduct)) / pow(CalculateModule(subtractionAxis),2);

	fPoint newVelA = velBodyA - (subtractionAxis * k);

	float jointMass = 2 * masBodyA / (masBodyA + masBodyB);
	fPoint subtractionVel = velBodyB - velBodyA;
	fPoint subtractionAxis = axisBodyB - axisBodyA;
	fPoint dotProduct = subtractionVel * subtractionAxis;
	float k = jointMass * (CalculateModule(dotProduct)) / pow(CalculateModule(subtractionAxis),2);

	fPoint newVelB = velBodyB - (subtractionAxis * k);

	bodyA.SetVelocity(newVelA);
	bodyB.SetVelocity(newVelB);
}
float PhysicsEngine::CalculateModule(fPoint distance)
{
	return sqrt((distance.x * distance.x) + (distance.y * distance.y));
}
fPoint PhysicsEngine::NormalizeVector(fPoint distance)
{
	fPoint normalize;
	normalize.x = distance.x / CalculateModule(distance);
	normalize.y = distance.y / CalculateModule(distance);
	return normalize ;
}

void PhysicsEngine::Step(float dt)
{
	// VelocityVerletLinear();
	// VelocityVerletAngular();
	// IsInsidePolygons();
}