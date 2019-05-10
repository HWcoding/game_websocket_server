#define TEST_FILE_LINK_DEPENDENCIES "source/message_handlers/client_partitioner.cpp"

#include "source/message_handlers/client_partitioner.h"
#include "tests/test.h"

class TestClient : public Client
{
public:
	TestClient(int _id, unsigned int _x, unsigned int _y) : Client(), id(_id), x(_x), y(_y), clients(nullptr), change(true) {}

	void setClientCluster(const std::vector<Client*> *_clients) override {
		clients = _clients;
	}

	bool changed() const override {
		return change;
	}

	ByteArray getData() const override {
		return ByteArray();
	}

	void setChanged(bool _change) {
		change = _change;
	}
	unsigned int getX() const {
		return x;
	}
	unsigned int getY() const {
		return y;
	}
	int getID() const {
		return id;
	}

	const std::vector<Client*> * getClients() {
		return clients;
	}
private:
	int id;
	unsigned int x;
	unsigned int y;
	const std::vector<Client*> *clients;
	bool change;
};

TEST(ClientPartitioner, Test)
{
	unsigned int radius = 3;
	unsigned int mapWidth = 12;
	unsigned int mapHeight = 12;

	ClientPartitioner partitioner(radius, mapWidth, mapHeight);

	std::vector<TestClient> clients;

	//                   id  x  y
	clients.emplace_back( 0, 0, 0);
	clients.emplace_back( 1, 3, 0);
	clients.emplace_back( 2, 4, 0);
	clients.emplace_back( 3, 7, 0);
	clients.emplace_back( 4, 9, 0);
	clients.emplace_back( 5, 2, 2);
	clients.emplace_back( 6, 3, 2);
	clients.emplace_back( 7, 0, 3);
	clients.emplace_back( 8, 2, 3);
	clients.emplace_back( 9, 0, 4);
	clients.emplace_back( 10, 0, 7);
	clients.emplace_back( 11, 0, 9);
	/*      0           1           2           3
	        |           |           |           |
	        0  1  2  3  4  5  6  7  8  9 10 11 12
	0   - 0 0        1  2        3     4
	      1
	      2       5  6
	      3 7     8
	1   - 4 9           +           +           +
	      5
	      6
	      7 10
	2   - 8             +           +           +
	      9 11
	     10
	     11
	3   -12             +           +           +

	0 -> UL 0,0  0, 1, 5, 7
	1 -> T  0,0  0, 1, 2, 5, 6, 7
	2 -> UL 1,0  1, 2, 3, 5, 6
	3 -> T  1,0  2, 3, 6
	4 -> T  2,0  4
	5 -> M  0,0  0, 1, 2, 5, 6, 7, 8, 9
	6 -> R  0,0  1, 2, 3, 5, 6, 8
	7 -> L  0,0  0, 1, 5, 7, 8, 9
	8 -> B  0,0  5, 6, 7, 8, 9, 10
	9 -> UL 0,1  5, 7, 8, 9, 10
	10-> L  0,1  8, 9, 10
	11-> L  0,2  11
	*/

	partitioner.reset();
	for(auto &client : clients) {
		client.setChanged(true);
		partitioner.addToCluster(client.getX(), client.getY(), &(client) );
	}

	EXPECT_EQ(clients.size(), 12);

	auto clusterClients = clients[0].getClients();
	// 0 -> UL 0,0  0, 1, 5, 7
	EXPECT_EQ(clusterClients->size(), 4);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[0]))->getID(), 0);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[1]))->getID(), 1);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[2]))->getID(), 5);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[3]))->getID(), 7);

	clusterClients = clients[1].getClients();
	// 1 -> T  0,0  0, 1, 2, 5, 6, 7
	EXPECT_EQ(clusterClients->size(), 6);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[0]))->getID(), 0);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[1]))->getID(), 1);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[2]))->getID(), 2);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[3]))->getID(), 5);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[4]))->getID(), 6);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[5]))->getID(), 7);

	clusterClients = clients[2].getClients();
	// 2 -> UL 1,0  1, 2, 3, 5, 6
	EXPECT_EQ(clusterClients->size(), 5);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[0]))->getID(), 1);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[1]))->getID(), 2);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[2]))->getID(), 3);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[3]))->getID(), 5);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[4]))->getID(), 6);

	clusterClients = clients[3].getClients();
	// 3 -> T  1,0  2, 3, 6
	EXPECT_EQ(clusterClients->size(), 3);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[0]))->getID(), 2);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[1]))->getID(), 3);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[2]))->getID(), 6);

	clusterClients = clients[4].getClients();
	// 4 -> T  2,0  4
	EXPECT_EQ(clusterClients->size(), 1);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[0]))->getID(), 4);

	clusterClients = clients[5].getClients();
	// 5 -> M  0,0  0, 1, 2, 5, 6, 7, 8, 9
	EXPECT_EQ(clusterClients->size(), 8);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[0]))->getID(), 0);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[1]))->getID(), 1);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[2]))->getID(), 2);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[3]))->getID(), 5);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[4]))->getID(), 6);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[5]))->getID(), 7);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[6]))->getID(), 8);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[7]))->getID(), 9);

	clusterClients = clients[6].getClients();
	// 6 -> R  0,0  1, 2, 3, 5, 6, 8
	EXPECT_EQ(clusterClients->size(), 6);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[0]))->getID(), 1);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[1]))->getID(), 2);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[2]))->getID(), 3);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[3]))->getID(), 5);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[4]))->getID(), 6);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[5]))->getID(), 8);

	clusterClients = clients[7].getClients();
	// 7 -> L  0,0  0, 1, 5, 7, 8, 9
	EXPECT_EQ(clusterClients->size(), 6);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[0]))->getID(), 0);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[1]))->getID(), 1);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[2]))->getID(), 5);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[3]))->getID(), 7);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[4]))->getID(), 8);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[5]))->getID(), 9);

	clusterClients = clients[8].getClients();
	// 8 -> B  0,0  5, 6, 7, 8, 9, 10
	EXPECT_EQ(clusterClients->size(), 6);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[0]))->getID(), 5);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[1]))->getID(), 6);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[2]))->getID(), 7);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[3]))->getID(), 8);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[4]))->getID(), 9);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[5]))->getID(), 10);

	clusterClients = clients[9].getClients();
	// 9 -> UL 0,1  5, 7, 8, 9, 10
	EXPECT_EQ(clusterClients->size(), 5);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[0]))->getID(), 5);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[1]))->getID(), 7);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[2]))->getID(), 8);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[3]))->getID(), 9);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[4]))->getID(), 10);

	clusterClients = clients[10].getClients();
	// 10-> L  0,1  8, 9, 10
	EXPECT_EQ(clusterClients->size(), 3);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[0]))->getID(), 8);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[1]))->getID(), 9);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[2]))->getID(), 10);

	clusterClients = clients[11].getClients();
	// 11-> L  0,2  11
	EXPECT_EQ(clusterClients->size(), 1);
	EXPECT_EQ((dynamic_cast<TestClient*>((*clusterClients)[0]))->getID(), 11);
}


int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
