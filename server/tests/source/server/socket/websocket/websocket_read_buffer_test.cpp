#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/websocket/websocket_read_buffer.cpp, \
                                     source/server/socket/set_of_file_descriptors.cpp, \
                                     source/server/socket/file_descriptor.cpp, \
                                     source/logging/exception_handler.cpp, \
                                     source/data_types/socket_message.cpp, \
                                     source/data_types/byte_array.cpp"

#include "source/server/socket/websocket/websocket_read_buffer.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include "tests/test_lib/mocks_stubs/mock_system_wrapper.h"
#include "tests/test_lib/mocks_stubs/socket_test_helpers.h"
#include "tests/test_lib/clock_cycle_counter.h"

#include "tests/test.h"


class ReadBufferTestFactory{
public:
	WebsocketReadBuffers RB;
	ReadBufferTestFactory(SetOfFileDescriptors *FDs, size_t size): RB(FDs, size) {
		MockSystemWrapperState::resetState();
	}
};




// defined in source/server/socket/websocket/websocket_read_buffer.cpp
void unmask (ByteArray::iterator messageStart, ByteArray::iterator messageEnd, ByteArray & out);
uint16_t getNet16bit (ByteArray::iterator num);
uint64_t getNet64bit (ByteArray::iterator num);
int64_t getMessageSize (ByteArray::iterator &messageStart, ByteArray::iterator bufferEnd);



void test_Unmask_performance(){
	profiling::PerformanceTimer timer;
	uint32_t mask = 3893384930;
	size_t messageSize = 100000000;

	//create a number of strings and encode them with a mask the same way a browser would before sending them to this server
	std::string message;
	std::string maskedTestString;

	message = generateTestString(messageSize);
	maskedTestString.append(applyMask(message, mask));

	//test unmask
	uint8_t *pMask = reinterpret_cast<uint8_t*>(&mask);
	std::string fastMaskedTestString;
	fastMaskedTestString.append(reinterpret_cast<char*>(pMask),4);
	fastMaskedTestString.append(maskedTestString);

	ByteArray fastMaskedTestStringVec(fastMaskedTestString);

	double totalTime = DBL_MAX;

	int count2 = 10;
	while(--count2){
		int iterations = 10;
		int count = iterations;
		double localTime = 0;
		timer.start();
		while(--count){
			ByteArray fastUnmask;
			unmask(fastMaskedTestStringVec.begin(), fastMaskedTestStringVec.end(), fastUnmask);
		}
		localTime = timer.end();
		localTime /= iterations;
		if(localTime<totalTime)totalTime=localTime;
	}
	double gBitsPerSec = static_cast<double>(messageSize)/(totalTime*125000);
	std::cout<<"unmask processed at a rate of "<<gBitsPerSec<<" Gb/s"<<std::endl;
	std::cout<<"unmask processed "<<messageSize/1000000<<"MBs in "<<totalTime<<" milliseconds"<<std::endl;
}



//extract processed at a rate of 2.43902 Gb/s
//extract processed 800Megabits in 328 milliseconds
//release
//extract processed at a rate of 3.26531 Gb/s
//extract processed 800 Megabits in 245 milliseconds
void test_ExtractMessage_performance(){
	std::vector< ByteArray > splitMessage;
	uint32_t mask = 3893384930;
	size_t messageSize = 100000000;
	profiling::PerformanceTimer timer;
	{//build a vector of string fragments (splitMessage) simulating a long message retreived over multiple reads
		std::string testString = generateTestString(messageSize);
		std::string maskedTestString = maskMessageForTesting(std::move(testString), mask, true, 20533);
		size_t splitSize =3271; //size of each transmission
		size_t maskedSize = maskedTestString.size();
		for(size_t i = 0, position = 0; position<maskedSize; ++i, position += splitSize){
			splitMessage.push_back( ByteArray( maskedTestString.substr(position,splitSize) ) );
		}
	}

	// reset system state
	MockSystemWrapperState::resetState();

	SetOfFileDescriptors FDs;
	FDs.addFD(1);
	ReadBufferTestFactory readbuffer(&FDs, messageSize);

	double totalTime = DBL_MAX;
	int count = 10;
	while(--count){
		std::vector< ByteArray > decoded;
		std::vector<int> types;
		timer.start();
		for(auto element : splitMessage){
			readbuffer.RB.extractMessages(element, 1);
		}
		double localTime = timer.end();
		if(localTime<totalTime)totalTime=localTime;
	}
	double gBitsPerSec = static_cast<double>(messageSize)/(totalTime*125000);
	std::cout<<"extract processed at a rate of "<<gBitsPerSec<<" Gb/s"<<std::endl;
	std::cout<<"extract processed "<<messageSize/125000<<" Megabits in "<<totalTime<<" milliseconds"<<std::endl;
}






