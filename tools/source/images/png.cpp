#include "source/images/png.h"
#include <cstring>
//#include <iostream>
#include <array>
#include <cstdlib>
#include "zlib.h"

/*
 * Helper functions and classes for PngImage
 *
 */
namespace {

/**
 * Singleton that stores a uint32_t [256] array of CRCs of all 8-bit messages used for
 * CRC (Cyclic Redundancy Check) employed in PNG chunks.
 * CRC code adapted from http://www.w3.org/TR/PNG/#D-CRCAppendix
 *
 * Use PngCrc::generateCRC() to generate a crc value for a buffer.
 */

class PngCrc {
private:
	/** array of CRCs of all 8-bit messages */
	uint32_t pngCrcTable[256];

	/** private constructor fills the array with correct values */
	PngCrc(){
		// calculate CRC for each of 256 possible byte values
		// and store in pngCrcTable
		for(uint32_t byteValue = 0; byteValue < 256; byteValue++){
			uint32_t crc = byteValue;
			// loop through for each bit
			for(uint32_t bit = 0; bit < 8; bit++){
				// if least significant bit is 1
				if(crc & 0x00000001L){
					// right shift and mask
					crc = 0xEDB88320L^(crc>>1);
				}
				else{
					crc = crc>>1;
				}
			}
			pngCrcTable[byteValue] = crc;
		}
	}

	/**
	 * Holds the static instance of the class and returns the table in it.
	 *
	 * @return A reference to uint32_t pngCrcTable[256];
	 */
	static const uint32_t (&getPngCrcTable())[256] {
		static PngCrc instance;
		return instance.pngCrcTable;
	}

public:
	/**
	 * Returns the CRC of the bytes from first to last, not including last [first, last).
	 *
	 * @param first An input iterator pointing to the first byte to be checked.
	 *              The type pointed to by first must have a size of 1 byte.
	 *
	 * @param end An input iterator pointing to the byte after the last to be
	 *           checked. The type pointed to by end must have a size of 1 byte.
	 *
	 * @return Cyclic Redundancy Check value for the bytes in buf
	 */

	// only works for iterators to types of size 1
	template <typename InputIterator,
	         class = typename std::enable_if<sizeof(
	             typename std::iterator_traits<InputIterator>::value_type) == 1,
	             InputIterator>::type>

	static uint32_t	generateCRC(InputIterator first, InputIterator end){
		uint32_t crc = 0xFFFFFFFFL;

		for (auto i = first; i != end; ++i) {
			crc = getPngCrcTable()[(crc^(*i))&0xFF]^(crc>>8);
		}
		return crc ^ 0xFFFFFFFFL;
	}

