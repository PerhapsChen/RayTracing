#pragma once

#include "Material.h"
#include "Texture.h"

class DiffuseLightMaterial : public Material
{
public:
    DiffuseLightMaterial(std::shared_ptr<Texture> a) : emit(a) {}
    DiffuseLightMaterial(color c) : emit(std::make_shared<SolidColorTexture>(c)) {}

    virtual bool scatter(const Ray& r_in, const HitRecord& rec, color& attenuation, Ray& scattered) const override
    {
        return false;
    }

    virtual color emitted(float u, float v, const point3& p) const
    {
        return emit->value(u, v, p);
    }

public:
    std::shared_ptr<Texture> emit;
};