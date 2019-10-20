#include "bitmap.h"

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
		Bitmap image(1920, 1080, modeType::Noise);
		image.writeBitmap();
	}
	return 0;
}