	PngCrc(PngCrc const&) = delete;
	void operator=(PngCrc const&) = delete;
};



void addSig(std::vector<uint8_t> &file){
	uint8_t sig [8];
	sig[0] = 137;	//high bit set
	sig[1] = 80;	//P
	sig[2] = 78;	//N
	sig[3] = 71;	//G
	sig[4] = 13;	//CR
	sig[5] = 10;	//LF
	sig[6] = 26;	//EOF
	sig[7] = 10;	//LF
	file.insert(std::end(file), std::begin(sig), std::end(sig));
}



void addIHDR(std::vector<uint8_t> &file, uint32_t height, uint32_t width, uint8_t bitDepth, uint8_t colorType){

	uint8_t IHDR[25];
	//add chunk length
	uint32_t length = 13;
	auto convertChar = reinterpret_cast<uint8_t *> (&length);
	IHDR[0] = convertChar[3]; //reverse order for Big Endian
	IHDR[1] = convertChar[2];
	IHDR[2] = convertChar[1];
	IHDR[3] = convertChar[0];
	//add chunk type
	IHDR[4] = 73;	//I
	IHDR[5] = 72;	//H
	IHDR[6] = 68;	//D
	IHDR[7] = 82;	//R
	//add width
	convertChar = reinterpret_cast<uint8_t *> (&width);
	IHDR[8] = convertChar[3]; //reverse order for Big Endian
	IHDR[9] = convertChar[2];
	IHDR[10] = convertChar[1];
	IHDR[11] = convertChar[0];
	//add height
	convertChar = reinterpret_cast<uint8_t *> (&height);
	IHDR[12] = convertChar[3]; //reverse order for Big Endian
	IHDR[13] = convertChar[2];
	IHDR[14] = convertChar[1];
	IHDR[15] = convertChar[0];

	IHDR[16] = bitDepth;
	IHDR[17] = colorType;
	IHDR[18] = 0; //CompressionMethod
	IHDR[19] = 0; //FilterMethod
	IHDR[20] = 0; //InterlaceMethod

	//calculate and add Cyclic Redundancy Check skipping the length
	auto crcBegin = std::begin(IHDR) + 4;
	auto crcEnd = std::begin(IHDR) + 21;
	uint32_t CRC = PngCrc::generateCRC(crcBegin, crcEnd);
	convertChar = reinterpret_cast<uint8_t *> (&CRC);
	IHDR[21] = convertChar[3]; //reverse order for Big Endian
	IHDR[22] = convertChar[2];
	IHDR[23] = convertChar[1];
	IHDR[24] = convertChar[0];
	file.insert(std::end(file), std::begin(IHDR), std::end(IHDR));
}


/*
void addPLTE(std::vector<uint8_t> &file, std::vector<uint8_t> &table){
	uint32_t tableSize = static_cast<uint32_t>(table.size());
	std::vector<uint8_t> PLTE(tableSize+12);
	//add chunk length
	uint8_t* convertChar = reinterpret_cast<uint8_t *> (&tableSize);
	PLTE[0] = convertChar[3]; //reverse order for Big Endian
	PLTE[1] = convertChar[2];
	PLTE[2] = convertChar[1];
	PLTE[3] = convertChar[0];
	//add chunk type
	PLTE[4] = 80;	//P
	PLTE[5] = 76;	//L
	PLTE[6] = 84;	//T
	PLTE[7] = 69;	//E
	for(unsigned int i=0; i<tableSize; i++){
		PLTE[i+8]= table[i];//should already be in Big Endian order
	}
	//calculate and add Cyclic Redundancy Check skipping the length
	auto crcBegin = std::begin(PLTE) + 4;
	auto crcEnd = std::begin(PLTE) + 8 + tableSize;
	uint32_t CRC = PngCrc::generateCRC(crcBegin, crcEnd);
	convertChar = reinterpret_cast<uint8_t *> (&CRC);
	PLTE[tableSize+8] = convertChar[3]; //reverse order for Big Endian
	PLTE[tableSize+9] = convertChar[2];
	PLTE[tableSize+10] = convertChar[1];
	PLTE[tableSize+11] = convertChar[0];
	file.insert(std::end(file), std::begin(PLTE), std::end(PLTE));
}
*/


void addIDAT(std::vector<uint8_t> &file, std::vector<uint8_t> &data){
	auto dataSize = static_cast<uint32_t>(data.size());
	std::vector<uint8_t> IDAT(dataSize+12);
	//add chunk length
	auto convertChar = reinterpret_cast<uint8_t *> (&dataSize);
	IDAT[0] = convertChar[3]; //reverse order for Big Endian
	IDAT[1] = convertChar[2];
	IDAT[2] = convertChar[1];
	IDAT[3] = convertChar[0];
	//add chunk type
	IDAT[4] = 73;	//I
	IDAT[5] = 68;	//D
	IDAT[6] = 65;	//A
	IDAT[7] = 84;	//T
	for(unsigned int i=0; i<dataSize; i++){
		IDAT[i+8]= data[i];
	}
	//calculate and add Cyclic Redundancy Check skipping the length
	auto crcBegin = std::begin(IDAT) + 4;
	auto crcEnd = std::begin(IDAT) + 8 + dataSize;
	uint32_t CRC = PngCrc::generateCRC(crcBegin, crcEnd);
	convertChar = reinterpret_cast<uint8_t *> (&CRC);
	IDAT[dataSize+8] = convertChar[3]; //reverse order for Big Endian
	IDAT[dataSize+9] = convertChar[2];
	IDAT[dataSize+10] = convertChar[1];
	IDAT[dataSize+11] = convertChar[0];

	file.insert(std::end(file), std::begin(IDAT), std::end(IDAT));
}

void addIEND(std::vector<uint8_t> &file){
	uint8_t IEND[12];
	//add chunk length
	uint32_t length = 0;
	auto convertChar = reinterpret_cast<uint8_t *> (&length);
	IEND[0] = convertChar[3]; //reverse order for Big Endian
	IEND[1] = convertChar[2];
	IEND[2] = convertChar[1];
	IEND[3] = convertChar[0];
	//add chunk type
	IEND[4] = 73;	//I
	IEND[5] = 69;	//E
	IEND[6] = 78;	//N
	IEND[7] = 68;	//D
	//calculate and add Cyclic Redundancy Check skipping the length
	auto crcBegin = std::begin(IEND) + 4;
	auto crcEnd = std::begin(IEND) + 8;
	uint32_t CRC = PngCrc::generateCRC(crcBegin, crcEnd);
	convertChar = reinterpret_cast<uint8_t *> (&CRC);
	IEND[8] = convertChar[3]; //reverse order for Big Endian
	IEND[9] = convertChar[2];
	IEND[10] = convertChar[1];
	IEND[11] = convertChar[0];

	file.insert(std::end(file), std::begin(IEND), std::end(IEND));
}



bool isGrayScale(image &data, uint16_t accuracy){
	const std::vector<pixel> &imageBuffer = data.getBuffer();
	auto length = static_cast<uint32_t>(data.size());
	uint32_t deviation = 0;
	uint32_t diff = 0;
	for(uint32_t i = 0; i < length; i++){
		diff = static_cast<uint32_t>(abs(imageBuffer[i].red-imageBuffer[i].green));
		if(diff > deviation) {
			 deviation = diff;
		}
		diff = static_cast<uint32_t>(abs(imageBuffer[i].red-imageBuffer[i].blue));
		if(diff > deviation) {
			deviation = diff;
		}
		diff = static_cast<uint32_t>(abs(imageBuffer[i].blue-imageBuffer[i].green));
		if(diff > deviation) {
			deviation = diff;
		}
	}
	if(deviation < accuracy)return true;
	return false;
}




/*bool isIndexed(std::vector<uint16_t> &data, uint16_t accuracy){
	(void)data;
	(void)accuracy;
	//unsigned int length = data.size();
	return false;
}*/




/*void createTable(std::vector<uint8_t> &file, std::vector<uint16_t> &data){
	(void) file;
	(void) data;
	//const unsigned int dataSize = data.size();
	return;
}*/


void compressBuffer(std::vector<uint8_t> &data){
	uint64_t dataSize = data.size();
	uint64_t outputSize =compressBound(dataSize);
	std::vector<uint8_t> compressed;
	compressed.resize(outputSize);
	int res = compress(&compressed[0], &outputSize, (const uint8_t*) &data[0], dataSize);
	if (res != Z_OK){ //failed.
		throw std::runtime_error("zlib compress failed in PngImage::compress");
		return;
	}
	//needed because compress shrinks outputSize
	compressed.resize(outputSize);
	data = compressed;
	return;
}


uint64_t calculateFilterCost(std::vector<uint8_t> &data){

	int64_t total = 0;
	for(uint64_t i = 1; i < data.size(); i++) {
		// Consider the output bytes as signed differences for this test
		total += (*(reinterpret_cast<int8_t*>(&data[i])));
	}
	return static_cast<uint64_t>(abs(static_cast<int32_t>(total)));
}


void scanfilter0(std::vector<uint8_t> &filtered, std::vector<uint8_t> &Data, uint64_t start, uint64_t end){
	filtered[0] = 0; // first byte indicates filter type
	for(uint64_t dataIndex = start + 1, filterIndex = 1; dataIndex <= end; dataIndex++, filterIndex++){
		filtered[filterIndex] = Data[dataIndex];
	}
}

void scanfilter1(std::vector<uint8_t> &filtered, std::vector<uint8_t> &Data, uint64_t start, uint64_t end,  uint8_t pixelWidth){
	uint8_t left = 0;
	filtered[0] = 1; // first byte indicates filter type
	for(uint64_t dataIndex = start + 1, filterIndex = 1; dataIndex <= end; dataIndex++, filterIndex++){
		if(dataIndex < start + pixelWidth) {
			left = 0;
		}
		else{
			left = Data[dataIndex - pixelWidth];
		}
		filtered[filterIndex] = static_cast<uint8_t>(Data[dataIndex] - left);
	}
}

void scanfilter2(std::vector<uint8_t> &filtered, std::vector<uint8_t> &Data, uint64_t start, uint64_t end){
	uint64_t length = (end - start + 1);
	uint8_t above = 0;
	filtered[0] = 2; // first byte indicates filter type
	for(uint64_t dataIndex = start + 1, filterIndex = 1; dataIndex <= end; dataIndex++,filterIndex++){
		if(dataIndex < length) {
			above = 0;
		}
		else{
			above = Data[dataIndex-length];
		}
		filtered[filterIndex] = static_cast<uint8_t>(Data[dataIndex] - above);
	}
}

void scanfilter3(std::vector<uint8_t> &filtered, std::vector<uint8_t> &Data, uint64_t start, uint64_t end,  uint8_t pixelWidth){
	uint64_t length = (end - start + 1);
	uint8_t left = 0;
	uint8_t above = 0;
	filtered[0] = 3; // first byte indicates filter type
	for(uint64_t dataIndex = start + 1, filterIndex = 1; dataIndex <= end; dataIndex++, filterIndex++){
		if(dataIndex < start + pixelWidth) {
			left = 0;
		}
		else{
			left = Data[dataIndex - pixelWidth];
		}
		if(dataIndex < length) {
			above = 0;
		}
		else{
			above = Data[dataIndex-length];
		}
		filtered[filterIndex] = static_cast<uint8_t>(Data[dataIndex] - (left + above)/2);
	}
}

void scanfilter4(std::vector<uint8_t> &filtered, std::vector<uint8_t> &Data, uint64_t start, uint64_t end,  uint8_t pixelWidth){
	uint64_t length = (end - start + 1);
	uint8_t left = 0;
	uint8_t above = 0;
	uint8_t aboveLeft = 0;
	filtered[0] = 4; // first byte indicates filter type
	for(uint64_t dataIndex = start + 1, filterIndex = 1; dataIndex <= end; dataIndex++, filterIndex++){
		if(dataIndex < start + pixelWidth) {
			left = 0;
		}
		else{
			left = Data[dataIndex - pixelWidth];
		}
		if(dataIndex < length) {
			above = 0;
		}
		else{
			above = Data[dataIndex - length];
		}
		if(dataIndex < length || dataIndex < start + pixelWidth) {
			aboveLeft = 0;
		}
		else{
			aboveLeft = Data[dataIndex - length - pixelWidth];
		}

		int cost =  left + above - aboveLeft;
		int leftCost = abs(cost - left);
		int aboveCost = abs(cost - above);
		int aboveLeftCost = abs(cost - aboveLeft);
		int cheapest;

		if (leftCost <= aboveCost && leftCost <= aboveLeftCost) {
			cheapest = left;
		}
		else if(aboveCost <= aboveLeftCost) {
			cheapest = above;
		}
		else {
			cheapest = aboveLeft;
		}

		filtered[filterIndex] = static_cast<uint8_t>(Data[dataIndex] - cheapest);
	}
}


void filterChunk(std::vector<uint8_t> &filtered, std::vector<uint8_t> &Data, uint64_t start, uint64_t end,  uint8_t pixelWidth){
	uint64_t length = end - start+1;
	std::vector<uint8_t> tempfilteredA(length);
	std::vector<uint8_t> tempfilteredB(length);
	std::vector<uint8_t> tempfilteredC(length);
	std::vector<uint8_t> tempfilteredD(length);
	std::vector<uint8_t> tempfilteredE(length);

	scanfilter0(tempfilteredA, Data, start, end);
	scanfilter1(tempfilteredB, Data, start, end, pixelWidth);
	scanfilter2(tempfilteredC, Data, start, end);
	scanfilter3(tempfilteredD, Data, start, end, pixelWidth);
	scanfilter4(tempfilteredE, Data, start, end, pixelWidth);

	std::vector<uint8_t>* cheapestfiltered = &tempfilteredA;

	if(start < length){//first line
		cheapestfiltered = &tempfilteredB;
	}
	else{
		auto minCost = calculateFilterCost(tempfilteredA);
		auto cost = calculateFilterCost(tempfilteredB);
		if(cost < minCost) {
			cheapestfiltered = &tempfilteredB;
			minCost = cost;
		}
		cost = calculateFilterCost(tempfilteredC);
		if(cost < minCost) {
			cheapestfiltered = &tempfilteredC;
			minCost = cost;
		}
		cost = calculateFilterCost(tempfilteredD);
		if(cost < minCost) {
			cheapestfiltered = &tempfilteredD;
			minCost = cost;
		}
		cost = calculateFilterCost(tempfilteredE);
		if(cost < minCost) {
			cheapestfiltered = &tempfilteredE;
		}
	}

	memcpy(&filtered[start],&(*cheapestfiltered)[0],sizeof(uint8_t)*length);
}


}// anonomous namespace






