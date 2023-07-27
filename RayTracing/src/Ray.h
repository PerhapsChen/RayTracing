#pragma once

#include <glm/glm.hpp>

struct Ray
{
	glm::vec3 Origin;
	glm::vec3 Direction;
	double time = 0.0;
	Ray(const glm::vec3& ori, const glm::vec3& dir, double t = 0.0):
		Origin(ori), Direction(dir), time(t) {}

	glm::vec3 at(double t) {
		return Origin + Direction * glm::vec3(t, t, t);
	}
};