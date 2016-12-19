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

struct Plane
{
	Point3D pointOnPlane;
	Point3D planeNormal;
};

struct Box
{
	Point3D center;
	Point3D normal;
	double width;
	double depth;
	double height;
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

struct Sphere
{
	Point3D center {0.0,0.0,0.0};
	double radius {1.0};
};

double fastInvSqrt(double input);

namespace vector_math {

Point3D normalize(const Point3D &p);
Point3D scale(const Point3D &p, double scalar);
Point3D add(const Point3D &p1, const Point3D &p2);
Point3D subtract(const Point3D &p1, const Point3D &p2);
double dotProduct(const Point3D &p1, const Point3D &p2);
Point3D crossProduct(const Point3D &p1, const Point3D &p2);
double squareDistance(const Point3D &p1, const Point3D &p2);
double distance(const Point3D &p1, const Point3D &p2);

} // namespace vector_math


#endif /* SOURCE_MATH_GEOMETRIC_MATH_H */
