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
	double a = 3.0;
	double b = 2.0;
	double c = 6.0;

	double normA = a*7.0;
	double normB = b*7.0;
	double normC = c*7.0;

	vector_math::scale(a, b, c, 7.0);

	EXPECT_DOUBLE_EQ(a, normA);
	EXPECT_DOUBLE_EQ(b, normB);
	EXPECT_DOUBLE_EQ(c, normC);
}


TEST(vector_math, normalize)
{
	// 3^2 +  2^2 + 6^2 = 7^2

	double a = 3.0;
	double b = 2.0;
	double c = 6.0;

	double normA = a/7.0;
	double normB = b/7.0;
	double normC = c/7.0;

	vector_math::normalize(a, b, c);

	double length = sqrt(a*a+b*b+c*c);

	// normalize uses approximate inverse square
	// which is inaccurate
	EXPECT_NEAR(length, 1.0, 0.001);

	EXPECT_NEAR(a, normA, 0.001);
	EXPECT_NEAR(b, normB, 0.001);
	EXPECT_NEAR(c, normC, 0.001);
}




int main(int argc, char *argv[])
{
	//std::cout<<"builtin cycles taken = "<<profiling::cpuCycleTest(builtinInvSqrtProf,100000)<<std::endl;
	//std::cout<<"fast cycles taken =    "<<profiling::cpuCycleTest(fastInvSqrtProf,100000)<<std::endl;

	::testing::InitGoogleTest(&argc, argv);
	STAY_SILENT_ON_SUCCESS;
	return RUN_ALL_TESTS();
}
