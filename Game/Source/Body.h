#pragma once
#include "p2Point.h"
#define RADTODEG 57.29577958f

enum class BodyType
{
	STATIC_BODY = 0,
	KINEMATIC_BODY,
	DYNAMIC_BODY
};

class Body
{
public:

	// Forces
	void AddForces(fPoint force);
	void AddTorque(fPoint force, fPoint distance);
	void AddTorque(float torqueCM);

	// Rotations
	void RotateBody(fPoint pointsCollision[]);

	// Reset
	void ResetForces();
	void ResetTorque();

	// Geters
	fPoint GetPosition() { return position; };
	fPoint GetVelocity() { return velocity; };
	fPoint& GetAcceleration() { return acceleration; };

	float& GetPositionAngular() { return angularPosition; };
	float& GetVelocityAngular() { return angularVelocity; };
	float& GetAccelerationAngular() { return angularAcceleration; };

	fPoint GetForces() { return forces; };
	float GetTorque() { return torque; };
	fPoint GetAxis() { return axisCM; };
	BodyType GetType() { return type; };
	float GetRotation() const { return  RADTODEG * angularPosition; };
	float GetMass() { return mass; };
	float GetRadio() { return radio; };
	int GetNumPoints() { return numPoints; };
	fPoint* GetPointsCollision() { return bodyPointsCollision; };

	//Seters
	void SetMass(float _mass) { mass = _mass; };
	void SetRadio(float _radio) { radio = _radio; };
	void SetBodyType(BodyType _type) { type = _type; };
	void SetCollisions(int _numPoints, fPoint _bodyPointsCollision[]);
	void SetAxisCM(fPoint axis) { axisCM = axis; };
	void SetVelocity(fPoint _velocity) { velocity = _velocity; };

private:

	//General properties
	float mass = 0;
	BodyType type = BodyType::DYNAMIC_BODY;
	bool active = true;
	fPoint axisCM;
	bool isFlat = false;
	float radio = 0;

	// Properties linal
	fPoint position = { 0,0 };
	fPoint velocity = { 0,0 };
	fPoint acceleration = { 0,0 };

	// Properties angular
	float angularPosition = 0;
	float angularVelocity = 0;
	float angularAcceleration = 0;

	// Forces
	fPoint forces = { 0,0 };
	float torque = 0;

	// Properties for ForceDrag
	float surface = 0;
	float cd = 0;

	// Collisions
	int numPoints = 0;
	fPoint* bodyPointsCollision = new fPoint[numPoints];
};