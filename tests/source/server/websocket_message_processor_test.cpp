/*#define TESTING //to activate conditional macros for test logging
#include "tests/test.h"
//#include <iostream>
#include <cstring>
#include <limits>
#include <errno.h>
#include <string>
#include <memory>
#include "server/socket/set_of_file_descriptors.h"
#include "server/socket/socket_message.h"
#include "server/socket/websocket/websocket_message_processor.h"
#include "server/socket/websocket/websocket_read_buffer.h"
#include "server/socket/message_queue.h"
#include "engine/byte_array.h"

#include "tests/server/mocks_stubs/mock_system_wrapper.h"
#include "tests/server/websocket_message_processor_test/websocket_message_processor_test.h"
#include "tests/server/mocks_stubs/socket_test_helpers.h"
#include "tests/performance_test.h"
#include "main/includes.h"

namespace WebsocketMessageProcessor_Test{
*//*****************WebsocketMessageProcessor tests********************************//*

void test_GetMessageSize(){
	uint32_t mask = 3893384930;

	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketMessageProcessor processor(&FDs);


	{//test small message
		size_t messageSize = 125;
		std::string message;
		std::string maskedTestString = createMaskFragmentHeader(129, messageSize, mask);
		ByteArray input(maskedTestString);

		uint64_t MS;
		uint64_t start =0;
		uint64_t size = processor.getMessageSize (input, MS, start, 1);

		if(size != messageSize){
			TEST_PRINT(redTestText("Message size is wrong: "<<size<<" Size should be "<<messageSize));
			throw 1;
		}

		if(MS != 6){
			TEST_PRINT(redTestText("Message Start index is wrong: "<<MS<<" Index should be 6"));
			throw 1;
		}

		input[1]=125;//unset mask bit
		try{
			processor.getMessageSize (input, MS, start, 1); //should throw because mask bit unset
			TEST_PRINT(redTestText("getMessageSize did not throw with an set mask bit"));
			throw; //kill test
		}
		catch(...){}//correct behavior
	}

	{//test 16 bit size
		size_t messageSize = 126;
		std::string message;
		std::string maskedTestString = createMaskFragmentHeader(129, messageSize, mask);
		ByteArray input(maskedTestString);

		uint64_t MS;
		uint64_t start =0;
		uint64_t size = processor.getMessageSize (input, MS, start, 1);

		if(size != 0){
			TEST_PRINT(redTestText("Message size was too small but getMessage processed it"));
			throw 1;
		}

		maskedTestString.append(generateTestString(126));
		input = ByteArray(maskedTestString);
		size = processor.getMessageSize (input, MS, start, 1);

		if(size != messageSize){
			TEST_PRINT(redTestText("Message size is wrong: "<<size<<" Size should be "<<messageSize));
			throw 1;
		}


		if(MS != 8){
			TEST_PRINT(redTestText("Message Start index is wrong: "<<MS<<" Index should be 8"));
			throw 1;
		}
	}

	{//test 64 bit size
		size_t messageSize = 65536;
		std::string message;
		std::string maskedTestString = createMaskFragmentHeader(129, messageSize, mask);
		ByteArray input(maskedTestString);

		uint64_t MS;
		uint64_t start =0;
		uint64_t size = processor.getMessageSize (input, MS, start, 1);

		if(size != 0){
			TEST_PRINT(redTestText("Message size was too small but getMessage processed it"));
			throw 1;
		}

		maskedTestString.append(generateTestString(65536));
		input = ByteArray(maskedTestString);
		size = processor.getMessageSize (input, MS, start, 1);

		if(size != messageSize){
			TEST_PRINT(redTestText("Message size is wrong: "<<size<<" Size should be "<<messageSize));
			throw 1;
		}

		if(MS != 14){
			TEST_PRINT(redTestText("Message Start index is wrong: "<<MS<<" Index should be 14"));
			throw 1;
		}
	}

	{//Too Large size
		size_t messageSize = processor.MaxReadBufferSize+1;
		std::string message;
		std::string maskedTestString = createMaskFragmentHeader(129, messageSize, mask);
		maskedTestString.append(generateTestString(65536));
		ByteArray input(maskedTestString);

		uint64_t MS;
		uint64_t start =0;
		try{
			processor.getMessageSize (input, MS, start, 1); //should throw because message is too large
			TEST_PRINT(redTestText("getMessageSize did not throw with a message that is too large: "<<messageSize));
			throw; //kill test
		}
		catch(...){}//correct behavior
	}
}

void test_CompleteFracture(){
	//create processor for testing and set its state
	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	ByteArray IP( std::string("IP") );
	ByteArray PORT( std::string("PORT") );
	ByteArray KEY( std::string("KEY") );

	FDs.addFD(1);
	FDs.setIP(1,IP);
	FDs.setPort(1,PORT);
	FDs.setCSRFkey(1,KEY );
	WebsocketMessageProcessor processor(&FDs);

	bool running = true;
	std::unique_ptr<MessageQueue> testReaderQueue(new MessageQueue(&running));
	processor.setReaderQueue(testReaderQueue.get());

	std::string test("testing");
	ByteArray input(test);
	processor.handleFragment (input, 1, 1);
	input = ByteArray(test);
	processor.handleFragment (input, 0, 1);

	int type =0;
	input = ByteArray();

	processor.completeFracture (input, type, 0, 1);

	std::string result = input.toString();

	test.append(test);
	if(type != 1){
		TEST_PRINT(redTestText("Wrong message type: "<<processor.ReadBuffers->getFractureType(1)<<" Type should be 1"));
		throw 1;
	}
	if(result.compare(test) !=0){
		TEST_PRINT(redTestText("completed message does not match original.  Fracture buffer: "<<result<<" It should contain "<<test));
		throw 1;
	}
	if(processor.ReadBuffers->fractureBufferType.count(1) != 0){
		TEST_PRINT(redTestText("fractureBufferType was not erased"));
		throw 1;
	}
}

void test_HandleFragment(){
	{// test for error handling
		//create processor for testing and set its state
		MockSystemWrapper systemWrap;
		SetOfFileDescriptors FDs(&systemWrap);

		ByteArray IP( std::string("IP") );
		ByteArray PORT( std::string("PORT") );
		ByteArray KEY( std::string("KEY") );

		FDs.addFD(1);
		FDs.setIP(1,IP);
		FDs.setPort(1,PORT);
		FDs.setCSRFkey(1,KEY );

		WebsocketMessageProcessor processor(&FDs);

		bool running = true;
		std::unique_ptr<MessageQueue> testReaderQueue(new MessageQueue(&running));
		try{
			processor.setReaderQueue(testReaderQueue.get());

			ByteArray test( std::string("testing") );

			processor.handleFragment (test, 3, 1); //sending an invalid opcode (3). The function should throw an error
			TEST_PRINT(redTestText("handleFragment did not throw error with invalid opcode")); //exception should prevent this line from executing
			throw;//kill test
		}
		catch(...){}//correct behavior
	}

	{
		//create processor for testing and set its state
		MockSystemWrapper systemWrap;
		SetOfFileDescriptors FDs(&systemWrap);

		ByteArray IP( std::string("IP") );
		ByteArray PORT( std::string("PORT") );
		ByteArray KEY( std::string("KEY") );

		FDs.addFD(1);
		FDs.setIP(1,IP);
		FDs.setPort(1,PORT);
		FDs.setCSRFkey(1,KEY );

		WebsocketMessageProcessor processor(&FDs);

		bool running = true;
		std::unique_ptr<MessageQueue> testReaderQueue(new MessageQueue(&running));
		processor.setReaderQueue(testReaderQueue.get());

		std::string test("testing");
		ByteArray input(test);

		processor.handleFragment (input, 1, 1);
		if(processor.ReadBuffers->getFractureType(1) != true){
			TEST_PRINT(redTestText("Wrong fracture type: "<<processor.ReadBuffers->getFractureType(1)<<" Type should be true"));
			throw 1;
		}
		std::string fracBuffer = processor.ReadBuffers->fractureBuffer[1].buffer[0].toString();

		if(fracBuffer.compare(test) !=0){
			TEST_PRINT(redTestText("Fracture buffer does not match original.  Fracture buffer: "<<fracBuffer<<" It should contain "<<test));
			throw 1;
		}

		input = ByteArray(test);
		processor.handleFragment (input, 0, 1);

		if(processor.ReadBuffers->getFractureType(1) != true){
			TEST_PRINT(redTestText("Wrong fracture type: "<<processor.ReadBuffers->getFractureType(1)<<" Type should be true"));
			throw 1;
		}
		fracBuffer = processor.ReadBuffers->fractureBuffer[1].buffer[1].toString();
		if(fracBuffer.compare(test) !=0){
			TEST_PRINT(redTestText("Fracture buffer does not match original.  Fracture buffer: "<<fracBuffer<<" It should contain "<<test));
			throw 1;
		}

		input = ByteArray(test);

		processor.handleFragment (input, 2, 1);
		if(processor.ReadBuffers->getFractureType(1) != false){
			TEST_PRINT(redTestText("Wrong fracture type: "<<processor.ReadBuffers->getFractureType(1)<<" Type should be false"));
			throw 1;
		}
		fracBuffer = processor.ReadBuffers->fractureBuffer[1].buffer[2].toString();
		if(fracBuffer.compare(test) !=0){
			TEST_PRINT(redTestText("Fracture buffer does not match original.  Fracture buffer: "<<fracBuffer<<" It should contain "<<test));
			throw 1;
		}

		input = ByteArray(test);
		processor.handleFragment (input, 0, 1);

		if(processor.ReadBuffers->getFractureType(1) != false){
			TEST_PRINT(redTestText("Wrong fracture type: "<<processor.ReadBuffers->getFractureType(1)<<" Type should be false"));
			throw 1;
		}
		fracBuffer = processor.ReadBuffers->fractureBuffer[1].buffer[3].toString();
		if(fracBuffer.compare(test) !=0){
			TEST_PRINT(redTestText("Fracture buffer does not match original.  Fracture buffer: "<<fracBuffer<<" It should contain "<<test));
			throw 1;
		}
	}
}

































































void test_ExtractMessage_performance(){
	std::vector< ByteArray > splitMessage;
	uint32_t mask = 3893384930;
	size_t messageSize = 1000000;
	performanceTimer timer;
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
	WebsocketMessageProcessor processor(&FDs);



	double totalTime = 0;
	{
		std::vector< ByteArray > decoded;
		std::vector<int> types;
		timer.start();
		for(auto element : splitMessage){
			processor.extractMessage (element, decoded, types, 1);
		}
		totalTime = timer.end();
	}
	int count = 10;
	while(--count){
		std::vector< ByteArray > decoded;
		std::vector<int> types;
		timer.start();
		for(auto element : splitMessage){
			processor.extractMessage (element, decoded, types, 1);
		}
		double localTime = timer.end();
		if(localTime<totalTime)totalTime=localTime;
	}
	double gBitsPerSec = static_cast<double>(messageSize)/(totalTime*125000);
	std::cout<<"extract processed at a rate of "<<gBitsPerSec<<" Gb/s"<<std::endl;
	std::cout<<"extract processed "<<messageSize/1000000<<"MBs in "<<totalTime<<" milliseconds"<<std::endl;
}




void test_Unmask_performance(){
	performanceTimer timer;
	uint32_t mask = 3893384930;
	size_t messageSize = 500000;

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
	WebsocketMessageProcessor processor(&FDs);

	double totalTime = 0;
	{

		timer.start();
		int iterations = 100;
		int count = iterations;
		while(--count){
			ByteArray fastUnmask;
			processor.unmask(fastMaskedTestStringVec, fastUnmask, 4, maskedTestString.size());
		}
		totalTime = timer.end();
		totalTime /= iterations;
	}
	int count2 = 10;
	while(--count2){
		timer.start();
		int iterations = 100;
		int count = iterations;
		while(--count){
			ByteArray fastUnmask;
			processor.unmask(fastMaskedTestStringVec, fastUnmask, 4, maskedTestString.size());
		}
		double localTime = timer.end();
		localTime /= iterations;
		if(localTime<totalTime)totalTime=localTime;
	}
	double gBitsPerSec = static_cast<double>(messageSize)/(totalTime*125000);
	std::cout<<"unmask processed at a rate of "<<gBitsPerSec<<" Gb/s"<<std::endl;
	std::cout<<"unmask processed "<<messageSize/1000000<<"MBs in "<<totalTime<<" milliseconds"<<std::endl;
}

*/
































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
#include <atomic>

