#define TEST_FILE_LINK_DEPENDENCIES "source/math/geometric_math.cpp, \
                                     source/ai/pathfinding.cpp"


#include "tests/test.h"
#include "tests/test_lib/clock_cycle_counter.h"
#include <cstdint>

#include "source/ai/pathfinding.h"



TEST(pathFinding, Astar)
{
	PathSearchNodeGraph graph;

	/*
	         5
	       /   \
	      /     \
	 0---1---4   3
	      \  |  /
	         2
	*/

	graph.addNode(0, {0,0,0});
	graph.addNode(1, {1,1,1});
	graph.addNode(2, {1,2,2});
	graph.addNode(3, {4,4,4});
	graph.addNode(4, {2,2,2});
	graph.addNode(5, {1,6,1});

	graph.addEdge(0,1);
	graph.addEdge(1,2);
	graph.addEdge(1,4);
	graph.addEdge(1,5);
	graph.addEdge(4,2);
	graph.addEdge(5,3);
	graph.addEdge(2,3);

	std::vector<size_t> ints = Astar(0, 3, graph);

	EXPECT_EQ(ints.size(), 3);

	EXPECT_EQ(ints[0], 1);
	EXPECT_EQ(ints[1], 2);
	EXPECT_EQ(ints[2], 3);


	PathSearchNodeGraph graph2;
	graph2.addNode(0, {0,0,0});
	graph2.addNode(1, {1,1,1});
	graph2.addNode(2, {2,2,2});
	graph2.addEdge(0,1);
	// should not be able to find a path from 0-2 in graph2
	ASSERT_THROW(ints = Astar(0, 2, graph2), std::runtime_error);

	// start is not on graph2
	ASSERT_THROW(ints = Astar(3, 2, graph2), std::runtime_error);

	// goal is not on graph2
	ASSERT_THROW(ints = Astar(0, 3, graph2), std::runtime_error);
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