//vector version
void PngImage::convertBitDepth(std::vector<uint8_t> &buffer, image &data, uint8_t pixelWidth){//TODO grayscale conversion
	auto pixelCount = static_cast<uint32_t>(data.size());
	uint8_t* convertChar;
	uint32_t i =0;
	uint32_t j =0;
	uint32_t k =0;
	uint32_t offset =0;

	if(bitDepth == 16){
		uint32_t widthP1 = (width*pixelWidth)+1;
		uint64_t size = 2*pixelCount*pixelWidth+height;
		buffer.resize(size);
		std::vector<uint8_t> filtered(size);
		for(i =0; i<height; i++){
			offset = i*(widthP1);

			buffer[offset] = 0;//filler for filter type
			for(j = 1, k= 0; j<widthP1; k++){
				if(colorType == PNG_TRUECOLOR || colorType == PNG_TRUECOLOR_A) {
					convertChar = reinterpret_cast<uint8_t *>(&(data.getAt(k,i).red));
					buffer[j+offset] = convertChar[1];
					buffer[j+offset+1] = convertChar[0];

					convertChar = reinterpret_cast<uint8_t *>(&(data.getAt(k,i).green));
					buffer[j+offset+2] = convertChar[1];
					buffer[j+offset+3] = convertChar[0];

					convertChar = reinterpret_cast<uint8_t *>(&(data.getAt(k,i).blue));
					buffer[j+offset+4] = convertChar[1];
					buffer[j+offset+5] = convertChar[0];
					j+=6;
				} else if(colorType == PNG_GRAYSCALE || colorType == PNG_GRAYSCALE_A ) {
					convertChar = reinterpret_cast<uint8_t *>(&(data.getAt(k,i).red));
					buffer[j+offset] = convertChar[1];
					buffer[j+offset+1] = convertChar[0];
					j+=2;
				}

				if(colorType == PNG_GRAYSCALE_A ||
					colorType == PNG_TRUECOLOR_A) {
					convertChar = reinterpret_cast<uint8_t *>(&(data.getAt(k,i).alpha));
					buffer[j+offset] = convertChar[1];
					buffer[j+offset+1] = convertChar[0];
					j+=2;
				}
			}
			filterChunk(filtered, buffer, offset, offset+widthP1-1, pixelWidth);
		}
		buffer = filtered;
		return;
	}

	else if(bitDepth == 8){
		uint32_t widthP1 = (width*pixelWidth)+1;
		uint64_t size = pixelCount*pixelWidth+height;
		buffer.resize(size);
		std::vector<uint8_t> filtered(size);
		for(i =0; i<height; i++){
			offset = i*(widthP1);

			buffer[offset] = 0;//filler for filter type
			for(j = 1, k= 0; j<widthP1; k++){
				if(colorType == PNG_TRUECOLOR || colorType == PNG_TRUECOLOR_A) {
					buffer[j+offset] = static_cast<uint8_t>(data.getAt(k,i).red>>8);
					buffer[j+offset+1] = static_cast<uint8_t>(data.getAt(k,i).green>>8);
					buffer[j+offset+2] = static_cast<uint8_t>(data.getAt(k,i).blue>>8);
					j+=3;
				}else if(colorType == PNG_GRAYSCALE || colorType == PNG_GRAYSCALE_A ) {
					buffer[j+offset] = static_cast<uint8_t>((data.getAt(k,i).red>>8));
					j++;
				}
				if(colorType == PNG_GRAYSCALE_A ||
					colorType == PNG_TRUECOLOR_A) {
					buffer[j+offset] = static_cast<uint8_t>(data.getAt(k,i).alpha>>8);
					j++;
				}
			}
			filterChunk(filtered, buffer, offset, offset+widthP1-1, pixelWidth);
		}
		buffer = filtered;
		return;
	}
	return;
}


