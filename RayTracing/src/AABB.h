#pragma once

#include "Utils.h"
#include "Ray.h"

class AABB  // axis aligned bounding box.
{
public:
    AABB() : minimum(point3{ Utils::INF }), maximum(point3{ -Utils::INF })
    {
    }
    // two points can determine an aabb
    AABB(const point3& a, const point3& b) {
        minimum = a; maximum = b;
    }

    point3 min() { return minimum; }
    point3 max() { return maximum; }


    // 判断是否与包围盒相交
    bool hit(const Ray& r, float t_min, float t_max) const
    {
        for (int a = 0; a < 3; a++)
        {
            double inv = 1.0f / r.Direction[a];
            auto t0 = fmin((minimum[a] - r.Origin[a]) * inv,
                (maximum[a] - r.Origin[a]) * inv);
            auto t1 = fmax((minimum[a] - r.Origin[a]) * inv,
                (maximum[a] - r.Origin[a]) * inv);
            t_min = fmax(t0, t_min);
            t_max = fmin(t1, t_max);
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

public:
    point3 minimum;
    point3 maximum;

    //static AABB surrounding_box(AABB box0, AABB box1) {
    //    point3 small(
    //        fmin(box0.min().x, box1.min().x),
    //        fmin(box0.min().y, box1.min().y),
    //        fmin(box0.min().z, box1.min().z)
    //    );

    //    point3 big(
    //        fmax(box0.max().x, box1.max().x),
    //        fmax(box0.max().y, box1.max().y),
    //        fmax(box0.max().z, box1.max().z)
    //    );

    //    return AABB(small, big);
    //}
};

