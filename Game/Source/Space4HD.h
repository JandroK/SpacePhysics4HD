#include "Point.h"
#include "List.h"
#include "Collisions.h"

#define GRAVITY 9.81f //LA GRAVEDAD SERÁ VARIABLE, ESTE DEFINE SE ELIMINARÁ

class Body
{
public:

	// Forces
	void AddForces(fPoint force);
	void AddTorque(fPoint force, fPoint distance);
	void AddTorque(float torqueCM);

	// Rotations
	void RotateBody(fPoint pointsCollision[], int numPoints, fPoint axis);
	float GetRotation() const;

	// Accelerations
	void CalculateAcceleration();
	void CalculateAngularAcceleration(float radio); //hacer un GetDistance() para saber a que distance se está aplicando  
													//la fuerza y así no tener que pasarle ningún valor a la función
	// Reset
	void ResetForces();
	void ResetTorque();

	// Properties
	fPoint position;
	fPoint velocity = { 0,0 };
	fPoint acceleration = { 0,0 };
	float angularPosition = 0;
	float angularVelocity = 0;
	float angularAcceleration = 0;
	fPoint forces = { 0,0 };
	float torque = 0;
	float mass;

	// Collisions
	int numPoints = 0;
	fPoint *bodyPointsCollision = new fPoint [numPoints];
};

class Ship : public Body
{
public:
	float health;
	float fuel;
	int ammo; //munición
	float surface; // surface for calculate Fdrag
	float cd; // cd for calculate Fdrag
	void LaunchMissiles();
};

class PhysicsEngine : public Collisions
{
public:
	PhysicsEngine() {};
	~PhysicsEngine() {};

	Ship ship;
	List<Body*> asteroids;
	List<Body*> missiles;

	fPoint ForceGrav(float mass);
	fPoint ForceAeroDrag(fPoint dirVelocity, float density, float velRelative, float surface, float Cd);
	//iPoint ForceAeroLift();
	//iPoint ForceHydroBuoy();
	//iPoint ForceHydroDrag();

	void Step(); //Step physics: apply current physics & integrate & solve collisions advance one frame
	//Remember to reset current forces/momentum of each body.

	void VelocityVerletLinear(fPoint& position, fPoint& velocity, fPoint acceleration, float dt);
	void VelocityVerletAngular(float& angularPosition, float& angularVelocity, float angularAcceleration, float dt);

	float CalculateModule(fPoint distance);
	fPoint NormalizeVector(fPoint distance);

} Space4HD; // Instantiate PhysicsEngine as a global variable


