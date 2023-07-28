#pragma once
#include "Hittable.h"
#include "Utils.h"

class Sphere : public Hittable
{
public:
    Sphere() {}
    Sphere(point3 cen, float r, std::shared_ptr<Material> m);

    virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
    virtual AABB bounding_box() const override;
public:
    point3 center;
    float radius;
    std::shared_ptr<Material> material_ptr;
    AABB box;
private:
    static void get_sphere_uv(const point3& p, float& u, float& v)
    {
        auto theta = acos(-p.y);
        auto phi = atan2(-p.z, p.x) + Utils::pi;

        u = phi / (2 * Utils::pi);
        v = theta / Utils::pi;
    }
};