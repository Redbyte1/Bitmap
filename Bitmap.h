#pragma once

#ifndef Bitmap_h
#define Bitmap_h

#include "FastNoise.h"
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <chrono>
#include <random>

template<typename T = float>
T mapValue(T value, T minIn, T maxIn, T minOut, T maxOut)
{
	return ((value - minIn) / (maxIn - minIn))
		* (maxOut - minOut) + minOut;
}

enum modeType
{
	Noise = 0,
	rgb = 1
};

#pragma pack(2)
struct Header
{
    char Signature[2] = {'B', 'M'};
    int fileSize = 0;
    int reserved1 = 0x0000;
    //Length of both header's since there's no color table
    int dataOffset = 0x36;
};
#pragma pack(2)
struct infoHeader
{
    int sizeOfHeader = 40;
    int width = 0;
    int height = 0;
    short int plane = 1;
    short int BPP = 24;
    int compression = 0;
    int pixelDataSize = 0;
    int PPMh = 0;
    int PPMv = 0;
	int colors = 0;
	int importantColors = 0;
};
#pragma pack(2)
struct pixel
{
	pixel(){}
    pixel(int r, int g, int b) : r(r), g(g), b(b){}
    uint8_t b = 0;
	uint8_t g = 0;
	uint8_t r = 0;
};
//in the event this pixel struct needs to be expanded beyond
//the current number of variables
#define pixelValueCount 3
#define pixelType uint8_t

#pragma pack(8)
pixel rgb255(pixel color);

class Bitmap
{
    public:
    Bitmap(int width, int height, std::string filename, modeType mode);
    Bitmap(int width, int height, int z, int seed, std::string filename, modeType mode);
	~Bitmap();
    bool writeBitmap();
    private:
    bool writeHeaders();
    void generatePixelArray();
    bool writePixelArray();
    //Only call in constructor
    void setup();
    private:
    int width, height;

    int bytesPerPixel = 0;
    unsigned int scanline = 0;
    //Stride is the padding that makes each scanline
    //divisible by 4
    int stride = 0;
	char stride_data[4] = { 0x000, 0x000 , 0x000  , 0x000 };

    pixel* pixels = nullptr;
    //This is for easy selection of pixel generation mode
    //Just for testing but makes it easy to add new ones without
    //lots of extra work
	modeType mode;

    //The main File Stream object
    std::fstream fstream_;
    std::string filename;

    int seed = 0;
    int z = 0;

    Header header_;
	infoHeader infoHeader_;

	FastNoise noise;
};

#endif
