#pragma once

#include "Utils.h"
#include "Material.h"
#include "Texture.h"

class MetalMaterial : public Material
{
public:
    MetalMaterial(const color& a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(const Ray& r_in, const HitRecord& rec, color& attenuation, Ray& scattered) const override
    {
        vec3 reflected = glm::reflect(glm::normalize(r_in.Direction), rec.normal);

        scattered = Ray(rec.p, reflected + fuzz * Walnut::Random::InUnitSphere(), r_in.time);
        attenuation = albedo;
        return (glm::dot(scattered.Direction, rec.normal) > 0);
    }
public:
    color albedo;
    float fuzz;
};