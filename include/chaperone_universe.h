#pragma once

#include <cstdint>
#include <vector>
#include <array>

struct Vec3 {
    float x, y, z;
};

struct ChaperoneUniverse
{
    struct BoundsRect
    {
        Vec3 corners[4];
    };

    struct PlayArea
    {
        float sizeX;
        float sizeZ;
    };

    struct PlayerPosition {
        Vec3 translation;
        float yaw;
    };

    struct Tracker
    {
        Vec3 angularOffset;
        Vec3 positionOffset;
        std::string serialNumber;
    };

    std::vector<BoundsRect> collisionBounds;
    PlayArea playArea;
    PlayerPosition seated;
    PlayerPosition standing;
    std::array<Tracker, 2> trackers;
    std::string date;
    uint32_t id;
};
