#include "Body.h"

void Body::AddForces(fPoint force)
{
	forces.x += force.x;
	forces.y += force.y;
}

// A force that generates a torque
void Body::AddTorque(fPoint force, fPoint distance)
{
	torque += (force.x * distance.y) - (forces.y * distance.x);
}
// A pure torque applied to the center of mass
void Body::AddTorque(float torqueCM)
{
	torque += torqueCM;
}

// Reset Forces and Torque
void Body::ResetForces()
{
	forces = { 0,0 };
}
void Body::ResetTorque()
{
	torque = 0;
}

// Save the array of collision points of the objects inside the engine so that it takes care of resolving collisions
void Body::SetCollisions(fPoint _bodyPointsCollision[], fPoint _bodyPointsCollisionWorld[], float _numPoints)
{
	numPoints = _numPoints;
	for (int i = 0; i < numPoints; i++)
	{
		*(bodyPointsCollision + i) = _bodyPointsCollision[i];
		*(bodyPointsCollisionWorld + i) = _bodyPointsCollisionWorld[i];
	}
}

// Rotate the array of collision points to adapt to the orientation of the body
void Body::RotateBody()
{
	for (int i = 0; i < numPoints; i++)
	{
		float posX = (bodyPointsCollision[i].x * cos(angularPosition)) - (bodyPointsCollision[i].y * sin(angularPosition));
		float posY = (bodyPointsCollision[i].x * sin(angularPosition)) + (bodyPointsCollision[i].y * cos(angularPosition));
		(bodyPointsCollisionWorld + i)->x = posX + METERS_TO_PIXELS(axisCM.x);
		(bodyPointsCollisionWorld + i)->y = posY + METERS_TO_PIXELS(axisCM.y);
	}
}