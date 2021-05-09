#include <iostream>
#include <vector>
#include "glm/glm.hpp"

#include "IO.h"
#include "PixelBuffer.h"


int main()
{
	std::cout << "Initial Project is running" << std::endl << std::endl;

	PixelBuffer pixel_buffer(glm::ivec2(640, 480));





	std::ofstream output;
	output.open("./render.ppm", std::ios::out | std::ios::trunc);
	if (!output.is_open())
		return 1;

	std::cout << "Outputting...";
	IO::write_as_PPM(pixel_buffer, output);
	output.close();
	std::cout << "done!" << std::endl;

	return 0;
}