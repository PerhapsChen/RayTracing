#pragma once

#include "Utils.h"

// Çø¼äÀà¡£

class Interval {
public:
    Interval() : min(+Utils::INF), max(-Utils::INF) {} // Default interval is empty

    Interval(float _min, float _max) : min(_min), max(_max) {}

    Interval(const Interval& a, const Interval& b)
        : min(fmin(a.min, b.min)), max(fmax(a.max, b.max)) {}

    float size() const {
        return max - min;
    }

    Interval expand(float delta) const {
        const auto padding = delta / 2;
        return Interval(min - padding, max + padding);
    }

    bool contains(float x) const {
        return min <= x && x <= max;
    }

    float clamp(float x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }
    static const Interval empty, universe;

public:
    float min, max;
};


Interval operator+(const Interval& ival, float displacement);

Interval operator+(float displacement, const Interval& ival);
