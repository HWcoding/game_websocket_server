#ifndef SOURCE_IMAGES_IMAGE_H_
#define SOURCE_IMAGES_IMAGE_H_
//#include "source/images/image.h"

#include <cstdint>
#include <vector>

class pixel {
public:
	uint16_t red;
	uint16_t green;
	uint16_t blue;
	uint16_t alpha;
};


class image {
private:
	uint32_t width;
	uint32_t height;
	std::vector<pixel> imageBuffer;
public:
	image(uint32_t _height, uint32_t _width);
	pixel &getAt(uint32_t x, uint32_t y);
	const std::vector<pixel> &getBuffer();
	uint32_t getWidth();
	uint32_t getHeight();
	uint64_t size();
};


#endif /* SOURCE_IMAGES_IMAGE_H_ */