#include "tests/test.h"




class WebsocketMessageProcessorWrap : public WebsocketMessageProcessor
{
public:
	WebsocketMessageProcessorWrap(SetOfFileDescriptors *_FDs) : WebsocketMessageProcessor(_FDs) {}
	using WebsocketMessageProcessor::extractMessage;
	using WebsocketMessageProcessor::getMessageSize;
	using WebsocketMessageProcessor::completeFracture;
	using WebsocketMessageProcessor::handleFragment;
	using WebsocketMessageProcessor::unmask;
	using WebsocketMessageProcessor::getNet64bit;
	using WebsocketMessageProcessor::getNet16bit;
};

















TEST(WebsocketMessageProcessorTest, test_GetNet64bit)
{
	//0010 0001 1000 0100 1100 0110 0011 1001 0111 1011 1101 1110 1010 0101 0101 0110
	//33        132       198       57        123       222       165		86
	//2415273250371052886
	uint8_t temp[8];
	temp[0]= 33; //Network byte order
	temp[1]= 132; //-124; //
	temp[2]= 198; //-58; //
	temp[3]= 57;
	temp[4]= 123;
	temp[5]= 222; //-34;//
	temp[6]= 165; //-91;//
	temp[7]= 86;

	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketMessageProcessorWrap processor(&FDs);

	uint64_t result =processor.getNet64bit(temp);

	EXPECT_EQ(result, 2415273250371052886);
	/*if(result != 2415273250371052886){
		TEST_PRINT(redTestText("result is wrong: "<<result<<" Result should be 2415273250371052886"));
		throw 1;
	}*/
}



