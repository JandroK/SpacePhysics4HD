#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include <iostream>
#include"p2Point.h"
#include "Body.h"

#define PI 3.14159265

class Collisions
{
public:
	// Functions for comprobate collisions

	// Given three colinear points p, q, r, the function checks if 
	// point q lies on line segment 'pr' 
	// Comprueba si el punto 'q' pertenece al segmento que forman 'pr'
	bool OnSegment(fPoint p, fPoint q, fPoint r);
	// To find orientation of ordered triplet (p, q, r). 
	// The function returns following values 
	// 0 --> p, q and r are colinear 
	// 1 --> Clockwise (right)
	// 2 --> Counterclockwise (left)
	// Para conocer si el punto respecto el segmento se encuentra a la derecha, a la izquirda o pertenece
	int Orientation(fPoint p, fPoint q, fPoint r);
	// The function that returns true if line segment 'p1q1' and 'p2q2' intersect. 
	// Devuelve true si los segmentos se intersectan
	bool DoIntersect(fPoint p1, fPoint q1, fPoint p2, fPoint q2);
	// Returns true if the point p lies inside the polygon[] with n vertices 
	// Devuelve true si el punto p está dentro del polygono
	bool IsInside(fPoint polygon[], int n, fPoint p);
	bool IsInsidePolygons(fPoint polygon[], int n, fPoint polygon2[], int n2);
	float CollisionSquare(Body* bodyA, Body* bodyB);

	// Functions for resolve collisions
	void Collision(Body* bodyA, Body* bodyB);
	void CollisionFlatSurfaceX(Body* bodyA);
	void CollisionFlatSurfaceY(Body* bodyA);
	void CollisionShpere(Body* bodyA, Body* bodyB);

	// Functions extra
	float CalculateModule(fPoint distance);
	fPoint NormalizeVector(fPoint distance);
	void Descliping(Body* body, fPoint vertex, fPoint* colliders);
};

#endif // __COLLISIONS_H__

