#pragma once
#include "Hittable.h"
#include "Utils.h"

class Triangle : public Hittable
{
public:
	Triangle() {}
	Triangle(point3 a, point3 b, point3 c, std::shared_ptr<Material> m);
	virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
	virtual AABB bounding_box() const override;

public:
	point3 mA_Position;
	point3 mB_Position;
	point3 mC_Position;
	vec3 m_Normal = glm::vec3(0.0f, 1.0f, 0.0f);
	std::shared_ptr<Material> material_ptr;
	AABB box;
};

