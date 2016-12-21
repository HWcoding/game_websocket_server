#define TEST_FILE_LINK_DEPENDENCIES "source/math/geometric_math.cpp"


#include "tests/test.h"
#include "tests/test_lib/clock_cycle_counter.h"
#include <cstdint>

#include "source/math/geometric_math.h"



//speed tests
void builtinInvSqrtProf(){
	static double output = 1000;
	output += 1/sqrt(output);
}
void fastInvSqrtProf(){
	static double output = 1000;
	output += fastInvSqrt(output);
}




TEST(vector_math, scale)
{
	Point3D point(3.0, 2.0, 6.0);
	Point3D expected(3.0*7.0, 2.0*7.0, 6.0*7.0);

	Point3D r = v_math::scale(point, 7.0);

	EXPECT_DOUBLE_EQ(r.x, expected.x);
	EXPECT_DOUBLE_EQ(r.y, expected.y);
	EXPECT_DOUBLE_EQ(r.z, expected.z);
}


TEST(vector_math, fastNormalize)
{
	// 3^2 +  2^2 + 6^2 = 7^2

	Point3D point(3.0, 2.0, 6.0);
	Point3D expected(3.0/7.0, 2.0/7.0, 6.0/7.0);

	Point3D r = v_math::fastNormalize(point);

	double length = sqrt(r.x*r.x+r.y*r.y+r.z*r.z);

	// normalize uses approximate inverse square
	// which is inaccurate
	EXPECT_NEAR(length, 1.0, 0.001);

	EXPECT_NEAR(r.x, expected.x, 0.001);
	EXPECT_NEAR(r.y, expected.y, 0.001);
	EXPECT_NEAR(r.z, expected.z, 0.001);
}

TEST(vector_math, normalize)
{
	// 3^2 +  2^2 + 6^2 = 7^2

	Point3D point(3.0, 2.0, 6.0);
	Point3D expected(3.0/7.0, 2.0/7.0, 6.0/7.0);

	Point3D r = v_math::normalize(point);

	double length = sqrt(r.x*r.x+r.y*r.y+r.z*r.z);

	EXPECT_DOUBLE_EQ(length, 1.0);

	EXPECT_DOUBLE_EQ(r.x, expected.x);
	EXPECT_DOUBLE_EQ(r.y, expected.y);
	EXPECT_DOUBLE_EQ(r.z, expected.z);
}


TEST(vector_math, add)
{
	Point3D a(1.0, 2.0, 3.0);
	Point3D b(4.0, 5.0, 6.0);

	Point3D expected(5.0, 7.0, 9.0);

	Point3D r = v_math::add(a, b);

	EXPECT_DOUBLE_EQ(r.x, expected.x);
	EXPECT_DOUBLE_EQ(r.y, expected.y);
	EXPECT_DOUBLE_EQ(r.z, expected.z);
}


TEST(vector_math, subtract)
{
	Point3D a(1.0, 2.0, 3.0);
	Point3D b(4.0, 6.0, 8.0);

	Point3D expected(-3.0, -4.0, -5.0);

	Point3D r = v_math::subtract(a, b);

	EXPECT_DOUBLE_EQ(r.x, expected.x);
	EXPECT_DOUBLE_EQ(r.y, expected.y);
	EXPECT_DOUBLE_EQ(r.z, expected.z);
}


TEST(vector_math, dotProduct)
{
	Point3D a(1.0, 2.0, 3.0);
	Point3D b(4.0, 5.0, 6.0);

	double expected = 32.0;

	double r = v_math::dotProduct(a, b);

	EXPECT_DOUBLE_EQ(r, expected);
}


TEST(vector_math, crossProduct)
{
	Point3D a(1.0, 2.0, 3.0);
	Point3D b(4.0, 6.0, 11.0);

	Point3D expected(4.0, 1.0, -2.0);

	Point3D r = v_math::crossProduct(a, b);

	EXPECT_DOUBLE_EQ(r.x, expected.x);
	EXPECT_DOUBLE_EQ(r.y, expected.y);
	EXPECT_DOUBLE_EQ(r.z, expected.z);
}


