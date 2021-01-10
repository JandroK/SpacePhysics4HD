#include "p2Point.h"
#include "List.h"
#include "Collisions.h"
#include "Body.h"

class PhysicsEngine : public Collisions
{
public:
	PhysicsEngine() {};
	~PhysicsEngine() {};

	List<Body*> asteroids;
	List<Body*> missiles;
	List<Body*> bodies;

	void AddBody(Body* body);
	void deleteBody(Body* body);

	fPoint ForceGrav(float mass, float hight); // masa del objeto y distancia a la superfície del planeta
	fPoint ForceAeroDrag(fPoint dirVelocity, float density, float velRelative, float surface, float cd);
	//fPoint ForceAeroLift();
	//fPoint ForceHydroBuoy();
	//fPoint ForceHydroDrag();

	// Accelerations
	void CalculateAcceleration();
	void CalculateAngularAcceleration();

	void Step(float dt); //Step physics: apply current physics & integrate & solve collisions advance one frame
	//Remember to reset current forces/momentum of each body.

	void VelocityVerletLinear(Body* body, float dt);
	void VelocityVerletAngular(Body* body, float dt);
	void CollisionFlatSurface(Body bodyA);
	void Collision(Body* bodyA, Body* bodyB);

	float CalculateModule(fPoint distance);
	fPoint NormalizeVector(fPoint distance);

	// Geters
	float GetCurrentGravity() { return gravity; };

	// Seters
	void SetGravityPlanetA(float gravity) { gravityEarth = gravity; slopeEarth = -gravityEarth / rangeRadiusPlanetA; };
	void SetGravityPlanetB(float gravity) { gravityMoon = gravity; slopeMoon = -gravityMoon / rangeRadiusPlanetB; };
	void SetPositionAndRangePlanetA(float position, float radius) { positionPlanetA = position, rangeRadiusPlanetA = radius; };
	void SetPositionAndRangePlanetB(float position, float radius) { positionPlanetB = position, rangeRadiusPlanetB = radius; };

	bool CleanUp();

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
	float slopeEarth = 0;
	float slopeMoon = 0; 

};// world; // Instantiate PhysicsEngine as a global variable