void PngImage::createDataChunk(std::vector<uint8_t> &file, image &data, uint8_t pixelWidth){
	//createTable(file,data, length);
	std::vector<uint8_t> converted;
	convertBitDepth(converted, data, pixelWidth);
	compressBuffer(converted);
	addIDAT(file, converted);
}


std::vector<uint8_t> PngImage::createPNG(image &data, bool Alpha, uint8_t _bitDepth, float _accuracy){
	std::vector<uint8_t> file;
	uint32_t _width = data.getWidth();
	uint32_t _height = data.getHeight();
	if(_accuracy > 1 || _accuracy < 0) {
		throw std::runtime_error("accuracy must be <= 1  and >= 0 PngImage::createPNG");
	}
	if(_width == 0 || _height == 0){
		throw std::runtime_error("width and height must not be zero PngImage::createPNG");
	}

	//temp.................................................................................................//
	if(_bitDepth != 8 && _bitDepth != 16 ){
		throw std::runtime_error("bitDepth must be 8 or 16 PngImage::createPNG");
	}
	//.....................................................................................................//

	if(!(_bitDepth == 1 || _bitDepth == 2 || _bitDepth == 4 || _bitDepth == 8 || _bitDepth == 16)){
		throw std::runtime_error("bitDepth must be 1,2,4,8,or 16 PngImage::createPNG");
	}
	if(_bitDepth<8 && Alpha) {
		throw std::runtime_error("Alpha cannot be used with specified bitDepth PngImage::createPNG");
	}
	uint64_t length = _width*_height;

	if(data.size()!= length) {
		throw std::runtime_error("data size does not match width*height*bitDepth PngImage::createPNG");
	}
	width = _width;
	height = _height;
	bitDepth = _bitDepth;
	auto accuracy = static_cast<uint16_t>(65535 *_accuracy);
	bool grayscale = isGrayScale(data, accuracy);
	//if(bitDepth<8)bitDepth = 8; //TODO enable bit depths below 8
	//if(bitDepth<8){
	//	if(grayscale)colorType = PNG_GRAYSCALE;
	//	else colorType = PNG_GRAYSCALE;//PNG_INDEXCOLOR;
	//}
	//else
	if(bitDepth == 16){
		if(Alpha){
			if(grayscale)colorType = PNG_GRAYSCALE_A;
			else colorType = PNG_TRUECOLOR_A;
		}
		else{
			if(grayscale)colorType = PNG_GRAYSCALE;
			else colorType = PNG_TRUECOLOR;
		}
	}
	else{
		if(Alpha){
			if(grayscale)colorType = PNG_GRAYSCALE_A;
			else colorType = PNG_TRUECOLOR_A;
		}
		else if(grayscale)colorType = PNG_GRAYSCALE;
		//else if(isIndexed(data, accuracy))colorType = PNG_INDEXCOLOR;
		else colorType = PNG_TRUECOLOR;
	}
	uint8_t pixelWidth = 0;
	if(colorType == PNG_GRAYSCALE){
		pixelWidth = static_cast<uint8_t>(bitDepth/8);
	}
	else if(colorType == PNG_GRAYSCALE_A){
		pixelWidth = static_cast<uint8_t>(bitDepth/4);
	}
	else if(colorType == PNG_TRUECOLOR_A){
		pixelWidth = static_cast<uint8_t>(bitDepth/2);
	}
	else if(colorType == PNG_TRUECOLOR){
		pixelWidth = static_cast<uint8_t>(3*bitDepth/8);
	}
	else{
		throw std::runtime_error("bad color type PngImage::createPNG");
	}

	//check that bit depth is 1,2,4,8, or 16
	if(!(bitDepth==1 || bitDepth==2 || bitDepth==4 || bitDepth==8 || bitDepth==16)) {
		throw std::runtime_error("bitDepth invalid in PngImage::addIHDR");
	}
	//check color type is a recognized value
	if(!(colorType == PngImage::PNG_TRUECOLOR ||
		colorType == PngImage::PNG_TRUECOLOR_A ||
		colorType == PngImage::PNG_INDEXCOLOR ||
		colorType == PngImage::PNG_GRAYSCALE ||
		colorType == PngImage::PNG_GRAYSCALE_A)) {
		throw std::runtime_error("colorType invalid PngImage::addIHDR");
	}
	//check that bit depth matches color type
	if(colorType == PngImage::PNG_INDEXCOLOR && bitDepth>8) {
		throw std::runtime_error("ColorType does not match bitDepth PngImage::addIHDR");
	}
	if((colorType == PngImage::PNG_TRUECOLOR ||
		colorType == PngImage::PNG_GRAYSCALE_A ||
		colorType == PngImage::PNG_TRUECOLOR_A) && bitDepth<8) {
		throw std::runtime_error("colorType does not match bitDepth PngImage::addIHDR");
	}
	//check width and height
	if(!width) {
		throw std::runtime_error("width cannot be zero PngImage::addIHDR");
	}
	if(!height) {
		throw std::runtime_error("height cannot be zero PngImage::addIHDR");
	}

	addSig(file);
	addIHDR(file, height, width, bitDepth, colorType);
	createDataChunk(file, data, pixelWidth);
	addIEND(file);

	return file;
}
