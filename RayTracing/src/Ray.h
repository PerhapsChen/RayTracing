#pragma once

#include <glm/glm.hpp>

struct Ray
{
	glm::vec3 Origin;
	glm::vec3 Direction;
	float time;
	Ray() = default;

	Ray(const glm::vec3& ori, const glm::vec3& dir, float t):
		Origin(ori), Direction(dir), time(t) {}

	glm::vec3 at(float t) const
	{
		return Origin + t * Direction;
	}
};