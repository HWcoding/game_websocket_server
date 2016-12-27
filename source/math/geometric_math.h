#ifndef SOURCE_MATH_GEOMETRIC_MATH_H
#define SOURCE_MATH_GEOMETRIC_MATH_H
//#include "source/math/geometric_math.h"

struct Point3D {
	double x {0.0};
	double y {0.0};
	double z {0.0};

	Point3D() {}
	Point3D(double X, double Y, double Z) : x(X), y(Y), z(Z) {}
};



double fastInvSqrt(double input);

namespace v_math {

Point3D fastNormalize(const Point3D &p);
Point3D normalize(const Point3D &p);
Point3D scale(const Point3D &p, double scalar);
Point3D add(const Point3D &p1, const Point3D &p2);
Point3D subtract(const Point3D &p1, const Point3D &p2);
double dotProduct(const Point3D &p1, const Point3D &p2);
Point3D crossProduct(const Point3D &p1, const Point3D &p2);
double squareDistance(const Point3D &p1, const Point3D &p2);
double distance(const Point3D &p1, const Point3D &p2);

} // namespace vector_math



struct Plane
{
	double distFromOrigin {0.0};
	Point3D planeNormal {0.0,0.0,0.0};

	Plane();
	Plane( const Point3D &a, const Point3D &b, const Point3D &c )
	{
		set(a, b, c);
	}
	void set( const Point3D &a, const Point3D &b, const Point3D &c ) {
		planeNormal = v_math::normalize(v_math::crossProduct( v_math::subtract(b,a) , v_math::subtract(c,a) ));
		distFromOrigin = v_math::dotProduct(planeNormal, a);
	}

};

struct BoundingBox
{
	Point3D center {0.0,0.0,0.0};
	Point3D axisNormals[3] { {1.0,0.0,0.0},
	                         {0.0,1.0,0.0},
	                         {0.0,0.0,1.0}};
	Point3D halfWidth {1.0,1.0,1.0};

/*	compute(Point3D [], size_t size)
	{
		uint64_t numberOfPoints = size/sizeof(double)/3;
	}*/
};

struct Triangle
{
	Point3D a;
	Point3D b;
	Point3D c;
};

struct Line
{
	Point3D a;
	Point3D b;
};

struct Ray
{
	Point3D origin;
	Point3D direction;
};

struct Sphere
{
	Point3D center {0.0,0.0,0.0};
	double radius {1.0};
};




namespace collision {

bool doesSphereIntersectSphere(Sphere &a, Sphere &b);
bool doesSphereIntersectPlane(Sphere &s, Plane &p);
bool isSphereFullyBehindPlane(Sphere &s, Plane &p);
bool isPartOfSphereBehindPlane(Sphere &s, Plane &p);
bool doesLineIntersectPlane(Line &line, Plane &plane, Point3D &pointOfIntersection);
bool doesRayIntersectShere(const Ray &ray, const Sphere &sphere, Point3D &pointOfIntersection);

}

#endif /* SOURCE_MATH_GEOMETRIC_MATH_H */
