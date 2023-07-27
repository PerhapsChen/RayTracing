#pragma once

#include "Utils.h"
#include "Ray.h"
#include "glm/glm.hpp"

class Material;

struct HitRecord {
    glm::vec3 p;
    glm::vec3 normal;
    std::shared_ptr<Material> material_ptr;
    float t;
    float u;
    float v;
    bool front_face;

    inline void set_face_normal(const Ray& r, const glm::vec3& outward_normal)
    {
        front_face = glm::dot(r.Direction, outward_normal) < 0; // ������ߵķ���������ķ��߷���н��Ƕ۽ǣ���������
        normal = front_face ? outward_normal : -outward_normal;
    }

};