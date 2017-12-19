#define TEST_FILE_LINK_DEPENDENCIES "source/server/socket/websocket/websocket_message_processor.cpp, \
                                     source/data_types/byte_array.cpp, \
                                     source/server/socket/system_wrapper.cpp, \
                                     source/server/socket/websocket/websocket_read_buffer.cpp, \
                                     source/data_types/socket_message.cpp, \
                                     source/server/socket/set_of_file_descriptors.cpp, \
                                     source/server/socket/message_queue.cpp, \
                                     source/server/socket/file_descriptor.cpp, \
                                     source/logging/exception_handler.cpp"

#include "source/server/socket/websocket/websocket_message_processor.h"
#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/message_queue.h"
#include "source/data_types/socket_message.h"
#include "source/data_types/message_type.h"
#include "tests/test_lib/mocks_stubs/mock_system_wrapper.h"
#include "tests/test_lib/mocks_stubs/socket_test_helpers.h"
#include "tests/test_lib/clock_cycle_counter.h"
#include <atomic>

#include "tests/test.h"




class WebsocketMessageProcessorWrap : public WebsocketMessageProcessor
{
public:
	WebsocketMessageProcessorWrap(SetOfFileDescriptors *_FDs) : WebsocketMessageProcessor(_FDs) {}
	using WebsocketMessageProcessor::extractMessages;
	using WebsocketMessageProcessor::getMessageSize;
	using WebsocketMessageProcessor::completeFracture;
	using WebsocketMessageProcessor::handleFragment;
	using WebsocketMessageProcessor::unmask;
	using WebsocketMessageProcessor::getNet64bit;
	using WebsocketMessageProcessor::getNet16bit;
};



/*void test_ExtractMessage_performance(){
	std::vector< ByteArray > splitMessage;
	uint32_t mask = 3893384930;
	size_t messageSize = 1000000;
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

	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketMessageProcessorWrap processor(&FDs);

	double totalTime = DBL_MAX;
	int count = 10;
	while(--count){
		std::vector< ByteArray > decoded;
		std::vector<int> types;
		timer.start();
		for(auto element : splitMessage){
			processor.extractMessages (element, decoded, types, 1);
		}
		double localTime = timer.end();
		if(localTime<totalTime)totalTime=localTime;
	}
	double gBitsPerSec = static_cast<double>(messageSize)/(totalTime*125000);
	std::cout<<"extract processed at a rate of "<<gBitsPerSec<<" Gb/s"<<std::endl;
	std::cout<<"extract processed "<<messageSize/1000000<<"MBs in "<<totalTime<<" milliseconds"<<std::endl;
}


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

	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketMessageProcessorWrap processor(&FDs);

	double totalTime = DBL_MAX;

	int count2 = 10;
	while(--count2){
		int iterations = 10;
		int count = iterations;
		double localTime = 0;
		timer.start();
		while(--count){
			ByteArray fastUnmask;
			processor.unmask(fastMaskedTestStringVec, fastUnmask, 4, maskedTestString.size());
		}
		localTime = timer.end();
		localTime /= iterations;
		if(localTime<totalTime)totalTime=localTime;
	}
	double gBitsPerSec = static_cast<double>(messageSize)/(totalTime*125000);
	std::cout<<"unmask processed at a rate of "<<gBitsPerSec<<" Gb/s"<<std::endl;
	std::cout<<"unmask processed "<<messageSize/1000000<<"MBs in "<<totalTime<<" milliseconds"<<std::endl;
}*/






TEST(WebsocketMessageProcessorTest, test_GetNet64bit)
{
	//0010 0001 1000 0100 1100 0110 0011 1001 0111 1011 1101 1110 1010 0101 0101 0110
	//33        132       198       57        123       222       165		86
	//2415273250371052886
	uint8_t temp[8];
	temp[0]= 33; //Network byte order
	temp[1]= 132;
	temp[2]= 198;
	temp[3]= 57;
	temp[4]= 123;
	temp[5]= 222;
	temp[6]= 165;
	temp[7]= 86;

	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketMessageProcessorWrap processor(&FDs);

	uint64_t result =processor.getNet64bit(temp);

	EXPECT_EQ(result, 2415273250371052886);
}



