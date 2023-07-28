#pragma once;

#include "Hittable.h"
#include "Utils.h"
#include <vector>

class HittableList : public Hittable
{
public:
	HittableList() = default;
	HittableList(std::shared_ptr<Hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(std::shared_ptr<Hittable> object);
	virtual bool hit(const Ray& ray, Interval ray_t, HitRecord& record) const override;
	virtual AABB bounding_box() const override;

public:
	std::vector<std::shared_ptr<Hittable>> objects;
	AABB box;
};

