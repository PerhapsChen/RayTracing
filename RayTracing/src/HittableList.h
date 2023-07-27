#pragma onece;

#include "Hittable.h"
#include "Utils.h"
#include <vector>

class HittableList : public Hittable
{
public:
	HittableList() = default;
	HittableList(std::shared_ptr<Hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }
	virtual bool hit(const Ray& ray, float t_min, float t_max, HitRecord& record) const override;
	virtual bool bounding_box(float time0, float time1, AABB& output_box) const override;

public:
	std::vector<std::shared_ptr<Hittable>> objects;
};

