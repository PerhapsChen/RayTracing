#pragma once

#include "Utils.h"
#include "Ray.h"
#include "HitRecord.h"

class Material
{
public:
	virtual color emitted(float u, float v, const point3& p) const { return color(0.0f, 0.0f, 0.0f); }
	virtual bool scatter(const Ray& r_in, const HitRecord& record, color& attenuation, Ray& scattered) const = 0;
};