#ifndef SOURCE_IMAGES_PNG_H_
#define SOURCE_IMAGES_PNG_H_
//#include "source/images/png.h"

#include <cstdint>
#include <vector>
#include "source/images/image.h"

class PngImage {
	uint32_t height {0};
	uint32_t width {0};
	uint8_t bitDepth {0};
	uint8_t colorType {127};

	void convertBitDepth(std::vector<uint8_t> &buffer, image &data, uint8_t pixelWidth);
	void createDataChunk(std::vector<uint8_t> &file, image &data, uint8_t pixelWidth);

public:
	// color type
	static const uint8_t PNG_GRAYSCALE  = 0;    //1, 2, 4, 8, 16    Each pixel is a greyscale sample
	static const uint8_t PNG_TRUECOLOR  = 2;    //8, 16             Each pixel is an R,G,B triple
	static const uint8_t PNG_INDEXCOLOR = 3;    //1, 2, 4, 8        Each pixel is a palette index; a PLTE chunk shall appear.
	static const uint8_t PNG_GRAYSCALE_A = 4;   //8, 16             Each pixel is a greyscale sample followed by an alpha sample.
	static const uint8_t PNG_TRUECOLOR_A = 6;   //8, 16             Each pixel is an R,G,B triple followed by an alpha sample.

	std::vector<uint8_t> createPNG(image &data, bool Alpha,
	                             uint8_t _bitDepth, float _accuracy);

	PngImage() = default;
};


#endif /* SOURCE_IMAGES_PNG_H_ */
