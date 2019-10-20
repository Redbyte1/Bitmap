#pragma once
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
pixel rgb255(pixel color)
{

	if (color.r > 0 && color.b == 0) {
		color.r--;
		color.g++;
	}
	if (color.g > 0 && color.r == 0) {
		color.g--;
		color.b++;
	}
	if (color.b > 0 && color.g == 0) {
		color.r++;
		color.b--;
	}
	return color;

}
class Bitmap
{
    public:
    Bitmap(int width, int height, std::string filename = "image.bmp", modeType mode = modeType::rgb) 
    : width(width), height(height), mode(mode), filename(filename)
    {
       setup();
    }
    Bitmap(int width, int height, int z, int seed, std::string filename, modeType mode = modeType::rgb) 
    : width(width), height(height), mode(mode), z(z), seed(seed), filename(filename)
    {
       setup();
    }
    bool writeBitmap()
    {
        writeHeaders();
        generatePixelArray();
        writePixelArray();

        return true;
    }
    ~Bitmap()
    {
        if(pixels)
        {
            delete[] pixels;
            pixels = nullptr;
        }
    }
    private:
    bool writeHeaders()
    {
        fstream_.open(filename, std::ios::out|std::ios::trunc|std::ios::binary);
        if (fstream_.is_open())
        {
            fstream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
            fstream_.write(reinterpret_cast<char*>(&infoHeader_), sizeof(infoHeader_));
        }
        else
            return false;
        fstream_.close();
        return true;
    }
    void generatePixelArray()
    {
        float xyz;
		if (mode == modeType::rgb)
		{
			pixel color(255, 0, 0);
			int i = 0;
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					color = rgb255(color);
					pixels[x + width * y] = color;
				}
			}
		}
		else if (mode == modeType::Noise)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
                    if (z == 0)
					    xyz = noise.GetNoise(x, y);
                    else
                        xyz = noise.GetNoise(x, y, z);
                    
                    
					float t = mapValue<float>(xyz, -1.0f, 1.0f, 0.0f, 255.0f);
					auto determin = [](float t)-> int {return  t < 128 ? t / 2 : t * 2; };
					pixels[x + width * y] = (pixel(t, determin(t), determin(t / 3)));
				}
			}
		}
		else
		{
            //Uhh
		}
        
    }
    bool writePixelArray()
    {
        fstream_.open(filename, std::ios::app | std::ios::binary | std::ios::out);
        if (fstream_.is_open())
        {
            //It's easiest to write a whole scanline at a time then append padding
			for (unsigned int i = 0; i < height; ++i)
			{
				unsigned int scanlineOffset = (scanline * (height - i - 1)) / bytesPerPixel;
				const pixel* currentScanline = &pixels[scanlineOffset];

				fstream_.write(reinterpret_cast<const char*>(currentScanline), scanline);
				fstream_.write(stride_data, stride);
			}
        }
        else
            return false;
        
        fstream_.close();
        return true;
    }
    //Only call in constructor
    void setup()
    {
         //Set FastNoise Type and Seed
		noise.SetNoiseType(FastNoise::Simplex);
        if (seed == 0)
		    noise.SetSeed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        else
            noise.SetSeed(seed);
        
        //Allocate an array for pixel data
        if (!pixels)
            pixels = new pixel[height * width];
        
        bytesPerPixel = sizeof(pixel) / sizeof(pixelType);

        scanline = width * bytesPerPixel;

        //determin how much padding is needed per scanline
		stride = (4 - ((3 * width) % 4)) % 4;
        infoHeader_.width = width;
        //Inverting height flips the image to bottom->top
        infoHeader_.height = -height;

        infoHeader_.pixelDataSize = (width * height * pixelValueCount) + (stride * height);
        header_.fileSize = sizeof(header_) + sizeof(infoHeader_) + infoHeader_.pixelDataSize;
    }
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