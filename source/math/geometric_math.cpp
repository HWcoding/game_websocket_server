
#include "source/math/geometric_math.h"
#include <cstdint>
#include <math.h>
#include <string.h>

double fastInvSqrt(double input)
{
	int64_t bits;
	memcpy(&bits, &input, sizeof(int64_t));
	bits= 6910238826744275866LL - (bits>>1);
	double halfInput = 0.5*input;
	memcpy(&input, &bits, sizeof(int64_t));
	input *= 1.5 - halfInput*input*input;
	return input;
}



namespace v_math{


Point3D fastNormalize(const Point3D &p)
{
	Point3D output(p.x, p.y, p.z);

	double scalar = p.x*p.x+p.y*p.y+p.z*p.z;
	scalar = fastInvSqrt(scalar);
	output.x *= scalar;
	output.y *= scalar;
	output.z *= scalar;

	return output;
}

Point3D normalize(const Point3D &p)
{
	Point3D output(p.x, p.y, p.z);

	double scalar = p.x*p.x+p.y*p.y+p.z*p.z;
	scalar = 1/sqrt(scalar);
	output.x *= scalar;
	output.y *= scalar;
	output.z *= scalar;

	return output;
}

Point3D scale(const Point3D &p, double scalar)
{
	Point3D output(p.x, p.y, p.z);
	output.x *= scalar;
	output.y *= scalar;
	output.z *= scalar;
	return output;
}

Point3D add(const Point3D &p1, const Point3D &p2)
{
	Point3D output(p1.x, p1.y, p1.z);
	output.x += p2.x;
	output.y += p2.y;
	output.z += p2.z;
	return output;
}

Point3D subtract(const Point3D &p1, const Point3D &p2)
{
	Point3D output(p1.x, p1.y, p1.z);
	output.x -= p2.x;
	output.y -= p2.y;
	output.z -= p2.z;
	return output;
}

double dotProduct(const Point3D &p1, const Point3D &p2)
{
	return p1.x*p2.x+p1.y*p2.y+p1.z*p2.z;
}

Point3D crossProduct(const Point3D &p1, const Point3D &p2)
{
	Point3D output;
	output.x = p1.y*p2.z - p1.z*p2.y;
	output.y = p1.z*p2.x - p1.x*p2.z;
	output.z = p1.x*p2.y - p1.y*p2.x;
	return output;
}

double squareDistance(const Point3D &p1, const Point3D &p2)
{
	double x = p1.x-p2.x;
	double y = p1.y-p2.y;
	double z = p1.z-p2.z;
	return x*x+y*y+z*z;
}

double distance(const Point3D &p1, const Point3D &p2)
{
	return sqrt(squareDistance(p1,p2));
}


} // namespace v_math

//#include <iostream>

namespace collision {

bool doSpheresCollide(Sphere &a, Sphere &b)
{
	double distanceSQ = v_math::squareDistance(a.center, b.center);
	double radDist = a.radius+b.radius;
	return distanceSQ <= radDist*radDist;
}

bool doesSphereCollideWithPlane(Sphere &s, Plane &p)
{
	double distance = v_math::dotProduct(s.center, p.planeNormal)-p.distFromOrigin;
	return fabs(distance) < s.radius;
}

bool isSphereFullyBehindPlane(Sphere &s, Plane &p)
{
	double distance = v_math::dotProduct(s.center, p.planeNormal)-p.distFromOrigin;
/*	std::cout<<"isSphereFullyBehindPlane"<<std::endl;
	std::cout<<"distFromOrigin: "<<p.distFromOrigin<<std::endl;
	std::cout<<"radius: "<<s.radius<<std::endl;
	std::cout<<"distance: "<< distance<<std::endl;*/
	return distance < -s.radius;
}

bool isPartOfSphereBehindPlane(Sphere &s, Plane &p)
{
	double distance = v_math::dotProduct(s.center, p.planeNormal)-p.distFromOrigin;
/*	std::cout<<"isPartOfSphereBehindPlane"<<std::endl;
	std::cout<<"distFromOrigin: "<<p.distFromOrigin<<std::endl;
	std::cout<<"radius: "<<s.radius<<std::endl;
	std::cout<<"distance: "<< distance<<std::endl;*/
	return distance <= s.radius;
}

} // namespace collision