TEST(WebsocketMessageProcessorTest, testGetNet16bit)
{
	//1000 0100 0010 0001
	//132       33
	//33825
	uint8_t temp[2];
	temp[0]= 132; //Network byte order
	temp[1]= 33;

	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketMessageProcessorWrap processor(&FDs);

	uint16_t result =processor.getNet16bit(temp);

	EXPECT_EQ(result, 33825);
}



TEST(WebsocketMessageProcessorTest, testCloseFDHandler)
{
	//create processor for testing and set its state
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);

	ByteArray IP;
	IP.appendWithNoSize("IP");
	ByteArray PORT;
	PORT.appendWithNoSize("PORT");
	ByteArray KEY;
	KEY.appendWithNoSize("KEY");

	FDs.addFD(1);
	FDs.setIP(1,IP);
	FDs.setPort(1,PORT);
	FDs.setCSRFkey(1,KEY );

	WebsocketMessageProcessor processor(&FDs);
	std::atomic<bool> running;
	running.store(true);
	std::unique_ptr<MessageQueue> testReaderQueue(new MessageQueue(&running));
	processor.setReaderQueue(testReaderQueue.get());

	// should generage a logout message and place it in the message queue
	processor.closeFDHandler(1);

	// check that message queue contains a message
	EXPECT_FALSE( testReaderQueue->isEmpty() );

	SocketMessage temp = testReaderQueue->getNextMessage();
	// check that the message is logout
	EXPECT_EQ(temp.getType(), MessageType::logout);

	// check that the logout message has the right credentials
	EXPECT_STREQ( temp.getIP().toString().c_str(), "IP" );
	EXPECT_STREQ( temp.getPort().toString().c_str(), "PORT" );
	EXPECT_STREQ( temp.getCSRFkey().toString().c_str(), "KEY" );

}


TEST(WebsocketMessageProcessorTest, testUnmask)
{
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
		temp =(char)(((uint8_t)maskedTestString[i])^pMask[i % 4]);	//umask data by 'XOR'ing 4byte blocks with the mask one byte at a time
		simpleUnmask.append(1,temp);								//add unmasked byte to simpleUnmask
	}

	EXPECT_STREQ( simpleUnmask.c_str(), message.c_str() );


	//our test produced proper input data, use it to test unmask
	std::string fastMaskedTestString;
	fastMaskedTestString.append(reinterpret_cast<char*>(pMask),4);
	fastMaskedTestString.append(maskedTestString);

	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketMessageProcessorWrap processor(&FDs);

	ByteArray input;
	input.appendWithNoSize(fastMaskedTestString);
	ByteArray output;

	processor.unmask(input, output, 4, maskedTestString.size());
	std::string fastUnmask = output.toString();

	EXPECT_STREQ( fastUnmask.c_str(), message.c_str() );
}


