#pragma once
#include "Utils.h"
#include "Material.h"
#include "Texture.h"

class LambertianMaterial : public Material
{
public:
	LambertianMaterial(const color& alb) : albedo(std::make_shared<SolidColorTexture>(alb)) {  }
	LambertianMaterial(std::shared_ptr<Texture> a) : albedo(a) {}
	virtual bool scatter(const Ray& r_in, const HitRecord& rec, color& attenuation, Ray& scattered) const override;
public:
	std::shared_ptr<Texture> albedo;
};