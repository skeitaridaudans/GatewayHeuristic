/*
 * BitmapWriter.cpp
 *
 *  Created on: Dec 16, 2014
 *      Author: kaha
 */

#include <fstream>
#include "BitmapWriter.h"

namespace Bitmap {

void BitmapWriter::writeBMP(bool **map, int width, int height, string fileName)
{
	unsigned char *img = new unsigned char[3*width*height];

	for(int x = 0; x < width; x++)
	{
	    for(int y = 0; y < height; y++)
		{
			unsigned int r, g, b;

			if(map[x][y])
			{
				r = g = b = 255;
			}
			else
			{
				g = 100;
				r = b = 0;
			}

			img[(y * width + x) * 3 + 2] = (unsigned char)(r);
			img[(y * width + x) * 3 + 1] = (unsigned char)(g);
			img[(y * width + x) * 3 + 0] = (unsigned char)(b);
		}
	}

	writeBMP(img, width, height, fileName);

	delete[] img;
}

void BitmapWriter::writeBMP(int **map, int width, int height, int maxValue, string fileName)
{
	writeBMP((unsigned int **)map, (unsigned int)width, (unsigned int)height, (unsigned int)maxValue, fileName);
}

void BitmapWriter::writeBMP(unsigned int **map, unsigned int width, unsigned int height, unsigned int maxValue, string fileName)
{
	unsigned char *img = new unsigned char[3*width*height];

	for(unsigned int x = 0; x < width; x++)
	{
	    for(unsigned int y = 0; y < height; y++)
		{
			unsigned int r, g, b;

			//if(map[x][y] != 0)
			{
				r = g = b = (map[x][y] * 255) / maxValue;
				if(r > 255)
				{
					r = g = b = 255;
				}
			}
			/*else
			{
				g = 100;
				r = b = 0;
			}*/

			img[(y * width + x) * 3 + 2] = (unsigned char)(r);
			img[(y * width + x) * 3 + 1] = (unsigned char)(g);
			img[(y * width + x) * 3 + 0] = (unsigned char)(b);
		}
	}

	writeBMP(img, width, height, fileName);

	delete[] img;
}

void BitmapWriter::writeBMP(unsigned char *img, int width, int height, string fileName)
{
	ofstream fout;
	int filesize = 54 + 3 * width * height;  //w is your image width, h is image height, both int

	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppad[3] = {0,0,0};

	bmpfileheader[ 2] = (unsigned char)(filesize);
	bmpfileheader[ 3] = (unsigned char)(filesize >> 8);
	bmpfileheader[ 4] = (unsigned char)(filesize >> 16);
	bmpfileheader[ 5] = (unsigned char)(filesize >> 24);

	bmpinfoheader[ 4] = (unsigned char)(width);
	bmpinfoheader[ 5] = (unsigned char)(width >> 8);
	bmpinfoheader[ 6] = (unsigned char)(width >> 16);
	bmpinfoheader[ 7] = (unsigned char)(width >> 24);
	bmpinfoheader[ 8] = (unsigned char)(height);
	bmpinfoheader[ 9] = (unsigned char)(height >> 8);
	bmpinfoheader[10] = (unsigned char)(height >> 16);
	bmpinfoheader[11] = (unsigned char)(height >> 24);

	fout.open(fileName.c_str(), ios::out|ios::binary);
	fout.write((char*)bmpfileheader, 14);
	fout.write((char*)bmpinfoheader, 40);
	for(int i = 0; i < height; i++)
	{
		fout.write((char*)(img + (width * (height - i - 1) * 3)), 3 * width);
		fout.write((char*)bmppad, (4 - (width * 3) % 4) % 4);
	}
	fout.close();
}

}
