#pragma once
#include "Hittable.h"
#include "Utils.h"

class Sphere : public Hittable
{
public:
    Sphere() {}
    Sphere(point3 cen, float r, std::shared_ptr<Material> m) :
        center(cen), radius(r), material_ptr(m) {}

    virtual bool hit(const Ray& r, float t_min, float t_max, HitRecord& rec) const override;
    virtual bool bounding_box(float time0, float time1, AABB& output_box) const override;
public:
    point3 center;
    float radius;
    std::shared_ptr<Material> material_ptr;
private:
    static void get_sphere_uv(const point3& p, float& u, float& v)
    {
        auto theta = acos(-p.y);
        auto phi = atan2(-p.z, p.x) + Utils::pi;

        u = phi / (2 * Utils::pi);
        v = theta / Utils::pi;
    }
};