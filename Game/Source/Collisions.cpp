#include "Collisions.h"
#include <algorithm>

using namespace std;
#define INF 10000 

bool Collisions::OnSegment(fPoint p, fPoint q, fPoint r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;
	return false;
}

// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int Collisions::Orientation(fPoint p, fPoint q, fPoint r)
{
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear 
	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

// The function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
bool Collisions::DoIntersect(fPoint p1, fPoint q1, fPoint p2, fPoint q2)
{
	// Find the four orientations needed for general and 
	// special cases 
	int o1 = Orientation(p1, q1, p2);
	int o2 = Orientation(p1, q1, q2);
	int o3 = Orientation(p2, q2, p1);
	int o4 = Orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
	if (o1 == 0 && OnSegment(p1, p2, q1)) return true;

	// p1, q1 and p2 are colinear and q2 lies on segment p1q1 
	if (o2 == 0 && OnSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
	if (o3 == 0 && OnSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
	if (o4 == 0 && OnSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases 
}

// Returns true if the point p lies inside the polygon[] with n vertices 
bool Collisions::IsInside(fPoint polygon[], int n, fPoint p)
{
	// There must be at least 3 vertices in polygon[] 
	if (n < 3)  return false;

	// Create a point for line segment from p to infinite 
	fPoint extreme = { INF, p.y };

	// Count intersections of the above line with sides of polygon 
	int count = 0, i = 0;
	do
	{
		int next = (i + 1) % n;

		// Check if the line segment from 'p' to 'extreme' intersects 
		// with the line segment from 'polygon[i]' to 'polygon[next]' 
		if (DoIntersect(polygon[i], polygon[next], p, extreme))
		{
			// If the point 'p' is colinear with line segment 'i-next', 
			// then check if it lies on segment. If it lies, return true, 
			// otherwise false 
			if (Orientation(polygon[i], p, polygon[next]) == 0)
				return OnSegment(polygon[i], p, polygon[next]);

			count++;
		}
		i = next;
	} while (i != 0);

	// Return true if count is odd, false otherwise 
	return count & 1;  // Same as (count%2 == 1) 
}

// Returns true if the polygon1 is inside the polygon2 
bool Collisions::IsInsidePolygons(fPoint polygon[],int n, fPoint polygon2[], int n2)
{
	// There must be at least 3 vertices in polygon[] 
	if (n < 3 || n2 < 3)  return false;
	fPoint p;
	int count = 0;
	for ( int j = 0; j < n2; j++)
	{
		p = polygon2[j];
		// Create a point for line segment from p to infinite 
		fPoint extreme = { INF, p.y };

		// Count intersections of the above line with sides of polygon 
		int i = 0;
		do
		{
			int next = (i + 1) % n;

			// Check if the line segment from 'p' to 'extreme' intersects 
			// with the line segment from 'polygon[i]' to 'polygon[next]' 
			if (DoIntersect(polygon[i], polygon[next], p, extreme))
			{
				// If the point 'p' is colinear with line segment 'i-next', 
				// then check if it lies on segment. If it lies, return true, 
				// otherwise false 
				if (Orientation(polygon[i], p, polygon[next]) == 0)
					return OnSegment(polygon[i], p, polygon[next]);

				count++;
			}
			i = next;
		} while (i != 0);
		if (count % 2 == 1)
		{
			return true;
		}
		else count = 0;
	}
	// Return true if count is odd, false otherwise 
	return false;  // Same as (count%2 == 1) 
}

float Collisions::CollisionSquare(Body* bodyA, Body* bodyB)
{
	fPoint point;
	// Comprobe if some point of BodyB is inside of Body A
	for (int i = 0; i < bodyB->GetNumPoints(); i++)
	{
		point = bodyB->GetPointsCollisionWorld()[i];
		if (point.x > bodyA->GetPointsCollisionWorld()[0].x && point.x < bodyA->GetPointsCollisionWorld()[1].x
			&& point.y > bodyA->GetPointsCollisionWorld()[0].y && point.y < bodyA->GetPointsCollisionWorld()[3].y)
		{ 
			if (bodyA->GetClassType() == BodyClass::PLATFORMS)
			{
				// If the BodyA is a platform descliping the other body
				Descliping(bodyB, point, bodyA->GetPointsCollisionWorld());
				return 0;
			}
			else if (bodyA->GetClassType() == BodyClass::SEA)
			{
				// If a body collison with the sea calculate hight submerged of body
				float hSubmerged = point.y;
				for (int j = i+1; j < bodyB->GetNumPoints(); j++)
				{
					if (hSubmerged < bodyB->GetPointsCollisionWorld()[j].y)
							hSubmerged = bodyB->GetPointsCollisionWorld()[j].y;
				}
				hSubmerged = abs(hSubmerged- bodyA->GetPointsCollisionWorld()[0].y);
				hSubmerged = PIXEL_TO_METERS(hSubmerged);

				// If hight submerged is more bigger than the maximum height hight submerged = maximum height
				if (hSubmerged > bodyB->GetHight())hSubmerged = bodyB->GetHight();
				// Calculate volume
				float FUYM = 0.1;
				return hSubmerged * bodyB->GetSurface() * FUYM;
				// FUYM because we want a smaller volume so that it floats more, 
				// and we don't change the surface because the force drag need this surface
			}
		}
	}
	return 0;
}

void Collisions::Collision(Body* bodyA, Body* bodyB)
{
	bool sphere = false;
	if (bodyA->GetClassType()==BodyClass::ASTEROIDS && bodyB->GetClassType() == BodyClass::ASTEROIDS) 
		sphere = true;
	
	// Get velocities and axis of bodies
	fPoint velBodyA = bodyA->GetVelocity();
	fPoint velBodyB = bodyB->GetVelocity();
	fPoint axisBodyA = { bodyA->GetAxis().x,bodyA->GetAxis().y };
	fPoint axisBodyB = { bodyB->GetAxis().x,bodyB->GetAxis().y };
	float masBodyA = bodyA->GetMass();
	float masBodyB = bodyB->GetMass();

	float jointMass = 2 * masBodyB / (masBodyA + masBodyB);
	fPoint subtractionVel = velBodyA - velBodyB;
	fPoint subtractionAxis = axisBodyA - axisBodyB;
	float dotProduct = subtractionVel.x * subtractionAxis.x + subtractionVel.y * subtractionAxis.y;
	float k = jointMass * dotProduct / pow(CalculateModule(subtractionAxis), 2);

	// Calculate the new velocity of Body A
	fPoint newVelA = velBodyA - (subtractionAxis * k);

	jointMass = 2 * masBodyA / (masBodyA + masBodyB);
	subtractionVel = velBodyB - velBodyA;
	subtractionAxis = axisBodyB - axisBodyA;
	dotProduct = subtractionVel.x * subtractionAxis.x + subtractionVel.y * subtractionAxis.y;
	k = jointMass * dotProduct / pow(CalculateModule(subtractionAxis), 2);

	// Calculate the new velocity of Body B
	fPoint newVelB = velBodyB - (subtractionAxis * k);

	// Only if body is DYNAMIC_BODY the object will be move
	if (bodyA->GetType() == BodyType::DYNAMIC_BODY)
	{
		bodyA->SetVelocity(newVelA);

		// If newVelocity is smaller than 0.2 the body sleep=true  
		float velCriticA = CalculateModule(newVelA);
		if (velCriticA < 0.2)
		{
			if (bodyA->GetClassType() != BodyClass::ASTEROIDS)bodyA->SetSleep(true);
			bodyA->SetVelocity({ 0,0 });
			bodyA->SetAcceleration({ 0,0 });
		}

		// Change the satate of Body
		if(sphere)bodyA->SetBodyState(BodyState::HIT);
		if (bodyA->GetClassType() == BodyClass::PLAYER) bodyA->SetBodyState(BodyState::HIT);
		if (velCriticA > 20) bodyA->SetBodyState(BodyState::DEADING);
	}
	if (bodyB->GetType() == BodyType::DYNAMIC_BODY)
	{
		bodyB->SetVelocity(newVelB);

		// If newVelocity is smaller than 0.2 the body sleep=true 
		float velCriticB = CalculateModule(newVelB);
		if (velCriticB < 0.2)// || velCriticB > 30)
		{
			if (bodyB->GetClassType() != BodyClass::ASTEROIDS)bodyB->SetSleep(true);
			bodyB->SetVelocity({ 0,0 });
			bodyB->SetAcceleration({ 0,0 });
		}

		// Change the satate of Body
		if (sphere) bodyB->SetBodyState(BodyState::HIT);
		if(bodyB->GetClassType()==BodyClass::PLAYER) bodyB->SetBodyState(BodyState::HIT);
		if (velCriticB > 20) bodyB->SetBodyState(BodyState::DEADING);
	}
}

// This function isn't used in the code but we didn't want to just use the black box
// Thereforce we have also calculated the collisions with angles
void Collisions::CollisionShpere(Body* bodyA, Body* bodyB)
{
	// Calculate the direction of the collision between axis
	fPoint vecDir = bodyA->GetAxis() - bodyB->GetAxis();
	// Calculate the angle in radians about the axes of the world of the collision
	float rad = atan2(vecDir.y, vecDir.x); 
	float lostEnergy = 0.9;

	// Only if body is DYNAMIC_BODY the object will be move
	if (bodyA->GetType() == BodyType::DYNAMIC_BODY)
	{
		fPoint vBodyA = bodyA->GetVelocity();

		// Rotate vector velocity for simulate the collision against a flat surface
		float vXa = vBodyA.x * cos(rad) * lostEnergy;
		float vYa = vBodyA.y * sin(rad) * -lostEnergy;

		// And roatet again but in the opposite direction 
		float vXaRotate = vXa * cos(rad + PI); // PI = 180º
		float vYaRotate = vYa * sin(rad + PI);

		bodyA->SetVelocity({ vXaRotate, vYaRotate });
		float velCriticA = CalculateModule({ vXaRotate, vYaRotate });
		if (velCriticA < 0.2 )
		{
			if (bodyA->GetClassType() != BodyClass::ASTEROIDS)bodyA->SetSleep(true);
			bodyA->SetAcceleration({ 0,0 });
			bodyA->SetAccelerationAngular(0);
		}
	}

	if (bodyB->GetType() == BodyType::DYNAMIC_BODY)
	{
		fPoint vBodyB = bodyB->GetVelocity();

		float vXb = vBodyB.x * cos(rad) * lostEnergy;
		float vYb = vBodyB.y * sin(rad) * -lostEnergy;

		float vXbRotate = vXb * cos(rad + PI * 2);
		float vYbRotate = vYb * sin(rad + PI * 2);

		bodyB->SetVelocity({ vXbRotate, vYbRotate });
		float velCriticB = CalculateModule({ vXbRotate, vYbRotate });
		if (velCriticB < 0.2)
		{
			if(bodyB->GetClassType() != BodyClass::ASTEROIDS)bodyB->SetSleep(true);
			bodyB->SetAcceleration({ 0,0 });
			bodyB->SetAccelerationAngular(0);
		}
	}
}

// Functions for resolve shocks
void Collisions::CollisionFlatSurfaceX(Body* body)
{
	// Flip the direction of axis X and reduce vector lenght 
	fPoint bodyVelocity = body->GetVelocity();
	float lostEnergy = 1;
	body->SetVelocity({ bodyVelocity.x * -lostEnergy, bodyVelocity.y * lostEnergy });
}
void Collisions::CollisionFlatSurfaceY(Body* body)
{
	// Flip the direction of axis Y and reduce vector lenght 
	fPoint bodyVelocity = body->GetVelocity();
	float lostEnergy = 0.6;
	body->SetVelocity({ bodyVelocity.x * lostEnergy, bodyVelocity.y * -lostEnergy });
	float velCritic = CalculateModule(body->GetVelocity());
	if (velCritic < 0.2 )
	{
		body->SetAcceleration({ 0,0 });
		body->SetAccelerationAngular(0);
		if (body->GetClassType() != BodyClass::ASTEROIDS)body->SetSleep(true);
	}
	if (velCritic > 20) body->SetBodyState(BodyState::DEADING);
}

float Collisions::CalculateModule(fPoint distance)
{
	return sqrt((distance.x * distance.x) + (distance.y * distance.y));
}
fPoint Collisions::NormalizeVector(fPoint distance)
{
	float module = CalculateModule(distance);
	if (module == 1 || module == 0) return distance;

	fPoint normalize;
	normalize.x = distance.x / module;
	normalize.y = distance.y / module;
	return normalize;
}

void Collisions::Descliping(Body* body, fPoint vertex, fPoint* colliders)
{
	float xRight = abs(vertex.x - colliders[0].x);	// Surface wall right
	float xLeft = abs(vertex.x - colliders[1].x);	// Surface wall left
	float yUp = abs(vertex.y - colliders[0].y);		// Surface floor
	float yDown = abs(vertex.y - colliders[3].y);	// Surface ceiling

	float distance = xRight;
	if (distance > xLeft)distance = xLeft;
	if (distance > yUp)distance = yUp;
	if (distance > yDown)distance = yDown;

	if (distance == yUp)
	{
		body->SetAxisCM({ body->GetAxis().x, body->GetAxis().y - PIXEL_TO_METERS(yUp) - 1 / 25 });
		body->RotateBody();
		CollisionFlatSurfaceY(body);
	}
	else if (distance == yDown)
	{
		body->SetAxisCM({ body->GetAxis().x, body->GetAxis().y + PIXEL_TO_METERS(yDown) + 1 / 25 });
		body->RotateBody();
		CollisionFlatSurfaceY(body);
	}
	else if (distance == xLeft)
	{
		body->SetAxisCM({ body->GetAxis().x + PIXEL_TO_METERS(xLeft) + 1 / 25, body->GetAxis().y });
		body->RotateBody();
		CollisionFlatSurfaceX(body);
	}
	else if (distance == xRight)
	{
		body->SetAxisCM({ body->GetAxis().x - PIXEL_TO_METERS(xRight) - 1 / 25, body->GetAxis().y });
		body->RotateBody();
		CollisionFlatSurfaceX(body);
	}
}
