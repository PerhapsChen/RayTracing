#include "AABB.h"

AABB operator+(const AABB& bbox, const vec3& offset) {
    return AABB(bbox.x + offset.x, bbox.y + offset.y, bbox.z + offset.z);
}

AABB operator+(const vec3& offset, const AABB& bbox) {
    return bbox + offset;
}