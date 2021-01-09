#include "p2Point.h"
#include "List.h"
#include "Collisions.h"

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
	

	// Accelerations
	void CalculateAcceleration();
	void CalculateAngularAcceleration(float radio); //hacer un GetDistance() para saber a que distance se está aplicando  
													//la fuerza y así no tener que pasarle ningún valor a la función
	// Reset
	void ResetForces();
	void ResetTorque();

	// Geters
	fPoint GetPosition() { return position; };
	fPoint GetVelocity() { return velocity; };
	fPoint GetAcceleration() { return acceleration; };
	fPoint GetAxis() { return axisCM; };
	BodyType GetType() { return type; };
	float GetRotation() const { return  RADTODEG * angularPosition; };
	float GetMass() { return mass; };

	//Seters
	void SetCollisions(int _numPoints, fPoint _bodyPointsCollision[]);
	void SetAxisCM(fPoint axis) { axisCM = axis; };
	void SetVelocity(fPoint _velocity) { velocity = _velocity; };

private:

	//General properties
	float mass = 0;
	BodyType type = BodyType::STATIC_BODY;
	bool active = true;
	fPoint axisCM;

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
	fPoint *bodyPointsCollision = new fPoint [numPoints];
};


class Ship : public Body
{
public:
	float health;
	float fuel;
	int ammo; //munición

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

	fPoint ForceGrav(float mass, float hight); // masa del objeto y distancia a la superfície del planeta
	fPoint ForceAeroDrag(fPoint dirVelocity, float density, float velRelative, float surface, float cd);
	//fPoint ForceAeroLift();
	//fPoint ForceHydroBuoy();
	//fPoint ForceHydroDrag();

	void Step(float dt); //Step physics: apply current physics & integrate & solve collisions advance one frame
	//Remember to reset current forces/momentum of each body.

	void VelocityVerletLinear(fPoint& position, fPoint& velocity, fPoint acceleration, float dt);
	void VelocityVerletAngular(float& angularPosition, float& angularVelocity, float angularAcceleration, float dt);
	void CollisionFlatSurface(Body bodyA);
	void Collision(Body bodyA, Body bodyB);

	float CalculateModule(fPoint distance);
	fPoint NormalizeVector(fPoint distance);

	// Geters
	float GetCurrentGravity() { return gravity; };

	// Seters
	void SetGravityPlanetA(float gravity) { gravityEarth = gravity; };
	void SetGravityPlanetB(float gravity) { gravityMoon  = gravity; };
	void SetPositionAndRangePlanetA(float position, float radius) { positionPlanetA = position, rangeRadiusPlanetA = radius; };
	void SetPositionAndRangePlanetB(float position, float radius) { positionPlanetB = position, rangeRadiusPlanetB = radius; };

private:

	// Gravedades iniciales
	float gravityEarth = 10;
	float gravityMoon = 2;
	float gravity = 0;

	// Posiciones iniciales de la superfície de los planetas y el rango de alcance de su campo gravitatorio
	float positionPlanetA = 0;
	float positionPlanetB = 0;
	float rangeRadiusPlanetA = 0;
	float rangeRadiusPlanetB = 0;

	// Inclinación de la recta: gravedad en la superfície entre la altura máxima a la afecta la gravedad
	float slopeEarth = - gravityEarth / rangeRadiusPlanetA;
	float slopeMoon  = - gravityMoon  / rangeRadiusPlanetB;

} world; // Instantiate PhysicsEngine as a global variable