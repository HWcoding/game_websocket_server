#include "source/images/image.h"

image::image(uint32_t _height, uint32_t _width) :
	width(_width),
	height(_height),
	imageBuffer(_width * _height)
{}

pixel &image::getAt(uint32_t x, uint32_t y){
	return imageBuffer[width * y + x];
}
const std::vector<pixel> &image::getBuffer(){
	return imageBuffer;
}
uint32_t image::getWidth(){
	return width;
}
uint32_t image::getHeight(){
	return height;
}

uint64_t image::size() {
	return imageBuffer.size();
}