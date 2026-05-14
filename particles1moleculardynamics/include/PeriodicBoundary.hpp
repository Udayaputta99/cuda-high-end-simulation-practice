#pragma once

#include <cmath>

#include "Vec.hpp"

class PeriodicBoundary {
public:
    PeriodicBoundary(double bLength) : boxLength(bLength) {};

    Vec3D wrapPosition(const Vec3D& position) const {
        Vec3D newPosition(position);
        for (auto i = 0; i < position.size(); ++i) {
           newPosition[i] = position[i] - boxLength * std::floor(position[i] / boxLength);
        }
        return newPosition;
        
    }

    Vec3D displacement(const Vec3D& positionI, const Vec3D& positionJ) const {
        auto dPos = positionI - positionJ;

        dPos[0] = dPos[0] - boxLength*std::round(dPos[0]/boxLength);
        dPos[1] = dPos[1] - boxLength*std::round(dPos[1]/boxLength);
        dPos[2] = dPos[2] - boxLength*std::round(dPos[2]/boxLength);
        return dPos;
    }

private:
    double boxLength;
};