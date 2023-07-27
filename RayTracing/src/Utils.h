#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "Walnut/Random.h"

typedef glm::vec3 point3;
typedef glm::vec3 vec3;
typedef glm::vec3 color;

namespace Utils {

	constexpr float pi = 3.1415926535897932385;
	constexpr float INF = std::numeric_limits<float>::infinity();

	static inline float deg2rad(float degrees) {
		return degrees * pi / 180.0f;
	}

	// 从glm4 转换到 8*4 32位二进制颜色表示
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}

	static uint32_t PCG_Hash(uint32_t input)
	{
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}

	// 获得 [0, 1)之间的随机数
	static float RandomFloat(uint32_t& seed) 
	{
		seed = PCG_Hash(seed);
		return (float)seed / (float)std::numeric_limits<uint32_t>::max();
	}

	// 获得 min, max 之间的浮点数
	static float RandomFloat(uint32_t& seed, float min, float max)
	{
		return min + (max - min) * RandomFloat(seed);
	}

	// 获得 min, max 之间的整数
	static float RandomInt(uint32_t& seed, int min, int max)
	{
		seed = PCG_Hash(seed);
		return static_cast<int>(RandomFloat(seed, min, max));
	}

	// 随机的单位球表面向量
	static glm::vec3 InUnitSphere(uint32_t& seed)
	{
		return glm::normalize(
			glm::vec3(
				RandomFloat(seed) * 2.0f - 1.0f,
				RandomFloat(seed) * 2.0f - 1.0f,
				RandomFloat(seed) * 2.0f - 1.0f)
		);
	}

	static glm::vec3 InHemiUnitSphere(uint32_t& seed, const glm::vec3& normal)
	{
		auto vec = InUnitSphere(seed);
		if (glm::dot(vec, normal) > 0.0f)
			return vec;
		else
			return -vec;
	}

	static glm::vec3 InUnitDisk(uint32_t seed)
	{
		while (true) {
			auto p = glm::vec3(RandomFloat(seed, -1.0f, 1.0f), RandomFloat(seed, -1.0f, 1.0f), 0.0f);
			if (glm::length(p) >= 1.0) continue;
			return p;
		}
	}

	static inline float Clamp(float x, float min, float max)
	{
		if (x < min)
			return min;
		if (x > max)
			return max;
		return x;
	}

	static bool NearZero(glm::vec3 vec) {
		constexpr auto s = 1e-8;
		return (fabs(vec[0]) < s) && (fabs(vec[1]) < s) && (fabs(vec[2]) < s);
	}

}