TEST(WebsocketMessageProcessorTest, testUnmask)
{
	// reset system state
	MockSystemWrapperState::resetState();

	uint32_t mask = 3893384930;
	size_t messageSize = 4000;

	//create a number of strings and encode them with a mask the same way a browser would before sending them to this server
	std::string message;
	std::string maskedTestString;

	message = generateTestString(messageSize);
	maskedTestString.append(applyMask(message, mask));

	//unmask simple way to test our masking test code
	std::string simpleUnmask;
	uint8_t *pMask = reinterpret_cast<uint8_t*>(&mask);
	char temp;
	for (uint64_t i = 0; i < maskedTestString.size(); ++i){
		//umask data by 'XOR'ing 4byte blocks with the mask one byte at a time
		//add unmasked byte to simpleUnmask
		temp =(char)(((uint8_t)maskedTestString[i])^pMask[i % 4]);
		simpleUnmask.append(1,temp);
	}

	EXPECT_STREQ( simpleUnmask.c_str(), message.c_str() );


	//our test produced proper input data, use it to test unmask
	std::string fastMaskedTestString;
	fastMaskedTestString.append(reinterpret_cast<char*>(pMask),4);
	fastMaskedTestString.append(maskedTestString);

	ByteArray input;
	input.appendWithNoSize(fastMaskedTestString);
	ByteArray output;

	unmask(input.begin() + 4, input.end(), output);
	std::string fastUnmask = output.toString();

	EXPECT_STREQ( fastUnmask.c_str(), message.c_str() );
}


TEST(WebsocketMessageProcessorTest, test_GetNet64bit)
{
	//0010 0001 1000 0100 1100 0110 0011 1001 0111 1011 1101 1110 1010 0101 0101 0110
	//33        132       198       57        123       222       165		86
	//2415273250371052886
	ByteArray temp;
	temp.resize(8);
	temp[0]= 33; //Network byte order
	temp[1]= 132;
	temp[2]= 198;
	temp[3]= 57;
	temp[4]= 123;
	temp[5]= 222;
	temp[6]= 165;
	temp[7]= 86;

	uint64_t result = getNet64bit(temp.begin());

	EXPECT_EQ(result, 2415273250371052886);
}



TEST(WebsocketMessageProcessorTest, testGetNet16bit)
{
	//1000 0100 0010 0001
	//132       33
	//33825
	ByteArray temp;
	temp.resize(2);
	temp[0]= 132; //Network byte order
	temp[1]= 33;

	uint16_t result = getNet16bit(temp.begin());

	EXPECT_EQ(result, 33825);
}



TEST(WebsocketReadBufferTest, extractMessages)
{
	std::vector<ByteArray> splitMessage;
	uint32_t mask = 3893384930;
	size_t messageSize = 70000;
	std::string message1 = generateTestString(messageSize);
	std::string message2 = generateTestString(messageSize, 12);
	//build a vector of ByteArray fragments (splitMessage) simulating 2 long messages retreived over multiple reads
	{
		std::string maskedTestString = maskMessageForTesting(message1, mask, true, 10533);
		maskedTestString.append(maskMessageForTesting(message2, mask, true, 10733));

		size_t splitSize =3271; //size of each transmission
		size_t maskedSize = maskedTestString.size();
		for(size_t i = 0, position = 0; position<maskedSize; ++i, position += splitSize){
			splitMessage.push_back( ByteArray( maskedTestString.substr(position,splitSize) ) );
		}
	}

	// reset system state
	MockSystemWrapperState::resetState();
	SetOfFileDescriptors FDs;
	FDs.addFD(1);
	ReadBufferTestFactory readbuffer(&FDs, messageSize);

	std::vector<SocketMessage> messages;
	// send each message part to readbuffer and put the completed messages in "messages"
	for(auto element : splitMessage){
		std::vector<SocketMessage> temp = readbuffer.RB.extractMessages(element, 1);
		if(temp.size() != 0) {
			for(auto m : temp) {
				messages.emplace_back(m);
			}
		}
	}

	// check to make sure we got 2 messages
	EXPECT_EQ(messages.size(), 2);
	// check to make sure the messages match the input
	EXPECT_STREQ( message1.c_str(), messages[0].getMessage().toString().c_str() );
	EXPECT_STREQ( message2.c_str(), messages[1].getMessage().toString().c_str() );
}


TEST(WebsocketReadBufferTest, extractMessagesTooLarge)
{
	std::vector<ByteArray> splitMessage;
	uint32_t mask = 3893384930;
	size_t messageSize = 1000;
	std::string message1 = generateTestString(messageSize);


	//build a vector of ByteArray fragments (splitMessage) simulating 2 long messages retreived over multiple reads
	{
		std::string maskedTestString = maskMessageForTesting(message1, mask, true, messageSize / 10);
		size_t splitSize = messageSize /10; //size of each transmission
		size_t maskedSize = maskedTestString.size();
		for(size_t i = 0, position = 0; position<maskedSize; ++i, position += splitSize){
			splitMessage.push_back( ByteArray( maskedTestString.substr(position,splitSize) ) );
		}
	}

	// reset system state
	MockSystemWrapperState::resetState();
	SetOfFileDescriptors FDs;
	FDs.addFD(1);
	// set max message size to messageSize - 1 (one too small)
	ReadBufferTestFactory readbuffer(&FDs, messageSize - 1);

	std::vector<SocketMessage> messages;
	// should throw while reading
	ASSERT_THROW(
	for(auto element : splitMessage){
		std::vector<SocketMessage> temp = readbuffer.RB.extractMessages(element, 1);
		if(temp.size() != 0) {
			for(auto m : temp) {
				messages.emplace_back(m);
			}
		}
	}
	, std::runtime_error);
}






int main(int argc, char *argv[])
{
	//test_Unmask_performance();
	//test_ExtractMessage_performance();
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
