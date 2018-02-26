#include <iostream>
#include <fstream>
#include <vector>
#include "source/images/png.h"


image createGradientImage(uint32_t height, uint32_t width);
image createGradientImage(uint32_t height, uint32_t width) {
	image data(height, width);
	std::cout<<data.size()<<std::endl;

	uint32_t heightColorMod = 65535/width;
	uint32_t widthColorMod = 65535/height;

	for(uint32_t i = 0; i < height; i++) {
		for(uint32_t j = 0; j < width; j++) {
			data.getAt(j,i).red = static_cast<uint16_t>(i * heightColorMod);
			data.getAt(j,i).green = static_cast<uint16_t>(i * j * widthColorMod/height);
			data.getAt(j,i).blue = static_cast<uint16_t>(j * widthColorMod);
			data.getAt(j,i).alpha = 0xFFFF;
		}
	}
	return data;
}


int main()
{
	std::cout<<"Work in progress"<<std::endl;

	image data = createGradientImage(1000, 1000);

	PngImage parser;

	std::vector<uint8_t> bytes = parser.createPNG(data, true, 16, 0.001f);

	std::fstream myfile("testPNG.png", std::ios::out | std::ios::binary);
	myfile.write((char*)&bytes[0], static_cast<ssize_t>(bytes.size()));
	myfile.close();

	return 0;
}
