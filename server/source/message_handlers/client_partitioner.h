#ifndef SOURCE_MESSAGE_HANDLERS_CLIENT_PARTITIONER_H
#define SOURCE_MESSAGE_HANDLERS_CLIENT_PARTITIONER_H
//#include "source/message_handlers/client_partitioner.h"

#include "source/data_types/byte_array.h"
#include <vector>

class Client
{
public:
	virtual void setClientCluster(const std::vector<Client*> *_clients) = 0;
	virtual ByteArray getData() const = 0;
	virtual bool changed() const = 0;
	virtual ~Client() = default;
};

class ClientPartitioner
{
private:
	std::vector<std::vector<std::vector<Client*>>> clients;
	unsigned int radius;
	unsigned int gridWidth;
public:
	ClientPartitioner(unsigned int _radius, unsigned int mapWidth, unsigned int mapHeight);
	void reset();
	void addToCluster(unsigned int xValue, unsigned int yValue, Client *client);
};

#endif /* SOURCE_MESSAGE_HANDLERS_CLIENT_PARTITIONER_H */
