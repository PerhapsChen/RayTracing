#include "LambertianMaterial.h"

bool LambertianMaterial::scatter(const Ray& r_in, const HitRecord& record, color& attenuation, Ray& scattered) const
{
	//uint32_t seed = r_in.Direction[1];
	glm::vec3 scatter_direction = record.normal + Walnut::Random::InUnitSphere(); // scatter方向为法线+单位球随机向量

	// 如果scatter方向为0，则按法线scatter.
	if (Utils::NearZero(scatter_direction))
		scatter_direction = record.normal;

	scattered = Ray(record.p, scatter_direction, r_in.time);
	attenuation = albedo->value(record.u, record.v, record.p);

	return true;
}