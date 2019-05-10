#include "source/message_handlers/client_partitioner.h"
#include <stdexcept>

ClientPartitioner::ClientPartitioner(unsigned int _radius, unsigned int mapWidth, unsigned int mapHeight) : clients(), radius(_radius), gridWidth(radius * 4 / 3)
{
	unsigned int widthCount = mapWidth/gridWidth + 2;
	widthCount = widthCount * 2 - 1;
	unsigned int heightCount = mapHeight/gridWidth + 2;
	heightCount = heightCount * 2 - 1;

	clients.resize(widthCount);
	for(auto & clientRow : clients ) {
		clientRow.resize(heightCount);
	}
}

void ClientPartitioner::reset()
{
	for(auto & clientRow : clients ) {
		for(auto & client : clientRow) {
			client.clear();
		}
	}
}

void ClientPartitioner::addToCluster(unsigned int xValue, unsigned int yValue, Client *client)
{
	// quantize the x value to get the x index of the grid (later mult by 2 to get actual)
	unsigned int upperLeftXindex = xValue / gridWidth;
	// this is the x coordinate of the grid point to the upperLeft of the xValue
	double xGridPos = upperLeftXindex * gridWidth;
	// quantize the y value to get the y index of the grid (later mult by 2 to get actual)
	unsigned int upperLeftYindex = yValue / gridWidth;
	// this is the y coordinate of the grid point to the upperLeft of the yValue
	double yGridPos = upperLeftYindex * gridWidth;

	// multiply by 2 to get actual index (a value containing a blend of adjacent indexes is located between each)
	upperLeftXindex *= 2;
	upperLeftYindex *= 2;

	// calculate the square of the component distances between the x/y values and the 4 grid points
	double leftDistSqr = xValue - xGridPos;
	leftDistSqr *= leftDistSqr;

	double rightDistSqr = (xGridPos + gridWidth) - xValue;
	rightDistSqr *= rightDistSqr;

	double topDistSqr = yValue - yGridPos;
	topDistSqr *= topDistSqr;

	double bottomDistSqr = (yGridPos + gridWidth) - yValue;
	bottomDistSqr *= bottomDistSqr;

	// calculate square radius so we can compare without needing a squareroot
	double radSqr = radius*radius;

	// see if each grid point surrounding the x/y values is within radius
	bool upperLeft = leftDistSqr + topDistSqr <= radSqr;
	bool lowerLeft = leftDistSqr + bottomDistSqr <= radSqr;
	bool upperRight = rightDistSqr + topDistSqr <= radSqr;
	bool lowerRight = rightDistSqr + bottomDistSqr <= radSqr;

	// check other clusters
	bool middle = upperLeft && lowerLeft && upperRight && lowerRight;
	bool left = upperLeft && lowerLeft;
	bool right = upperRight && lowerRight;
	bool top = upperLeft && upperRight;
	bool bottom = lowerLeft && lowerRight;

	size_t xPos = 0;
	size_t yPos = 0;

	// get x and y index of cluster client belongs to. Check broadest clusters first.
	if(middle) {
		xPos = upperLeftXindex + 1;
		yPos = upperLeftYindex + 1;

	} else if(left) {
		xPos = upperLeftXindex;
		yPos = upperLeftYindex + 1;

	} else if(right) {
		xPos = upperLeftXindex + 2;
		yPos = upperLeftYindex + 1;

	} else if(top) {
		xPos = upperLeftXindex + 1;
		yPos = upperLeftYindex;

	} else if(bottom) {
		xPos = upperLeftXindex + 1;
		yPos = upperLeftYindex + 2;

	} else if(upperLeft) {
		xPos = upperLeftXindex;
		yPos = upperLeftYindex;

	} else if(lowerLeft) {
		xPos = upperLeftXindex;
		yPos = upperLeftYindex + 2;

	} else if(upperRight) {
		xPos = upperLeftXindex + 2;
		yPos = upperLeftYindex;

	} else if(lowerRight) {
		xPos = upperLeftXindex + 2;
		yPos = upperLeftYindex + 2;

	} else {
		throw std::runtime_error("client did not have a cluster");
	}

	// set client to use cluster it belongs to.
	client->setClientCluster(&clients[xPos][yPos]);

	// if client needs to update other clients about changes add to all affected clusters
	if(client->changed()) {
		if(xPos > 0) {
			// upper left
			if(yPos > 0) {
				clients[xPos - 1][yPos - 1].emplace_back(client);
			}
			// lower left
			if(yPos < clients[xPos - 1].size() - 1) {
				clients[xPos - 1][yPos + 1].emplace_back(client);
			}
			// left
			clients[xPos - 1][yPos].emplace_back(client);

		}
		if(xPos < clients.size() - 1) {
			// upper right
			if(yPos > 0) {
				clients[xPos + 1][yPos - 1].emplace_back(client);
			}
			// lower right
			if(yPos < clients[xPos + 1].size() - 1) {
				clients[xPos + 1][yPos + 1].emplace_back(client);
			}
			// right
			clients[xPos + 1][yPos].emplace_back(client);
		}
		// above
		if(yPos > 0) {
			clients[xPos][yPos - 1].emplace_back(client);
		}
		// below
		if(yPos < clients[xPos].size() - 1) {
			clients[xPos][yPos + 1].emplace_back(client);
		}

		// center
		clients[xPos][yPos].emplace_back(client);
	}
}