TEST(WebsocketMessageProcessorTest, testProcessSockMessageWorks)
{
	uint32_t mask = 3893384930;
	uint32_t messageCount = 10;
	size_t messageSize = 200;

	//create a number of strings and encode them with a mask the same way a browser would before sending them to this server
	std::vector<std::string> messages;
	std::string maskedTestString;
	for(uint32_t i = 0; i <messageCount; ++i){
		messages.push_back( generateTestString(messageSize,i) );
		maskedTestString.append(maskMessageForTesting(messages[i], mask+static_cast<uint32_t>(i), true, 130));
	}

	ByteArray input;
	input.appendWithNoSize(maskedTestString);

	//create processor for testing and set its state
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);

	ByteArray IP;
	IP.appendWithNoSize("IP");
	ByteArray PORT;
	PORT.appendWithNoSize("PORT");
	ByteArray KEY;
	KEY.appendWithNoSize("KEY");

	FDs.addFD(1);
	FDs.setIP(1,IP);
	FDs.setPort(1,PORT);
	FDs.setCSRFkey(1,KEY );

	WebsocketMessageProcessor processor(&FDs);

	std::atomic<bool> running;
	running.store(true);
	//MessageQueue * testReaderQueue = new MessageQueue(&running);
	std::unique_ptr<MessageQueue> testReaderQueue(new MessageQueue(&running));

	processor.setReaderQueue(testReaderQueue.get());

	//process messages.  The function stores the output in a queue (testReaderQueue)
	processor.processSockMessage(input, 1);

	//move processed messages from queue into decoded
	std::vector< ByteArray > decoded;
	ByteArray temp = testReaderQueue->getNextMessage().getMessage();
	while(!temp.empty()){
		decoded.push_back(temp);
		temp = testReaderQueue->getNextMessage().getMessage();
	}

	//test to see if the number of messages output are the same as the number of messages input
	EXPECT_EQ( decoded.size(), messageCount );

	//test to see if the output messages match the input messages
	for(size_t i = 0; i< decoded.size(); ++i){
		EXPECT_STREQ( messages[i].c_str(), decoded[i].toString().c_str() );
	}

	// check that client is still connected
	EXPECT_TRUE(FDs.isFDOpen(1));

	ByteArray closeMessage;
	closeMessage.appendWithNoSize( createCloseControlMessage() );
	processor.processSockMessage(closeMessage, 1);
	// check that close message disconnected client
	EXPECT_FALSE(FDs.isFDOpen(1));
}


TEST(WebsocketMessageProcessorTest, testExtractMessageWorksWithPartialReads)
{
	uint32_t mask = 3893384930;
	uint32_t messageCount = 10;
	size_t messageSize = 200;

	std::vector<std::string> messages;
	std::string maskedTestString;
	for(uint32_t i = 0; i <messageCount; ++i){
		messages.push_back( generateTestString(messageSize,i) );
		maskedTestString.append(maskMessageForTesting(messages[i], mask+i, true, 130));
	}

	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketMessageProcessorWrap processor(&FDs);

	std::vector< ByteArray > decoded;
	std::vector<int> types;

	size_t readSize = messageSize/10;
	size_t position = 0;

	while( position < maskedTestString.size() ){
		std::vector< ByteArray > decodedTemp;
		ByteArray partial;
		partial.appendWithNoSize( maskedTestString.substr(position, readSize) );

		size_t decodedCount = processor.extractMessages (partial, decodedTemp, types, 1);
		for(size_t i = 0; i< decodedCount; ++i){
			decoded.push_back(decodedTemp[i]);
		}
		position+=readSize;
	}

	EXPECT_EQ( decoded.size(), messageCount );

	for(size_t i = 0; i< decoded.size(); ++i){
		EXPECT_STREQ( messages[i].c_str(), decoded[i].toString().c_str() );
	}
}

TEST(WebsocketMessageProcessorTest, testExtractMessageWorksWithWholeReads)
{
	uint32_t mask = 3893384930;
	uint32_t messageCount = 10;
	size_t messageSize = 200;

	std::vector<std::string> messages;
	std::string maskedTestString;
	for(uint32_t i = 0; i <messageCount; ++i){
		messages.push_back( generateTestString(messageSize,i) );
		maskedTestString.append(maskMessageForTesting(messages[i], mask+i, true, 130));
	}

	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketMessageProcessorWrap processor(&FDs);

	std::vector< ByteArray > decoded;
	std::vector<int> types;

	ByteArray input;
	input.appendWithNoSize(maskedTestString);

	size_t decodedCount = processor.extractMessages (input, decoded, types, 1);

	EXPECT_EQ( decodedCount, messageCount );

	for(size_t i = 0; i< decodedCount; ++i){
		EXPECT_STREQ( messages[i].c_str(), decoded[i].toString().c_str() );
	}
}




int main(int argc, char *argv[])
{
	//test_ExtractMessage_performance();
	//test_Unmask_performance();
	::testing::InitGoogleTest(&argc, argv);
	StaySilentOnSuccess();
	return RUN_ALL_TESTS();
}
