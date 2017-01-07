#include "source/ai/pathfinding.h"
#include <cfloat>
#include <vector>
#include <algorithm>
#include <set>

#include <assert.h>

namespace {

double costEstimate(const Point3D &p1, const Point3D &p2)
{
	// estimatedDistance is inaccurate so we mult by 0.75 to ensure it is
	// less than the actual distance
	return v_math::estimatedDistance(p1, p2)*0.75;
}

// we store this info in a separate structure so we can reuse nodegraph
struct pathData
{
	// For each node, which node it can most efficiently be reached from.
	// If a node can be reached from many nodes, cameFrom will eventually contain the
	// most efficient previous step.
	size_t cameFrom = 0;
	// The cost of getting from the start node to this node.
	double costToNode = DBL_MAX;
	// The total cost of getting from the start node to the goal by
	// passing through this node. That value is partly known, partly heuristic.
	double costToGoal = DBL_MAX;
};

} // namespace



void PathSearchNode::addNeighbor(size_t node, double cost)
{
	ConnectedNode newNode;
	newNode.node = node;
	newNode.cost = cost;
	connectedNodes.push_back(newNode);
}

void PathSearchNodeGraph::addNode(size_t proxy, Point3D pos)
{
	PathSearchNode node;
	node.pos = pos;
	node.proxy = proxy;
	std::vector<PathSearchNode>::push_back(node);
}


void PathSearchNodeGraph::addEdge(size_t vertA, size_t vertB, double AdditionalCost)
{
	assert(AdditionalCost >= 0.0);
	Point3D posA = std::vector<PathSearchNode>::operator[](vertA).pos;
	Point3D posB = std::vector<PathSearchNode>::operator[](vertB).pos;

	double cost = v_math::distance(posA, posB)+AdditionalCost;

	std::vector<PathSearchNode>::operator[](vertA).addNeighbor(vertB, cost);
	std::vector<PathSearchNode>::operator[](vertB).addNeighbor(vertA, cost);
}


std::vector<size_t> Astar(size_t start, size_t goal, PathSearchNodeGraph &nodeGraph)
{
	std::vector<pathData> data(nodeGraph.size());

	// The set of nodes already evaluated.
	std::set<size_t>closedSet;
	// The set of currently discovered nodes still to be evaluated.
	// Initially, only the start node is known.
	std::set<size_t>openSet;
	openSet.emplace(start);

	// The cost of going from start to start is zero.
	data[start].costToNode = 0.0;

	// Total cost of getting from the start node to the goal is completely heuristic.
	data[start].costToGoal = costEstimate(nodeGraph[start].pos, nodeGraph[goal].pos);

	size_t current = start;
	while( ! openSet.empty()) {

		current = *std::begin(openSet);
		for(auto element : openSet){
			if(data[element].costToGoal < data[current].costToGoal)
				current = element;
		}

		if( current == goal ) {
			std::vector<size_t> output;
			while (current != start){
				output.push_back(nodeGraph[current].proxy);
				current = data[current].cameFrom;
			}
			std::reverse(output.begin(),output.end());
			return output;
		}

		openSet.erase(current);
		closedSet.emplace(current);

		for(auto neighbor : nodeGraph[current].connectedNodes) {
			// if neighbor is in closedSet ignore the neighbor
			if(closedSet.count(neighbor.node) != 0 ) continue;
			// The distance from start to a neighbor
			double tentativeCostToNode = data[current].costToNode + neighbor.cost;
			// if neighbor is not in openSet	we discovered a new node
			if(openSet.count(neighbor.node) == 0 ) {
				openSet.emplace(neighbor.node);
			}
			else if(tentativeCostToNode >= data[neighbor.node].costToNode) {
				continue; // This is not a better path.
			}
			// This path is the best until now. Record it!  estimatedDistance
			data[neighbor.node].cameFrom = current;
			data[neighbor.node].costToNode = tentativeCostToNode;
			data[neighbor.node].costToGoal = data[neighbor.node].costToNode + \
			           costEstimate(nodeGraph[neighbor.node].pos, nodeGraph[goal].pos);
		}
	}
	//no path from start to goal
	throw -1;
}
