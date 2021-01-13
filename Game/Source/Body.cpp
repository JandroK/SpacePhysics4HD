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

void Body::SetCollisions(fPoint _bodyPointsCollision[], fPoint _bodyPointsCollisionWorld[], float _numPoints)
{
	// Guardamos el array de puntos de collision de los objetos dentro del motor para que se encarge de resolver colisiones
	numPoints = _numPoints;
	for (int i = 0; i < numPoints; i++)
	{
		*(bodyPointsCollision + i) = _bodyPointsCollision[i];
		*(bodyPointsCollisionWorld + i) = _bodyPointsCollisionWorld[i];
	}
}

void Body::RotateBody()
{
	// Rotamos la matriz de puntos de collision para que se adapten a la orientación del cuerpo
	for (int i = 0; i < numPoints; i++)
	{
		float posX = (bodyPointsCollision[i].x * cos(angularPosition)) - (bodyPointsCollision[i].y * sin(angularPosition));//Matrix rotation
		float posY = (bodyPointsCollision[i].x * sin(angularPosition)) + (bodyPointsCollision[i].y * cos(angularPosition));
		(bodyPointsCollisionWorld + i)->x = posX + METERS_TO_PIXELS(axisCM.x);//+axisCM change base
		(bodyPointsCollisionWorld + i)->y = posY + METERS_TO_PIXELS(axisCM.y);
	}
}