TEST(vector_math, squareDistance)
{
	Point3D a(4.0, 6.0, 8.0);
	Point3D b(1.0, 4.0, 2.0);

	double expected = 49.0;

	double r = v_math::squareDistance(a, b);

	EXPECT_DOUBLE_EQ(r, expected);
}


TEST(vector_math, distance)
{
	Point3D a(4.0, 6.0, 8.0);
	Point3D b(1.0, 4.0, 2.0);

	double expected = 7.0;

	double r = v_math::distance(a, b);

	EXPECT_DOUBLE_EQ(r, expected);
}


TEST(collision, sphere)
{
	Sphere a = {Point3D(3.0,2.0,6.0),3.499};
	Sphere b = {Point3D(0.0,0.0,0.0),3.499};

	EXPECT_FALSE( collision::doSpheresCollide(a,b) );

	Sphere c = {Point3D(3.0,2.0,6.0),3.501};
	Sphere d = {Point3D(0.0,0.0,0.0),3.501};

	EXPECT_TRUE( collision::doSpheresCollide(c,d) );
}


TEST(collision, sphereAndPlane)
{
	Sphere s1 = {Point3D(0.0,3.5,0.0),3.49};
	Plane  p1(Point3D(0.0,0.0,0.0),Point3D(0.0,0.0,1.0),Point3D(1.0,0.0,0.0));

	EXPECT_FALSE( collision::doesSphereCollideWithPlane(s1, p1) );

	Sphere s2 = {Point3D(0.0,3.5,0.0),3.51};
	Plane  p2(Point3D(0.0,0.0,0.0),Point3D(0.0,0.0,1.0),Point3D(1.0,0.0,0.0));

	EXPECT_TRUE( collision::doesSphereCollideWithPlane(s2, p2) );
}

TEST(collision, sphereFullyBehindPlane)
{
	Sphere s1 = {Point3D(0.0,-3.5,0.0),3.51};
	Plane  p1(Point3D(0.0,0.0,0.0),Point3D(0.0,0.0,1.0),Point3D(1.0,0.0,0.0));

	EXPECT_FALSE( collision::isSphereFullyBehindPlane(s1, p1) );

	Sphere s2 = {Point3D(0.0,-3.5,0.0),3.49};
	Plane  p2(Point3D(0.0,0.0,0.0),Point3D(0.0,0.0,1.0),Point3D(1.0,0.0,0.0));

	EXPECT_TRUE( collision::isSphereFullyBehindPlane(s2, p2) );
}

TEST(collision, partOfSphereBehindPlane)
{
	Sphere s1 = {Point3D(0.0,3.5,0.0),3.49};
	Plane  p1(Point3D(0.0,0.0,0.0),Point3D(0.0,0.0,1.0),Point3D(1.0,0.0,0.0));

	EXPECT_FALSE( collision::isPartOfSphereBehindPlane(s1, p1) );

	Sphere s2 = {Point3D(0.0,3.5,0.0),3.51};
	Plane  p2(Point3D(0.0,0.0,0.0),Point3D(0.0,0.0,1.0),Point3D(1.0,0.0,0.0));

	EXPECT_TRUE( collision::isPartOfSphereBehindPlane(s2, p2) );

	Sphere s3 = {Point3D(0.0,-3.5,0.0),3.49};
	Plane  p3(Point3D(0.0,0.0,0.0),Point3D(0.0,0.0,1.0),Point3D(1.0,0.0,0.0));

	EXPECT_TRUE( collision::isPartOfSphereBehindPlane(s3, p3) );
}



int main(int argc, char *argv[])
{
	//std::cout<<"builtin cycles taken = "<<profiling::countCpuCycles(builtinInvSqrtProf,100000)<<std::endl;
	//std::cout<<"fast cycles taken =    "<<profiling::countCpuCycles(fastInvSqrtProf,100000)<<std::endl;

	::testing::InitGoogleTest(&argc, argv);
	STAY_SILENT_ON_SUCCESS;
	return RUN_ALL_TESTS();
}
