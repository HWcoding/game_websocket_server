#ifndef SOURCE_AI_PATHFINDING_H
#define SOURCE_AI_PATHFINDING_H
//#include "source/ai/pathfinding.h"

#include <cstddef>
#include <vector>
#include "source/math/geometric_math.h"

struct PathSearchNode
{
	// This node's position reletive to an origin. Used for cost estimation.
	Point3D pos {};

	struct ConnectedNode
	{
		// index to node
		size_t node {0};
		// cost to get to that node
		double cost {0.0};
	};
	// list of nodes this one connects to and the cost to get there
	std::vector<ConnectedNode> connectedNodes {};

	// An index to the actual structure or function this node is supposed
	// to represent. This is the value returned by Astar
	size_t proxy {0};

	void addNeighbor(size_t node, double cost);
};

struct PathSearchNodeGraph : public std::vector<PathSearchNode>
{
	void addNode(size_t proxy, const Point3D &pos);

	void addEdge(size_t vertA, size_t vertB, double additionalCost = 0);
};

// Calculates the sequence of PathSearchNodes needed to be traveled to reach goal from start using A*.
// The return value is an array of the proxy members from the PathSearchNodes listed in the order
// they need to be traversed, starting at the node following start and ending at goal.
std::vector<size_t> Astar(size_t start, size_t goal, PathSearchNodeGraph &nodeGraph);


#endif /* SOURCE_AI_PATHFINDING_H */
