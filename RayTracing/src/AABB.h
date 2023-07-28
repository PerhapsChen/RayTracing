#pragma once

#include "Utils.h"
#include "Ray.h"
#include "Interval.h"

class AABB  // axis aligned bounding box.
{
public:
    AABB() {}
    // two points can determine an aabb
    AABB(const Interval& ix, const Interval& iy, const Interval& iz) 
        : x(ix), y(iy), z(iz) {}


    AABB(const point3& a, const point3& b) {
        // Treat the two points a and b as extrema for the bounding box, so we don't require a
        // particular minimum/maximum coordinate order.
        x = Interval(fmin(a[0], b[0]), fmax(a[0], b[0]));
        y = Interval(fmin(a[1], b[1]), fmax(a[1], b[1]));
        z = Interval(fmin(a[2], b[2]), fmax(a[2], b[2]));
    }

    AABB(const AABB& box0, const AABB& box1) {
        x = Interval(box0.x, box1.x);
        y = Interval(box0.y, box1.y);
        z = Interval(box0.z, box1.z);
    }

    AABB pad() {
        // Return an AABB that has no side narrower than some delta, padding if necessary.
        const float delta = 0.0001;
        Interval new_x = (x.size() >= delta) ? x : x.expand(delta);
        Interval new_y = (y.size() >= delta) ? y : y.expand(delta);
        Interval new_z = (z.size() >= delta) ? z : z.expand(delta);

        return AABB(new_x, new_y, new_z);
    }

    const Interval& axis(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }


    // 判断是否与包围盒相交
    bool hit(const Ray& r, Interval ray_t) const {
        for (int a = 0; a < 3; a++) {
            auto t0 = fmin((axis(a).min - r.Origin[a]) / r.Direction[a],
                (axis(a).max - r.Origin[a]) / r.Direction[a]);
            auto t1 = fmax((axis(a).min - r.Origin[a]) / r.Direction[a],
                (axis(a).max - r.Origin[a]) / r.Direction[a]);
            ray_t.min = fmax(t0, ray_t.min);
            ray_t.max = fmin(t1, ray_t.max);
            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;


    }

public:
    Interval x, y, z;
};

AABB operator+(const AABB& bbox, const vec3& offset);

AABB operator+(const vec3& offset, const AABB& bbox);