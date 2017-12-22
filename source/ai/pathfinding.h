#ifndef SOURCE_AI_PATHFINDING_H
#define SOURCE_AI_PATHFINDING_H
//#include "source/ai/pathfinding.h"

#include <cstddef>
#include <vector>
#include <map>
#include "source/math/geometric_math.h"

/**
 * Class used by PathSearchNodeGraph to represent a single node.
 *
 */
class PathSearchNode
{
public:

	/**
	 * Class used to hold information about nodes connected to this one by an
	 * edge. This abrieviated class rather than PathSearchNode is used to save
	 * memory.
	 *
	 */
	class ConnectedNode
	{
	public:
		/**
		 * handle of the node
		 *
		 */
		size_t node {0};

		/**
		 * The cost to travel across the edge connecting to the node. Negative
		 * values are valid.
		 *
		 */
		double cost {0.0};

		/**
		 * Default Constructor
		 *
		 */
		ConnectedNode() = default;

		/**
		 * Constructor that takes a node handle and a cost to reach the node.
		 *
		 * @param _node The handle of the connected node.
		 *
		 * @param _cost The cost to travel across the edge connecting to the
		 *              node. Negative values are valid.
		 *
		 */
		ConnectedNode(size_t _node, double _cost) : node(_node), cost(_cost) {}
	};

	/**
	 * This is a unique ID to relate the node to the object it is supposed to
	 * represent. This allows you to build a graph of objects without needing
	 * to store the actual objects or pointers to them in the graph (which
	 * could be invalidated if thier containers changed size). You just need a
	 * way to map the handle back to the object later. The intention is for the
	 * handle to be an index or key for an element in a container, but any
	 * number unique among the nodes in the graph will work.
	 *
	 */
	size_t handle {0};

	/**
	 * This is a unique ID to relate the node to the object it is supposed to
	 * represent. This allows you to build a graph of objects without needing
	 * to store the actual objects or pointers to them in the graph (which
	 * could be invalidated if thier containers changed size). You just need a
	 * way to map the handle back to the object later. The intention is for the
	 * handle to be an index or key for an element in a container, but any
	 * number unique among the nodes in the graph will work.
	 *
	 */
	Point3D pos {0.0, 0.0, 0.0};

	/**
	 * A list of nodes connected to this one.
	 *
	 */
	std::vector<ConnectedNode> connectedNodes {};

	/**
	 * Default Constructor
	 *
	 */
	PathSearchNode() = default;

	/**
	 * Constructor that sets the handle and position of the node.
	 *
	 * @param _handle The handle of the node.
	 *
	 * @param _position The position of the node.
	 *
	 */
	PathSearchNode(size_t _handle, const Point3D &_position) : handle(_handle),
	                      pos(_position), connectedNodes(){}

	/**
	 * Adds a new node that is directly connected to this one.
	 *
	 * @param node The handle of the neighbor node.
	 *
	 * @param cost The cost to traverse the edge to the neighbor node.
	 *
	 */
	void addNeighbor(size_t node, double cost);
};


/**
 * Container Class that represents the connections between objects for the
 * purpose of finding paths between them. The default constructor will create a
 * graph where all connections allow travel in both directions. To create a
 * graph with a one-way connection, pass the constructor a true value. To add
 * an object to the graph use addNode(). To add a connection, use addEdge() or
 * addEdgeUseDistance(). The class inherits from std::map so those methods are
 * available. The container stores the nodes, not the edges. The edges are
 * stored in adjacency lists in the nodes. You can access a node using it's
 * handle.
 *
 */
class PathSearchNodeGraph : public std::map<size_t, PathSearchNode>
{
private:

	/**
	 * Whether the graph edges are directed.
	 *
	 */
	bool directed {false};
public:

	/**
	 * Constructs an undirected graph where all edges connect in both directions.
	 *
	 */
	PathSearchNodeGraph() = default;

	/**
	 * Comstructs a PathSearchNodeGraph. Whether the graphs edges are directed
	 * or not is indicated by the directed argument.
	 *
	 * @param _directed True indicates the the graph's edges should only connect
	 *                  in one direction. false indicates that all edges connect
	 *                  in both directions.
	 *
	 */
	PathSearchNodeGraph(bool _directed) : directed(_directed) {}

	/**
	 * Adds a new node to the graph.
	 *
	 * @param handle This is a unique ID to relate the node to the object it is
	 *               supposed to represent. This allows you to build a graph of
	 *               objects without needing to store the actual objects or
	 *				 pointers to them in the graph (which could be invalidated
	 *               if thier containers changed size). You just need a way
	 *               to map the handle back to the object later. The intention
	 *               is for the handle to be an index or key for an element in a
	 *               container, but any number unique among the nodes in the
	 *               graph will work.
	 *
	 * @param position A 3D point the node is positioned at relative to an origin.
	 *                 This is required by some algorithms such as A* to calculate
	 *                 the distance between nodes relative to each other. This is
	 *                 also needed to use addEdgeUseDistance().
	 *
	 */
	void addNode(size_t handle, const Point3D &position = {0.0,0.0,0.0});

	/**
	 * Adds an edge using the given cost. The node's position is not taken into
	 * account. If the graph is not directed, an edge in both directions is added,
	 * otherwise the edge traverses A->B.
	 *
	 * @param vertA The handle of the node the edge begins on.
	 *
	 * @param vertB The handle of the node the edge ends on.
	 *
	 * @param cost The cost required to traverse the edge. A negative value
	 *             is valid.
	 *
	 */
	void addEdge(size_t vertA, size_t vertB, double cost);

	/**
	 * Adds a new edge to the graph using the distance between the nodes to
	 * set the cost. An additional cost beyond the distance can be optionally
	 * added. If the graph is not directed, an edge in both directions is added,
	 * otherwise the edge traverses A->B.
	 *
	 * @param vertA The handle of the node the edge begins on.
	 *
	 * @param vertB The handle of the node the edge ends on.
	 *
	 * @param additionalCost An additional cost beyond the distance that is
	 *                       required to traverse the edge. A negative value
	 *                       can be used to decrease the cost. The total cost
	 *                       can be negative.
	 *
	 */
	void addEdgeUseDistance(size_t vertA, size_t vertB, double additionalCost = 0);

	/**
	 * Returns true if the graph contains a node with the given handle.
	 *
	 * @param handle The handle to look for in the node.
	 *
	 * @return true if a node with the given handle is in the graph. Otherwise
	 *              false.
	 *
	 */
	bool contains(size_t handle);

	/**
	 * Returns whether the graph is directed or not.
	 *
	 * @return true if the graph is directed. Otherwise false.
	 *
	 */
	bool isDirected() { return directed; }
};

/**
 * Calculates the sequence of PathSearchNodes needed to be traveled to reach
 * goal from start using A*. The return value is an array of the handles from
 * the PathSearchNodes listed in the order they need to be traversed, starting
 * at the node following start and ending at goal.
 *
 * @param start The handle in nodeGraph of the node that you are starting on.
 *
 * @param goal The handle in nodeGraph of the goal you are trying to reach.
 *
 * @param nodeGraph A PathSearchNodeGraph containing start and goal.
 *
 * @return A vector of handles of the nodes that needs to be traveled through in
 *         order to reach goal. They are in the order that they need to be
 *         traversed in. The first element is the first element is the node
 *         after start. The last element is goal.
 *
 * @throws std::runtime_error if a path cannot be found.
 *
 */
std::vector<size_t> Astar(size_t start, size_t goal, PathSearchNodeGraph &nodeGraph);


#endif /* SOURCE_AI_PATHFINDING_H */
