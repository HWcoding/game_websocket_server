
#include "source/math/geometric_math.h"
#include <cstdint>
#include <math.h>
#include <string.h>
#include <xmmintrin.h>

double fastInvSqrt(double input)
{
/*	int64_t bits;
	memcpy(&bits, &input, sizeof(int64_t));
	bits= 6910238826744275866LL - (bits>>1);
	double halfInput = 0.5*input;
	memcpy(&input, &bits, sizeof(int64_t));
	input *= 1.5 - halfInput*input*input;
	return input;*/

	float f = (float)input;
	float out;
	_mm_store_ss( &out, _mm_rsqrt_ss( _mm_load_ss( &f ) ) );
	return (double) out;
}


// extremely fast but very inaccurate. Good for heuristics
// fastSqrt(100) = 10.25
double fastSqrt(double f)
{
	uint64_t i;
	memcpy(&i , &f, sizeof(uint64_t));
	i -= 0x3FF0000000000000;
	i = i>>1;
	i += 0x3FF0000000000000;
	memcpy(&f, &i, sizeof(uint64_t));
	return f;
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

double estimatedDistance(const Point3D &a, const Point3D &b)
{
	return fastSqrt(squareDistance(a, b));
}


} // namespace v_math


namespace collision {

bool doesSphereIntersectSphere(const Sphere &a, const Sphere &b)
{
	double distanceSQ = v_math::squareDistance(a.center, b.center);
	double radDist = a.radius+b.radius;
	return distanceSQ <= radDist*radDist;
}

bool doesSphereIntersectPlane(const Sphere &s, const Plane &p)
{
	double distance = v_math::dotProduct(s.center, p.planeNormal)-p.distFromOrigin;
	return fabs(distance) < s.radius;
}

bool isSphereFullyBehindPlane(const Sphere &s, const Plane &p)
{
	double distance = v_math::dotProduct(s.center, p.planeNormal)-p.distFromOrigin;
	return distance < -s.radius;
}

bool isPartOfSphereBehindPlane(const Sphere &s, const Plane &p)
{
	double distance = v_math::dotProduct(s.center, p.planeNormal)-p.distFromOrigin;
	return distance <= s.radius;
}

bool doesLineIntersectPlane(const Line &line, const Plane &plane, Point3D &pointOfIntersection)
{
	using namespace v_math;

	Point3D diff = subtract(line.b, line.a);
	double dotOfPlaneAndA = dotProduct(plane.planeNormal, line.a);
	double dotOfPlaneAndDiff = dotProduct(plane.planeNormal, diff);
	double time = (plane.distFromOrigin - dotOfPlaneAndA)/dotOfPlaneAndDiff;
	if(time >= 0.0 && time <= 1.0f) {
		Point3D vectorToIntersection = scale(diff, time);
		pointOfIntersection = add(line.a, vectorToIntersection);
		return true;
	}
	else {
		return false;
	}
}

bool doesRayIntersectSphere(const Ray &ray, const Sphere &sphere, Point3D &pointOfIntersection)
{
	using namespace v_math;
	Point3D vectorToCenter = subtract(ray.origin, sphere.center);

	double dotOfRayAndCenter = dotProduct(vectorToCenter, ray.direction);
	double distanceToEdgeOfSphere = dotProduct(vectorToCenter,vectorToCenter) -
	                                                 sphere.radius * sphere.radius;

	// if starting poit is outside sphere and points away from sphere, we miss
	if(distanceToEdgeOfSphere > 0.0 && dotOfRayAndCenter > 0.0) return false;

	double discriminant = dotOfRayAndCenter * dotOfRayAndCenter - distanceToEdgeOfSphere;

	// no real roots so ray is not inline with sphere and misses
	if(discriminant < 0.0) return false;

	double time = -dotOfRayAndCenter - sqrt(discriminant);
	if(time < 0.0) time = 0.0;
	Point3D vectorToIntersection = scale(ray.direction, time);
	pointOfIntersection = add(ray.origin, vectorToIntersection);
	return true;
}

// faster version that does not find intersection point
bool doesRayIntersectSphere(const Ray &ray, const Sphere &sphere)
{
	using namespace v_math;
	Point3D vectorToCenter = subtract(ray.origin, sphere.center);

	double distanceToEdgeOfSphere = dotProduct(vectorToCenter,vectorToCenter) -
	                                                  sphere.radius * sphere.radius;

	// if at least one real root we hit
	if(distanceToEdgeOfSphere <= 0.0) return true;

	double dotOfRayAndCenter = dotProduct(vectorToCenter, ray.direction);

	// if starting point is outside sphere and points away from sphere, we miss
	if(dotOfRayAndCenter > 0.0) return false;

	double discriminant = dotOfRayAndCenter * dotOfRayAndCenter - distanceToEdgeOfSphere;

	// no real roots so ray is not inline with sphere and misses
	if(discriminant < 0.0) return false;

	// ray must hit sphere
	return true;
}


} // namespace collision














