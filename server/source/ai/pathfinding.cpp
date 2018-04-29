#include "source/ai/pathfinding.h"
#include <cfloat>
#include <vector>
#include <algorithm>
#include <set>
#include "source/logging/exception_handler.h"



void PathSearchNode::addNeighbor(size_t node, double cost)
{
	connectedNodes.emplace_back(ConnectedNode(node, cost));
}


bool PathSearchNodeGraph::contains(size_t key) {
	auto it = this->find(key);
	if (it != std::map<size_t, PathSearchNode>::end()) {
		return true;
	}
	return false;
}

void PathSearchNodeGraph::addNode(size_t handle, const Point3D &position)
{
	std::map<size_t, PathSearchNode>::emplace(handle, PathSearchNode(handle, position));
}

void PathSearchNodeGraph::addEdge(size_t vertA, size_t vertB, double cost)
{
	std::map<size_t, PathSearchNode>::operator[](vertA).addNeighbor(vertB, cost);
	if(!directed) {
		std::map<size_t, PathSearchNode>::operator[](vertB).addNeighbor(vertA, cost);
	}
}

void PathSearchNodeGraph::addEdgeUseDistance(size_t vertA, size_t vertB, double additionalCost)
{
	PathSearchNode &A = std::map<size_t, PathSearchNode>::operator[](vertA);
	PathSearchNode &B = std::map<size_t, PathSearchNode>::operator[](vertB);

	double cost = v_math::distance(A.pos, B.pos)+additionalCost;

	A.addNeighbor(vertB, cost);
	if(!directed) {
		B.addNeighbor(vertA, cost);
	}
}





std::vector<size_t> Astar(size_t start, size_t goal, PathSearchNodeGraph &nodeGraph)
{
	// we store this info in a separate structure so we can reuse nodegraph
	struct PathData
	{
		// For each node, which node can it most efficiently be reached from.
		// If a node can be reached from many nodes, cameFrom will eventually contain the
		// most efficient previous step.
		size_t cameFrom {0};
		// The cost of getting from the start node to this node.
		double costToNode {DBL_MAX};
		// The total cost of getting from the start node to the goal by
		// passing through this node. That value is partly known, partly heuristic.
		double costToGoal {DBL_MAX};

		PathData() = default;
		PathData(double _costToNode) : costToNode(_costToNode){}

		static double costEstimate(const Point3D &p1, const Point3D &p2)
		{
			// estimatedDistance is inaccurate so we multiply by 0.9 to ensure it is
			// less than the actual distance
			return v_math::estimatedDistance(p1, p2)*0.9;
		}
	};




	// check that the graph contains both the start and goal positions
	if(!nodeGraph.contains(start)) {
		throw std::runtime_error("Graph does not contain the start value");
	}
	if(!nodeGraph.contains(goal)) {
		throw std::runtime_error("Graph does not contain the goal value");
	}

	// PathData map
	std::map<size_t, PathData> data;

	// The set of nodes already evaluated.
	std::set<size_t>closedSet;

	// The set of currently discovered nodes still to be evaluated.
	// Initially, only the start node is known.
	std::set<size_t>openSet;
	openSet.emplace(start);

	// The cost of going from start to start is zero.
	data.emplace(start, PathData(0.0));

	// pointer to goal node
	auto &graphGoal = nodeGraph[goal];

	// Total cost of getting from the start node to the goal is completely heuristic.
	data.at(start).costToGoal = PathData::costEstimate(nodeGraph[start].pos, graphGoal.pos);



	size_t current = start;
	while( ! openSet.empty()) {

		// set current to first element in openSet
		current = *std::begin(openSet);
		// if there is a cheaper node in the openSet, set current to it
		PathData * cheapestNode = &data.at(current);
		for(auto element : openSet){
			if(data.at(element).costToGoal < cheapestNode->costToGoal) {
				current = element;
				cheapestNode = &data.at(current);
			}
		}

		// if we are done
		if( current == goal ) {
			std::vector<size_t> output;
			while (current != start){
				output.push_back(nodeGraph[current].handle);
				current = data.at(current).cameFrom;
			}
			std::reverse(output.begin(),output.end());
			return output;
		}

		// pointer to data[current]
		auto &dataCurrent = data.at(current);
		// pointer to nodeGraph[current]
		auto &graphCurrent = nodeGraph.at(current);

		// move the current node to the closedSet
		openSet.erase(current);
		closedSet.emplace(current);

		// loop through each of current's neighbors and add any new ones to the openSet.
		// If the neighbor is already in the openSet and the path to it though current is
		// cheaper than the old path to it, update it's values.
		for(auto neighbor : graphCurrent.connectedNodes) {

			// if neighbor is in closedSet ignore the neighbor
			if(closedSet.count(neighbor.node) != 0 ) continue;

			// The distance from start to a neighbor
			double tentativeCostToNode = dataCurrent.costToNode + neighbor.cost;

			// pointer to data[neighbor.node], add node if it doesn't exist
			auto &neighborData = data[neighbor.node];

			// if neighbor is not in openSet we discovered a new node
			if(openSet.count(neighbor.node) == 0 ) {
				openSet.emplace(neighbor.node);
			}
			else if(tentativeCostToNode >= neighborData.costToNode) {
				continue; // This is not a better path.
			}

			// This path is the best until now. Record it.
			neighborData.cameFrom = current;
			neighborData.costToNode = tentativeCostToNode;
			neighborData.costToGoal = neighborData.costToNode + \
			           PathData::costEstimate(nodeGraph[neighbor.node].pos, graphGoal.pos);
		}
	}
	//no path found from start to goal
	throw std::runtime_error("Could not find path from " + std::to_string(start) + " to " + std::to_string(goal) + ".");
}
