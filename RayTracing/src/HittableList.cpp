#include "HittableList.h"


void HittableList::add(std::shared_ptr<Hittable> object)
{
    objects.push_back(object);
    box = AABB(box, object->bounding_box());
}

bool HittableList::hit(const Ray& ray, Interval ray_t, HitRecord& record) const
{
    HitRecord temp_rec;
    auto hit_anything = false;
    auto closest_so_far = ray_t.max;

    for (const auto& object : objects) {
        if (object->hit(ray, Interval(ray_t.min, closest_so_far), temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            record = temp_rec;
        }
    }

    return hit_anything;
}

AABB HittableList::bounding_box() const
{
    return box;
}