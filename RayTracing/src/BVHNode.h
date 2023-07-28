#pragma once

#include "Utils.h"
#include "Hittable.h"
#include "HittableList.h"

class BVHNode : public Hittable
{
public:
    BVHNode() {}

    BVHNode(const HittableList& list) : BVHNode(list.objects, 0, list.objects.size()) {}

    BVHNode(const std::vector<std::shared_ptr<Hittable>>& src_objects, size_t start, size_t end);

    virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;

    AABB bounding_box() const override { return box; }

public:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB box;

private:
    static bool box_compare(
        const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis_index
    ) {
        return a->bounding_box().axis(axis_index).min < b->bounding_box().axis(axis_index).min;
    }

    static bool box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
        return box_compare(a, b, 2);
    }
};

