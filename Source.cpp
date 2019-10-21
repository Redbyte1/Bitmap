#include "Bitmap.h"

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		std::vector<std::string> arguments;
		for (int i = 0; i < argc; i++)
		{
			arguments.push_back(std::string(argv[i + 1]));
		}
		//Bitmap image(arguments[0], arguemnts);
	}
	else
	{
		Bitmap image(1920, 1080, std::string("image.bmp"), modeType::Noise);
		image.writeBitmap();

		//do this for frames
		// std::string filename("image");
		// for (int z = 0; z < 10; z++)
		// {
		// 	Bitmap image(1920, 1080, z, 1, filename + std::to_string(z) + std::string(".bmp"), modeType::Noise);
		// 	image.writeBitmap();
		// }
		
	}
	return 0;
}
