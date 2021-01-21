#include "List.h"
#include "Collisions.h"

class PhysicsEngine : public Collisions
{
public:
	PhysicsEngine() {};
	~PhysicsEngine() {};

	List<Body*> bodies;

	void AddBody(Body* body);
	void DeleteBody(Body* body);
	bool CleanUp();

	//Forces implmented + manual forces/torques
	fPoint ForceGrav(float mass, float hight); // Mass of the object and distance to the surface of the planet
	fPoint ForceAeroDrag(fPoint dirVelocity, float density, float velRelative, float surface, float cd);
	fPoint ForceHydroBuoy(Body* body, float volume);
	fPoint ForceHydroDrag(fPoint velBody, fPoint velSea);

	// Accelerations
	void CalculateAcceleration(Body* body);
	void CalculateAngularAcceleration(Body* body);

	void Step(float dt); //Apply current physics & integrate & solve collisions advance one frame
	void ApplyForcesWorld(ListItem<Body*>*& item);
	void Integrator(ListItem<Body*>*& item, float dt);
	void ComprobeCollisions(ListItem<Body*>*& item);
	void ComprobeState(ListItem<Body*>*& item);

	// Integrator
	void VelocityVerletLinear(Body* body, float dt);
	void VelocityVerletAngular(Body* body, float dt);

	// Geters
	float GetCurrentGravity() { return gravity; };

	// Seters
	void SetPositionAndRangePlanetA(float position, float radius) { positionPlanetA = position, rangeRadiusPlanetA = radius; };
	void SetPositionAndRangePlanetB(float position, float radius) { positionPlanetB = position, rangeRadiusPlanetB = radius; };
	void SetGravityPlanetA(float gravity) { gravityEarth = gravity; slopeEarth = -gravityEarth / rangeRadiusPlanetA; };
	void SetGravityPlanetB(float gravity) { gravityMoon = gravity; slopeMoon = -gravityMoon / rangeRadiusPlanetB; };

private:

	// Initial gravity
	float gravityEarth = 10;
	float gravityMoon = 2;
	float gravity = 0;

	// Initial positions of the surface of the planets and the range of their gravitational field
	float positionPlanetA = 0;
	float positionPlanetB = 0;
	float rangeRadiusPlanetA = 0;
	float rangeRadiusPlanetB = 0;

	// Inclination of the line: gravity on the surface between the maximum height affected by gravity
	float slopeEarth = 0;
	float slopeMoon = 0; 

};// world; // Instantiate PhysicsEngine as a global variable