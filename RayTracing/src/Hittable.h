#pragma once

#include "Utils.h"
#include "Ray.h"
#include "HitRecord.h"
#include "AABB.h"

class Hittable
{
public:
	virtual bool hit(const Ray& r, Interval ray_t, HitRecord& record) const = 0;
	virtual AABB bounding_box() const = 0;
};