TEST(WebsocketMessageProcessorTest, testGetNet16bit)
{
	//1000 0100 0010 0001
	//132       33
	//33825
	uint8_t temp[2];
	temp[0]= 132; //Network byte order //-124;//
	temp[1]= 33;

	MockSystemWrapper systemWrap;
	SetOfFileDescriptors FDs(&systemWrap);
	FDs.addFD(1);
	WebsocketMessageProcessorWrap processor(&FDs);

	uint16_t result =processor.getNet16bit(temp);

	EXPECT_EQ(result, 33825);
	/*if(result != 33825){
		TEST_PRINT(redTestText("result is wrong: "<<result<<" Result should be 33825"));
		throw 1;
	}*/
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

	processor.closeFDHandler(1);

	EXPECT_FALSE( testReaderQueue->isEmpty() );
	/*if( testReaderQueue->isEmpty() ){
		TEST_PRINT(redTestText("Message queue was empty"));
		throw 1;
	}*/

	SocketMessage temp = testReaderQueue->getNextMessage();

	EXPECT_EQ(temp.getType(), MessageType::logout);
	/*if(temp.getType()!=2){
		TEST_PRINT(redTestText("Message is wrong type: "<<temp.getType()<<" Type should be 2"));
		throw 1;
	}*/

	std::string testIP = temp.getIP().toString();
	std::string testPort = temp.getPort().toString();
	std::string testCSRFkey = temp.getCSRFkey().toString();

	EXPECT_STREQ( testIP.c_str(), "IP" );
	/*if(testIP.compare(std::string("IP")) != 0){
		TEST_PRINT(redTestText("Message has wrong IP: \""<<testIP<<"\" IP should be \"IP\""));
		throw 1;
	}*/

	EXPECT_STREQ( testPort.c_str(), "PORT" );
	/*if(testPort.compare(std::string("PORT")) != 0){
		TEST_PRINT(redTestText("Message has wrong Port: \""<<testPort<<"\" Port should be \"PORT\""));
		throw 1;
	}*/

	EXPECT_STREQ( testCSRFkey.c_str(), "KEY" );
	/*if(testCSRFkey.compare(std::string("KEY")) != 0){
		TEST_PRINT(redTestText("Message has wrong CSRFkey: \""<<testCSRFkey<<"\" CSRFkey should be \"KEY\""));
		throw 1;
	}*/
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

	//move processed messages from queue into vector decoded
	std::vector< ByteArray > decoded;
	ByteArray temp = testReaderQueue->getNextMessage().getMessage();
	while(!temp.empty()){
		decoded.push_back(temp);
		temp = testReaderQueue->getNextMessage().getMessage();
	}

	//test to see if the number of messages output are the same as the number of messages input
	EXPECT_EQ( decoded.size(), messageCount );
	/*if(decoded.size() != messageCount){
		TEST_PRINT(redTestText("The number of processed messages, "<<decoded.size()<<" is not equal to the number of encoded messages, "<<messageCount));
		throw 1;
	}*/

	//test to see if the output messages match the input messages
	for(size_t i = 0; i< decoded.size(); ++i){
		EXPECT_STREQ( messages[i].c_str(), decoded[i].toString().c_str() );
	}
	/*for(size_t i = 0; i< decoded.size(); ++i){
		if(messages[i].compare( decoded[i].toString() )!=0){
			TEST_PRINT(redTestText("decoded message "<<i<<" does not match encoded message "<<i));
			TEST_PRINT(redTestText("decoded message \n"<<messages[i]<<"\nencoded message \n"<<decoded[i].toString() ));
			throw 1;
		}
	}*/

	//test control message handling
	EXPECT_TRUE(FDs.isFDOpen(1));
	/*if(!FDs.isFDOpen(1)){
		TEST_PRINT(redTestText("client is disconnected but should not be"));
		throw 1;
	}*/


	ByteArray closeMessage;
	closeMessage.appendWithNoSize( createCloseControlMessage() );
	processor.processSockMessage(closeMessage, 1);
	EXPECT_FALSE(FDs.isFDOpen(1));
	/*if(FDs.isFDOpen(1)){
		TEST_PRINT(redTestText("Close control message did not disconnect client"));
		throw 1;
	}*/
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

		size_t decodedCount = processor.extractMessage (partial, decodedTemp, types, 1);
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

	size_t decodedCount = processor.extractMessage (input, decoded, types, 1);

	EXPECT_EQ( decodedCount, messageCount );

	for(size_t i = 0; i< decodedCount; ++i){
		EXPECT_STREQ( messages[i].c_str(), decoded[i].toString().c_str() );
	}
}








int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	STAY_SILENT_ON_SUCCESS;
	return RUN_ALL_TESTS();
}
