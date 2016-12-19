
#include "source/math/geometric_math.h"
#include <cstdint>
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



namespace vector_math{

void normalize(double &x, double &y, double &z)
{
	double scalar = x*x+y*y+z*z;
	scalar = fastInvSqrt(scalar);
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

void scale(double &x, double &y, double &z, double scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

void add(double &x1, double &y1, double &z1,
                const double &x2, const double &y2, const double &z2)
{
	x1 += x2;
	y1 += y2;
	z1 += z2;
}

void subtract(double &x1, double &y1, double &z1,
               const double &x2, const double &y2, const double &z2)
{
	x1 -= x2;
	y1 -= y2;
	z1 -= z2;
}

Point3D normalize(const Point3D &p)
{
	Point3D output(p.x, p.y, p.z);
	normalize(output.x, output.y, output.z);
	return output;
}

Point3D scale(const Point3D &p, double scalar)
{
	Point3D output(p.x, p.y, p.z);
	scale(output.x, output.y, output.z, scalar);
	return output;
}

Point3D add(const Point3D &p1, const Point3D &p2)
{
	Point3D output(p1.x, p1.y, p1.z);
	add(output.x, output.y, output.z, p2.x, p2.y, p2.z);
	return output;
}

Point3D subtract(const Point3D &p1, const Point3D &p2)
{
	Point3D output(p1.x, p1.y, p1.z);
	subtract(output.x, output.y, output.z, p2.x, p2.y, p2.z);
	return output;
}



} // namespace vector_math
