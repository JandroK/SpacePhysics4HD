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
	fPoint GetAcceleration() { return acceleration; };

	float& GetPositionAngular() { return angularPosition; };
	float& GetVelocityAngular() { return angularVelocity; };
	float& GetAccelerationAngular() { return angularAcceleration; };

	float GetMass() { return mass; };
	fPoint GetAxis() { return axisCM; };

	BodyType GetType() { return type; };
	bool GetActive() { return active; };
	bool GetIsFlat() { return isFlat; };

	int GetNumPoints() { return numPoints; };
	fPoint* GetPointsCollision() { return bodyPointsCollision; };
	float GetRadio() { return radio; };
	float GetSurface() { return surface; };
	float GetCoeficientDrag() { return cd; };
	float GetDensityFluid() { return densityFluid; };
	fPoint GetVelocityFluid() { return velocityFluid; };

	float GetRotation() const { return  RADTODEG * angularPosition; };
	fPoint GetForces() { return forces; };
	float GetTorque() { return torque; };

	//Seters
	void SetMass(float _mass) { mass = _mass; };
	void SetPosition(fPoint positionInitial) { position = positionInitial; };
	void SetVelocity(fPoint _velocity) { velocity = _velocity; };
	void SetAcceleration(fPoint _acceleration) { acceleration = _acceleration; };
	void SetAxisCM(fPoint axis) { axisCM = axis; };

	void SetPositionAngular(float angle) { angularPosition = angle; };
	void SetVelocityAngular(float velocity) { angularVelocity = velocity; };
	void SetAccelerationAngular(float _acceleration) { angularAcceleration = _acceleration; };

	void SetBodyType(BodyType _type) { type = _type; };
	void SetActive(bool _active) { active = _active; };
	void SetIsFlat(bool _flat) { isFlat = _flat; };

	void SetCollisions(int _numPoints, fPoint _bodyPointsCollision[]);
	void SetRadio(float _radio) { radio = _radio; };
	void SetSurface(float _surface) { surface = _surface; };
	void SetCoeficientDrag(float _cd) { cd = _cd; };
	void SetDensityFluid(float density) { densityFluid = density; };
	void SetVelocityFluid(fPoint velocity) { velocityFluid = velocity; };

private:

	//General properties
	float mass = 0;
	BodyType type = BodyType::DYNAMIC_BODY;
	bool active = true;
	fPoint axisCM;
	bool isFlat = false;
	float radio = 0;
	float densityFluid = 1.225f;
	fPoint velocityFluid = { 0,8 };

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