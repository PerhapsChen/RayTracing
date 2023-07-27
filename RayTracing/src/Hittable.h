#pragma once

#include "Utils.h"
#include "Ray.h"
#include "HitRecord.h"
#include "AABB.h"

class Hittable
{
public:
	virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& record) const = 0;
	virtual bool bounding_box(float time0, float time1, AABB& output_box) const = 0;
};

