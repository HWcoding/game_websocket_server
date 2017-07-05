#define TEST_FILE_LINK_DEPENDENCIES "source/math/geometric_math.cpp, \
                                     source/ai/pathfinding.cpp"


#include "tests/test.h"
#include "tests/test_lib/clock_cycle_counter.h"
#include <cstdint>

#include "source/ai/pathfinding.h"



TEST(pathFinding, Astar)
{
	int array[] = {0,1,2,3,4,5};

	PathSearchNodeGraph nodes;

	nodes.addNode(0, Point3D{0,0,0});
	nodes.addNode(1, Point3D{1,1,1});
	//2 should be skipped over because it's further than 3
	nodes.addNode(2, Point3D{3,2.9,3});
	nodes.addNode(3, Point3D{3,3,3});
	nodes.addNode(4, Point3D{4,4,4});
	nodes.addNode(5, Point3D{5,5,5});

	nodes.addEdge(0,1);
	nodes.addEdge(1,2);
	nodes.addEdge(1,3);
	nodes.addEdge(2,4);
	nodes.addEdge(3,4);
	nodes.addEdge(4,5);

	std::vector<size_t> ints = Astar(0, 4, nodes);

	EXPECT_EQ(ints.size(), 3);

	EXPECT_EQ(array[ints[0]], 1);
	EXPECT_EQ(array[ints[1]], 3);
	EXPECT_EQ(array[ints[2]], 4);
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
