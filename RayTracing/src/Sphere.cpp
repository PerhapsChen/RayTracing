#include "Sphere.h"

bool Sphere::hit(const Ray& ray, float t_min, float t_max, HitRecord& record) const
{

    float A = glm::dot(ray.Direction, ray.Direction);
    float B = 2.0f * glm::dot(ray.Direction, ray.Origin - center);
    float C = glm::dot(ray.Origin - center, ray.Origin - center) - radius * radius;

    auto discriminant = B * B - 4 * A * C;

    if (discriminant < 0) 
        return false;
    auto sqrtd = std::sqrt(discriminant);
    auto root = (-B - sqrtd) / (2.0 * A);
    if (root < t_min || root > t_max) {
        root = (-B + sqrtd) / (2.0 * A);
        if (root < t_min || root > t_max) {
            return false;
        }
    }
    record.t = root;
    record.p = ray.at(record.t);
    vec3 outward_normal = (record.p - center) / radius; // normal is a unit vec3
    record.set_face_normal(ray, outward_normal);
    get_sphere_uv(outward_normal, record.u, record.v);
    record.material_ptr = material_ptr;

    return true;
}

bool Sphere::bounding_box(float time0, float time1, AABB& output_box) const
{
    output_box = AABB(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
    return true;
}