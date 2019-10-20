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
	pixel()
	{

	}
    pixel(int r, int g, int b) : r(r), g(g), b(b)
    {

    }
    uint8_t b = 0;
	uint8_t g = 0;
	uint8_t r = 0;
};
#define pixelValueCount 3
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
	Header header_;
	infoHeader infoHeader_;
    public:

    public:
    Bitmap(int width, int height, modeType mode = modeType::rgb , std::string filename = "image.bmp") 
    : width(width), height(height), mode(mode), filename(filename)
    {
		noise.SetNoiseType(FastNoise::Simplex);
		noise.SetSeed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

        pixels = new pixel[height * width];
        bytesPerPixel = sizeof(pixel) / sizeof(char);
		stride = (4 - ((3 * width) % 4)) % 4;
        infoHeader_.pixelDataSize = (width * height * pixelValueCount) + (stride * height);
        infoHeader_.width = width;
        infoHeader_.height = -height;
        header_.fileSize = sizeof(header_) + sizeof(infoHeader_) + infoHeader_.pixelDataSize;
        

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
					float xyn = noise.GetNoise(x, y);
					float t = mapValue<float>(xyn, -1.0f, 1.0f, 0.0f, 255.0f);
					auto determin = [](float t)-> int {return  t < 128 ? t / 2 : t * 2; };
					pixels[x + width * y] = (pixel(t, determin(t), determin(t / 3)));
				}
			}
		}
		else
		{

		}
        
    }
    bool writePixelArray()
    {
        fstream_.open(filename, std::ios::app | std::ios::binary | std::ios::out);
        if (fstream_.is_open())
        {
			unsigned int scanline = width * bytesPerPixel;
			for (unsigned int i = 0; i < height; ++i)
			{
				unsigned int offset = (scanline * (height - i - 1));
				const pixel* data_ptr = &pixels[offset / bytesPerPixel];

				fstream_.write(reinterpret_cast<const char*>(data_ptr), sizeof(pixel) * width);
				fstream_.write(stride_data, stride);
			}
        }
        else
            return false;
        
        fstream_.close();
        return true;
    }
    private:
    int width, height;

    int bitsPerPixel = sizeof(int) * 8;
    int bytesPerPixel = 0;
    int stride = 0;
	char stride_data[4] = { 0x000, 0x000 , 0x000  , 0x000 };

    pixel* pixels = nullptr;
	modeType mode;

    std::fstream fstream_;
    std::string filename;

	FastNoise noise;
};