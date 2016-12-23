#include <string.h>
#include "source/server/socket/websocket/websocket_handshake.h"
#include "source/logging/exception_handler.h"

namespace HeaderHelpers {
	bool isHeaderValid(const ByteArray &vec, const std::string &expectedValue);
	bool isUpgradeHeader(const ByteArray &header);
	bool isConnectionHeader(const ByteArray &header);
	bool isSecWebSocketKeyHeader(const ByteArray &header);
	bool isSecWebSocketProtocolHeader(const ByteArray &header);
	bool isCookieHeader(const ByteArray &header);
	bool getNextLine(size_t &index, const ByteArray &in, ByteArray &out);

	ByteArray getHeader(const ByteArray &header, size_t headerBegin);
	ByteArray getUpgradeHeader(const ByteArray &header);
	ByteArray getConnectionHeader(const ByteArray &header);
	ByteArray getSecWebSocketKeyHeader(const ByteArray &header);
	ByteArray getSecWebSocketProtocolHeader(const ByteArray &header);
	ByteArray getCookieHeader(const ByteArray &header);

	std::vector< ByteArray > splitStringIntoLines(const ByteArray &s);
}// HeaderHelpers namespace






HandshakeHeaders::HandshakeHeaders() : Connection(), Upgrade(), SecWebSocketKey(), SecWebSocketProtocol(), Cookie(){}

HandshakeHeaders::HandshakeHeaders(const HandshakeHeaders& h) noexcept : Connection(h.Connection),
																		Upgrade(h.Upgrade),
																		SecWebSocketKey(h.SecWebSocketKey),
																		SecWebSocketProtocol(h.SecWebSocketProtocol),
																		Cookie(h.Cookie) {} // copy constructor

HandshakeHeadersInterface::~HandshakeHeadersInterface(){}


bool HandshakeHeaders::fillHeaders(const ByteArray &input){
	std::vector< ByteArray > headers = HeaderHelpers::splitStringIntoLines(input);


	for(size_t i =1; i<headers.size(); ++i){ //skip first line
		if ( HeaderHelpers::isUpgradeHeader(headers[i])) {
			Upgrade = HeaderHelpers::getUpgradeHeader(headers[i]);
		}

		else if	( HeaderHelpers::isConnectionHeader(headers[i])) {
			Connection = HeaderHelpers::getConnectionHeader(headers[i]);
		}

		else if	( HeaderHelpers::isSecWebSocketKeyHeader(headers[i])) {
			SecWebSocketKey = HeaderHelpers::getSecWebSocketKeyHeader(headers[i]);
		}

		else if	( HeaderHelpers::isSecWebSocketProtocolHeader(headers[i])) {
			SecWebSocketProtocol = HeaderHelpers::getSecWebSocketProtocolHeader(headers[i]);
		}

		else if	( HeaderHelpers::isCookieHeader(headers[i])) {
			Cookie = HeaderHelpers::getCookieHeader(headers[i]);
		}
		else LOG_INFO("Unused header: "<<headers[i].toString());
	}
	filled = checkHeaders();
	return filled;
}


bool HandshakeHeaders::checkHeaders() const{

	if(Upgrade.size() >= 9){
		if( ! HeaderHelpers::isHeaderValid(Upgrade, "websocket") ) {
			LOG_ERROR("Upgrade header invalid");
			return false;
		}
	}
	else {
		LOG_ERROR("Upgrade.size() < 9");
		return false;
	}

	if(Connection.size() >= 7){
		if(Connection.toString().find("Upgrade") == std::string::npos) {
			LOG_ERROR("Connection header invalid |"<<Connection.toString());
			return false;
		}
	}
	else {
		LOG_ERROR("Connection.size() < 7");
		return false;
	}

	if(SecWebSocketKey.empty()){
		LOG_ERROR("No Sec-WebSocket-Key header");
		return false;
	}
	else if(SecWebSocketProtocol.empty()){
		LOG_ERROR("No Sec-WebSocket-Protocol header");
		return false;
	}
	else if(Cookie.empty()){
		LOG_ERROR("No cookie header");
		return false;
	}
	return true;
}


ByteArray HandshakeHeaders::getConnection() const{
	return Connection;
}


ByteArray HandshakeHeaders::getCookie() const{
	return Cookie;
}


ByteArray HandshakeHeaders::getUpgrade() const{
	return Upgrade;
}


ByteArray HandshakeHeaders::getSecWebSocketKey() const{
	return SecWebSocketKey;
}


ByteArray HandshakeHeaders::getSecWebSocketProtocol() const{
	return SecWebSocketProtocol;
}



bool HandshakeHeaders::areHeadersFilled() const{
	return filled;
}















namespace HeaderHelpers {

bool isUpgradeHeader(const ByteArray &header) {
	return isHeaderValid(header, "Upgrade");
}

bool isConnectionHeader(const ByteArray &header) {
	return isHeaderValid(header, "Connection");
}

bool isSecWebSocketKeyHeader(const ByteArray &header) {
	return isHeaderValid(header, "Sec-WebSocket-Key");
}

bool isSecWebSocketProtocolHeader(const ByteArray &header) {
	return isHeaderValid(header, "Sec-WebSocket-Protocol");
}

bool isCookieHeader(const ByteArray &header) {
	return isHeaderValid(header, "Cookie");
}

ByteArray getUpgradeHeader(const ByteArray &header) {
	return getHeader(header, 9);
}

ByteArray getConnectionHeader(const ByteArray &header) {
	return getHeader(header, 12);
}

ByteArray getSecWebSocketKeyHeader(const ByteArray &header) {
	return getHeader(header, 19);
}

ByteArray getSecWebSocketProtocolHeader(const ByteArray &header) {

	return getHeader(header, 24);
}

ByteArray getCookieHeader(const ByteArray &header) {
	return getHeader(header, 19);
}

bool isHeaderValid(const ByteArray &vec, const std::string &expectedValue){
	if(expectedValue.size() > vec.size())return false;
	return memcmp(&vec[0], &expectedValue[0], expectedValue.size()) == 0;
}

ByteArray getHeader(const ByteArray &header, size_t headerBegin){
	if(header.size()< headerBegin+1) throwInt("header size was less than start index");
	ByteArray ret;
	size_t retSize = header.size()- headerBegin;
	ret.resize(retSize);
	memcpy(&ret[0], &header[headerBegin], retSize);
	return ret;
}

std::vector< ByteArray > splitStringIntoLines(const ByteArray &s) {
	std::vector< ByteArray > elems;
	ByteArray item;
	size_t index =0;
	while(getNextLine(index, s, item)){
		elems.push_back(std::move(item));

	}
	return elems;
}

bool getNextLine(size_t &index, const ByteArray &in, ByteArray &out) {	// copies part of 'in' into 'out' beginning at position 'index' until the end of line.
	out = ByteArray();

	uint8_t carriageReturn = static_cast<uint8_t>('\r');
	uint8_t newLine = static_cast<uint8_t>('\n');
	uint8_t nullChar = static_cast<uint8_t>('\0');

	while(out.empty() && index<in.size()){ // repeatedly call to skip over begining \r or \n
		while(in[index] != carriageReturn && in[index] != newLine && in[index] != nullChar && index < in.size()){
			out.push_back(in[index]);
			++index;
		}
		++index;
	}
	if(index >= in.size()) return false;
	return true;
}

}// HeaderHelpers namespace

