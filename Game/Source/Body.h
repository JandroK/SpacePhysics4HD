#pragma once
#include "p2Point.h"

#define RADTODEG 57.29577958f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) PIXELS_PER_METER * m)
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

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
	void RotateBody();

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
	float GetWidth() { return width; };
	float GetHight() { return hight; };

	BodyType GetType() { return type; };
	bool GetSleep() { return sleep; };
	bool GetIsShpere() { return isShpere; };

	int GetNumPoints() { return numPoints; };
	fPoint* GetPointsCollision() { return bodyPointsCollision; };
	fPoint* GetPointsCollisionWorld() { return bodyPointsCollisionWorld; };
	float GetRadio() { return radio; };
	float GetSurface() { return surface; };
	float GetCoeficientDrag() { return cd; };
	float GetDensityFluid() { return densityFluid; };
	fPoint GetVelocityFluid() { return velocityFluid; };

	float GetRotation() const { return  RADTODEG * angularPosition; };
	fPoint GetForces() { return forces; };
	float GetTorque() { return torque; };

	// Seters
	void SetMass(float _mass) { mass = _mass; };
	void SetDimension(float _width, float _hight) { width = _width; hight = _hight; };
	void SetPosition(fPoint positionInitial) { position = positionInitial; };
	void SetVelocity(fPoint _velocity) { velocity = _velocity; };
	void SetAcceleration(fPoint _acceleration) { acceleration = _acceleration; };
	void SetAxisCM(fPoint axis) { axisCM = axis; };

	void SetPositionAngular(float angle) { angularPosition = angle; };
	void SetVelocityAngular(float velocity) { angularVelocity = velocity; };
	void SetAccelerationAngular(float _acceleration) { angularAcceleration = _acceleration; };

	void SetBodyType(BodyType _type) { type = _type; };
	void SetSleep(bool _sleep) { sleep = _sleep; };
	void SetIsShpere(bool _flat) { isShpere = _flat; };

	void SetCollisions(fPoint _bodyPointsCollision[], fPoint _bodyPointsCollisionWorld[], float _numPoints);
	void SetRadio(float _radio) { radio = _radio; };
	void SetSurface(float _surface) { surface = _surface; };
	void SetCoeficientDrag(float _cd) { cd = _cd; };
	void SetDensityFluid(float density) { densityFluid = density; };
	void SetVelocityFluid(fPoint velocity) { velocityFluid = velocity; };

private:

	// General properties
	float mass = 0;
	float width = 0;
	float hight = 0;
	BodyType type = BodyType::DYNAMIC_BODY;
	bool sleep = false;
	fPoint axisCM;
	bool isShpere = false;
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
	int numPoints = 10;
	fPoint* bodyPointsCollision = new fPoint[numPoints];
	fPoint* bodyPointsCollisionWorld = new fPoint[numPoints];
};