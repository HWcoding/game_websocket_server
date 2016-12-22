#include "source/server/socket/websocket/websocket_authenticator.h"
#include <openssl/sha.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include "source/server/socket/set_of_file_descriptors.h"
#include "source/server/socket/websocket/websocket_handshake.h"
#include "source/server/socket/system_wrapper.h"



AuthenticatorInterface::~AuthenticatorInterface(){}


WebsocketAuthenticator::WebsocketAuthenticator(SystemInterface *_systemWrap, SetOfFileDescriptors*FDs) : systemWrap(_systemWrap), handshakeReadBuffer(), handshakeWriteBuffer(), maxHandshakeSize(2048), fileDescriptors(FDs){}

bool WebsocketAuthenticator::isNotValidConnection(int newConnection, const ByteArray &hbuf, const ByteArray &sbuf) const{

	//place holder to silence unused warnings //TODO: check IP and port info
	newConnection ++;
	ByteArray h = hbuf;
	h.push_back(0);
	h[0]++;
	ByteArray s = sbuf;
	s.push_back(0);
	s[0]++;
	/////////////////////////////////////
	return false;
}


void WebsocketAuthenticator::closeFD(int FD){
	if(handshakeReadBuffer.count(FD) != 0)handshakeReadBuffer.erase(FD);
	if(handshakeWriteBuffer.count(FD) != 0)handshakeWriteBuffer.erase(FD);
}

void WebsocketAuthenticator::processHandshake(const ByteArray &in, int FD){
	if(in.size()+handshakeReadBuffer[FD].size() > maxHandshakeSize){
		throw -1;//throwInt("Client sent too much data.  Size: "<<(in.size()+handshakeReadBuffer[FD].size()) );
	}
	handshakeReadBuffer[FD].append(in);
	if(in.size()>=20){
		if(!isHandshake(in)){
			throw -1;//throwInt("Message is not a handshake");
		}
	}
	if(!isCompleteHandshake(handshakeReadBuffer[FD]))return;
	HandshakeHeaders headers = getHandshakeHeaders(handshakeReadBuffer[FD]);
	fileDescriptors->setCSRFkey(FD, headers.getSecWebSocketProtocol() );
	handshakeWriteBuffer[FD] = createHandshake(headers);
	handshakeReadBuffer.erase(FD);

	if(handshakeWriteBuffer[FD].empty()){ //error creating handshake
		throw -1;//throwInt("createHandshake failed");
	}
}

bool WebsocketAuthenticator::isHandshake(const ByteArray &in) const{
	std::string requestType("GET ");
	bool ret = memcmp(&in[0], &requestType[0], 4) == 0;
	//ws://192.168.1.3//socket HTTP/1.1
	if(!ret){
	 	std::string temp( reinterpret_cast<const char*>(&in[0]), 4);
		//LOG_ERROR("WebsocketAuthenticator::isHandshake", temp<< " is not == "<< "GET " );
	}
	return ret;
}

bool WebsocketAuthenticator::isCompleteHandshake(const ByteArray &in) const{
	if(in.size()<4)return false;
	size_t end = in.size()-1;
	uint8_t carriageReturn = static_cast<uint8_t>('\r');
	uint8_t newLine = static_cast<uint8_t>('\n');

	if(in[end-3] != carriageReturn || in[end-2] != newLine || in[end-1] != carriageReturn || in[end] != newLine) return false; //check for "\r\n\r\n" ending
	return true;
}

HandshakeHeaders WebsocketAuthenticator::getHandshakeHeaders(const ByteArray &in) const{
	HandshakeHeaders empty;
	HandshakeHeaders headers;
	if(isHandshakeInvalid(in))return empty;
	headers.fillHeaders(in);
	if(headers.areHeadersFilled() == false)return empty;
	return headers;
}

ByteArray WebsocketAuthenticator::createHandshake(const HandshakeHeaders &headers) const{
	ByteArray output;
	output.reserve(195);

	output.appendWithNoSize( std::string("HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ", 97) );
	output.append( createSecWebSocketAccept( headers.getSecWebSocketKey() ) );
	output.appendWithNoSize( std::string("\r\nSec-WebSocket-Protocol: ",26) );
	output.append( headers.getSecWebSocketProtocol() ); //b89b73c543d8375331366338079f5e7c
	output.appendWithNoSize( std::string("\r\n\r\n", 4) );

	return output;
}

ByteArray WebsocketAuthenticator::createSecWebSocketAccept(const ByteArray &SecWebSocketKey) const{
	ByteArray magicKey = SecWebSocketKey;
	magicKey.appendWithNoSize( std::string("258EAFA5-E914-47DA-95CA-C5AB0DC85B11", 36) ); //append magic number spec requires

	ByteArray EncryptSocketKey;		//make a buffer to store the hashed data
	EncryptSocketKey.resize(20);				//sha1 is 20bytes
	SHA1(&magicKey[0], magicKey.size(),&EncryptSocketKey[0]); //hash magicKey and store in EncryptSocketKey
	ByteArray SocketAccept;			//make a buffer to store the base64 version of the hash
	toBase64(EncryptSocketKey, SocketAccept);	//convert to base64 to get our Sec-WebSocket-Accept value
	return SocketAccept;
}

