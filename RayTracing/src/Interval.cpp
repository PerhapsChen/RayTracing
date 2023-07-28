#include "Interval.h"

const Interval Interval::empty = Interval(+Utils::INF, -Utils::INF);
const Interval Interval::universe = Interval(-Utils::INF, +Utils::INF);

Interval operator+(const Interval& ival, float displacement) {
    return Interval(ival.min + displacement, ival.max + displacement);
}

Interval operator+(float displacement, const Interval& ival) {
    return ival + displacement;
}
