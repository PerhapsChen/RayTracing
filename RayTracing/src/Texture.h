#pragma once
#include "Utils.h"

class Texture
{
public:
    virtual color value(float u, float v, const point3& p) const = 0; // sample
};

class SolidColorTexture : public Texture // single color.
{
public:
    SolidColorTexture() = delete;
    SolidColorTexture(color c) : color_value(c) {}
    SolidColorTexture(float red, float green, float blue) : SolidColorTexture(color(red, green, blue)) {}

    virtual color value(float u, float v, const vec3& p) const override { return color_value; }

public:
    color color_value;
};