bool WebsocketAuthenticator::isHandshakeInvalid(const ByteArray &handShake) const{
	if(handShake.size() > 2048){
		//writeError("handshake header too large");
		return true;
	}
	else if(isHandshake(handShake))return false;
	else if (!isCompleteHandshake(handShake)) return true;
	else return true;
}

uint8_t WebsocketAuthenticator::convertTo64(uint8_t in) const{ //converts an 8bit int less than 64 into a base64 char
	if(in < 26)  		return static_cast<uint8_t>(in+65);
	else if(in < 52)  	return static_cast<uint8_t>(in+71);
	else if(in < 62)  	return static_cast<uint8_t>(in-4);
	else if(in == 62)	return 43;
	else if(in == 63)	return 47;
	//LOG_ERROR("WebsocketAuthenticator::convertTo64: ", "input out of range: "<<static_cast<int>(in)<<" should be less than 64" );
	return static_cast<uint8_t>('\\');	//return an invalid base64 char on error
}

void WebsocketAuthenticator::toBase64(const ByteArray &input, ByteArray &out) const{ //converts an array of data into a base64 string for calculating the Sec-WebSocket-Accept header field (won't work for general conversion)
	out = ByteArray();
	if(input.size() != 20){ //the incoming SHA-1 hash should be exactly 20 long.  This simplified function breaks for other lengths
		//LOG_ERROR("WebSocket::toBase64: ", "wrong input length: "<<input.size()<<" should be 20" );
		return;
	}
	out.resize(28);
	uint8_t temp;

	//splits bytes on 6bit bounderies in network byte order (assuming little endian on machine), converts the 6 bits to base64, then stores each in a char
	for(size_t i = 0, j = 0; i< 18; i+=3, j+=4){	//process first 18bytes of 'in' 3 bytes at a time.

		//out[0] will be the top 6 bits of in[0]
		temp = static_cast <uint8_t>(input[i]>>2);	//take top 6 bits of first byte
		out[j] = convertTo64(temp);				//convert to base64 and store them in first char

		//out[1] will be the bottom 2 bits of in[0] followed by the top 4 bits of in[1]
		temp = input[i] & 0x03;								//zero top 6 bits of first byte by & with 0000 0011
		temp = static_cast <uint8_t>(temp<<4);				//move the bottom 2 bits of the first byte to the 5th and 6th bits of temp
		temp = static_cast <uint8_t>((input[i+1]>>4)+temp);	//add the top 4 bits of the second byte to the bottom 4 bits of temp
		out[j+1] = convertTo64(temp);   					//convert to base64 and store temp in the second char

		//out[2] will be the bottom 4 bits of in[1] followed by the top 2 bits of in[2]
		temp = input[i+1] & 0x0F;							//zero top 4 bits of second byte by & with 0000 1111
		temp = static_cast <uint8_t>(temp <<2);				//move the the bottom 4 bits of second byte into the 6th-3rd bits of temp
		temp = static_cast <uint8_t>((input[i+2]>>6)+temp);	//add top 2 bits of third byte into bottom 2 bits of temp
		out[j+2] = convertTo64(temp);						//convert to base64 and store temp in third char

		//out[3] will be the bottom 6 bits of in[2]
		temp = input[i+2] & 0x3F;				//zero top 2 bits of third byte by & with 0011 1111
		out[j+3] = convertTo64(temp);			//convert bottom 6 bits of third byte to base64 and store in 4th char
	}											//repeat for next 3 bytes

	//the last 2 bytes need handled seperately because 20 is not a multiple of 3 and the = at the end breaks the pattern
	//out[24] will be the top 6 bits of in[18]
	temp = static_cast <uint8_t>(input[18]>>2);	//take top 6 bits of 19th byte
	out[24] = convertTo64(temp);				//convert to base64 and store them in 25th char

	//out[25] will be the bottom 2 bits of in[18] followed be the top 4 bits of in[19]
	temp = input[18] & 0x03;							//zero top 6 bits of 19th byte by & with 0000 0011
	temp = static_cast <uint8_t>(temp<<4);				//move the bottom 2 bits of the 19th byte to the 5th and 6th bits of temp
	temp = static_cast <uint8_t>((input[19]>>4)+temp);	//add the top 4 bits of the 20th byte to the bottom 4 bits of temp
	out[25] = convertTo64(temp);						//convert to base64 and store temp in the 26th char

	//out[26] will be the bottom 4 bits of in[19] followed by zeros (last byte is unused)
	temp = input[19] & 0x0F;					//zero top 4 bits of 20th byte by & with 0000 1111
	temp = static_cast <uint8_t>(temp <<2);		//move the the bottom 4 bits of second byte into the 6th-3rd bits of temp
	out[26] = convertTo64(temp);				//convert to base64 and store temp in 27th char
	out[27] = '=';								//cap with '=' in 28th char to signify the last group only has 2 bytes instead of 3
}

bool WebsocketAuthenticator::sendHandshake(int FD){
	size_t retSize = systemWrap->writeFD( FD, &handshakeWriteBuffer[FD][0], handshakeWriteBuffer[FD].size() );
	if(retSize< handshakeWriteBuffer[FD].size()){ //we didn't write all our data
		handshakeWriteBuffer[FD] = ByteArray( handshakeWriteBuffer[FD].begin()+static_cast<int64_t>(retSize), handshakeWriteBuffer[FD].end() );
		return false;
	}
	//we wrote all our data
	handshakeWriteBuffer.erase(FD);
	return true;
}
