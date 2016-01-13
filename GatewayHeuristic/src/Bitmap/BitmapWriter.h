/*
 * BitmapWriter.h
 *
 *  Created on: Dec 16, 2014
 *      Author: kaha
 */

#ifndef BITMAPWRITER_H_
#define BITMAPWRITER_H_

#include <string>

using namespace std;

namespace Bitmap {

class BitmapWriter {
private:
	static void writeBMP(unsigned char *img, int width, int height, string fileName);

public:
	//BitmapWriter();
	virtual ~BitmapWriter() {};

	static void writeBMP(bool **map, int width, int height, string fileName);
	static void writeBMP(int **map, int width, int height, int maxValue, string fileName);
	static void writeBMP(unsigned int **map, unsigned int width, unsigned int height, unsigned int maxValue, string fileName);
};

}

#endif /* BITMAPWRITER_H_ */
