#include <iostream>
#include <vector>
#include "glm/glm.hpp"

#include "IO.h"
#include "PixelBuffer.h"

struct Camera
{
	glm::dvec3 position, left_bottom, horizontal, vertical;
	double focal_length;

	Camera(const glm::dvec3& position, const glm::dvec3& target, const PixelBuffer& pixel_buffer)
		: position(position), focal_length(1)
	{
		glm::dvec3 up(0, 1, 0);
		auto forward = glm::normalize(target - position);
		auto right = glm::normalize(glm::cross(forward, up));
		up = glm::normalize(glm::cross(forward, -right));

		horizontal = right * (double(pixel_buffer.dimensions.x) / double(pixel_buffer.dimensions.y));
		vertical = up * 1.;
		left_bottom = position + forward * focal_length - horizontal * 0.5 - vertical * 0.5;
	}

	glm::dvec3 raster_to_world(const glm::dvec2& r)
	{
		return left_bottom + r.x * horizontal + r.y * vertical;
	}
};

struct Ray
{
	glm::dvec3 origin, direction;

	Ray(const glm::dvec3& origin, const glm::dvec3& direction)
		: origin(origin), direction(direction)
	{}

	glm::dvec3 at(double t) const
	{
		return origin + t * direction;
	}
};

struct Sphere
{
	glm::dvec3 center;
	double radius;

	Sphere(const glm::dvec3& c, double r)
		: center(c), radius(r)
	{}

	bool hit(const Ray& ray) const
	{
		glm::dvec3 oc = ray.origin - center;
		auto a = glm::dot(ray.direction, ray.direction);
		auto b = 2.0 * glm::dot(oc, ray.direction);
		auto c = glm::dot(oc, oc) - radius * radius;
		auto discriminant = b * b - 4.0 * a * c;
		return (discriminant >= 0);
	}
};

int main()
{
	std::cout << "Completed Project is running" << std::endl << std::endl;

	PixelBuffer pixel_buffer(glm::ivec2(640, 480));

	Camera camera(glm::dvec3(0, 5, 5), glm::dvec3(0, 0, 0), pixel_buffer);

	std::vector<Sphere> spheres
	{
		{glm::dvec3(0, 0, 0), 1},
		{glm::dvec3(2, 0, 0), 1},
		{glm::dvec3(2, 2, 0), 1},

		{glm::dvec3(-3, 0, 0), 1.5},
	};


	for (int y = 0; y < pixel_buffer.dimensions.y; ++y)
	{
		for (int x = 0; x < pixel_buffer.dimensions.x; ++x)
		{
			glm::dvec2 uv(x, y);
			uv /= pixel_buffer.dimensions; // Normalization

			glm::dvec3 color(0);

			Ray ray(camera.position, glm::normalize(camera.raster_to_world(uv) - camera.position));

			bool hit_any = false;
			for (const auto& sphere : spheres)
				hit_any |= sphere.hit(ray);

			if (hit_any)
			{
				color += 1;
			}
			else
			{
				color.r += glm::abs(ray.direction.x);
				color.b += glm::abs(ray.direction.y);
			}

			pixel_buffer.set(x, pixel_buffer.dimensions.y - y - 1, color);
		}
	}


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