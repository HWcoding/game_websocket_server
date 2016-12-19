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

double fastInvSqrt(double input);

namespace vector_math{

void normalize(double &x, double &y, double &z);

void scale(double &x, double &y, double &z, double scalar);

void add(double &x1, double &y1, double &z1,
                const double &x2, const double &y2, const double &z2);

void subtract(double &x1, double &y1, double &z1,
               const double &x2, const double &y2, const double &z2);

Point3D normalize(const Point3D &p);

Point3D scale(const Point3D &p, double scalar);

Point3D add(const Point3D &p1, const Point3D &p2);

Point3D subtract(const Point3D &p1, const Point3D &p2);

} // namespace vector_math


#endif /* SOURCE_MATH_GEOMETRIC_MATH_H */
