#include "BVHNode.h"
#include <iostream>


BVHNode::BVHNode(const std::vector<std::shared_ptr<Hittable>>& src_objects, size_t start, size_t end)
{
    auto objects = src_objects; // Create a modifiable array of the source scene objects

    int axis = static_cast<int>(3.0f*Walnut::Random::Float());
    auto comparator = (axis == 0) ? box_x_compare
        : (axis == 1) ? box_y_compare
        : box_z_compare;

    size_t object_span = end - start;

    if (object_span == 3) {
        left = std::make_shared<BVHNode>(objects, start, start + 2);
        right = objects[start + 2];
    }
    else if (object_span == 2) {
        left = objects[start];
        right = objects[start + 1];
    }
    else {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        auto mid = start + object_span / 2;
        left = std::make_shared<BVHNode>(objects, start, mid);
        right = std::make_shared<BVHNode>(objects, mid, end);
    }

    box = AABB(left->bounding_box(), right->bounding_box());
}

bool BVHNode::hit(const Ray& r, Interval ray_t, HitRecord& rec) const
{
    if (!box.hit(r, ray_t))
        return false;

    bool hit_left = left->hit(r, ray_t, rec);
    bool hit_right = right->hit(r, Interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

    return hit_left || hit_right;
}