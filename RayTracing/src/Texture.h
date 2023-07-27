#pragma once
#include "Utils.h"

class Texture
{
public:
    virtual color value(double u, double v, const point3& p) const = 0; // sample
};

class SolidColorTexture : public Texture // single color.
{
public:
    SolidColorTexture() = delete;
    SolidColorTexture(color c) : color_value(c) {}
    SolidColorTexture(double red, double green, double blue) : SolidColorTexture(color(red, green, blue)) {}

    virtual color value(double u, double v, const vec3& p) const override { return color_value; }

public:
    color color_value;
};