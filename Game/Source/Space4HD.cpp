#include "Space4HD.h"
#define PI 3.14159265
#define RADTODEG 57.29577958f

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

void Body::RotateBody(fPoint pointsCollision[], int numPoints, fPoint axis)
{
	for (int i = 0; i < numPoints; i++)
	{
		float posX = (bodyPointsCollision[i].x * cos(angularPosition)) - (bodyPointsCollision[i].y * sin(angularPosition));//Matrix rotation
		float posY = (bodyPointsCollision[i].x * sin(angularPosition)) + (bodyPointsCollision[i].y * cos(angularPosition));
		pointsCollision[i].x = posX + axis.x;//+axis change base
		pointsCollision[i].y = posY + axis.y;
	}
}
float Body::GetRotation() const
{
	return  RADTODEG*angularPosition;
}

fPoint PhysicsEngine::ForceGrav(float mass)
{
	fPoint fg;
	fg.x = 0;
	fg.y = mass * GRAVITY;
	return fg;
}

fPoint PhysicsEngine::ForceAeroDrag(fPoint dirVelocity, float density, float velRelative, float surface, float Cd)
{
	float fdModule;
	fdModule = 0.5 * density * velRelative * velRelative * surface * Cd;
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
