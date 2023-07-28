#include "Sphere.h"


Sphere::Sphere(point3 cen, float r, std::shared_ptr<Material> m) : 
    center(cen), radius(r), material_ptr(m) 
{
    const auto rvec = vec3(radius, radius, radius);
    box = AABB(center - rvec, center + rvec);
}

bool Sphere::hit(const Ray& ray, Interval ray_t, HitRecord& record) const
{

    float a = glm::dot(ray.Direction, ray.Direction);
    float half_b = glm::dot(ray.Direction, ray.Origin - center);
    float c = glm::dot(ray.Origin - center, ray.Origin - center) - radius * radius;


    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (!ray_t.contains(root)) {
        root = (-half_b + sqrtd) / a;
        if (!ray_t.contains(root))
            return false;
    }

    record.t = root;
    record.p = ray.at(record.t);
    vec3 outward_normal = (record.p - center) / radius;
    record.set_face_normal(ray, outward_normal);
    get_sphere_uv(outward_normal, record.u, record.v);
    record.material_ptr = material_ptr;

    return true;
}

AABB Sphere::bounding_box() const
{
    return box;
}