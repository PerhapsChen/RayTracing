#pragma once

#include "Utils.h"
#include "Material.h"
#include "Texture.h"

class DielectricMaterial : public Material
{
public:
    DielectricMaterial(float index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(const Ray& r_in, const HitRecord& rec, color& attenuation, Ray& scattered) const override 
    {
        attenuation = color(1.0, 1.0, 1.0);
        float refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        glm::vec3 unit_direction = glm::normalize(r_in.Direction);
        float cos_theta = fmin(glm::dot(-unit_direction, rec.normal), 1.0);
        float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        glm::vec3 direction;
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > Walnut::Random::Float())
            direction = glm::reflect(unit_direction, rec.normal);
        else
            direction = glm::refract(unit_direction, rec.normal, refraction_ratio);

        // scattered = ray(rec.p, direction);
        scattered = Ray(rec.p, direction, r_in.time);
        return true;
    }

public:
    float ir; // Index of Refraction

private:
    static float reflectance(float cosine, float ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};