#include "Body.h"

void Body::AddForces(fPoint force)
{
	forces.x += force.x;
	forces.y += force.y;
}
void Body::AddTorque(fPoint force, fPoint distance)// una fuerza que genera un torque
{
	torque += (force.x * distance.y) - (forces.y * distance.x);
}
void Body::AddTorque(float torqueCM)// un torque puro aplicado al centro de masas
{
	torque += torqueCM;
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