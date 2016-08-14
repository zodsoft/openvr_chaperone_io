#include <chaperone_io.h>

#include <fstream>
#include <json.h>
#include <memory>

Vec3 parseVec3(const Json::Value& vec3)
{
    return Vec3{
        vec3[0].asFloat(),
        vec3[1].asFloat(),
        vec3[2].asFloat()
    };
}

ChaperoneUniverse::PlayerPosition parsePlayerPosition(const Json::Value& playerPosJson)
{
    Vec3 translation = parseVec3(playerPosJson["translation"]);
    float yaw = playerPosJson["yaw"].asFloat();
    return ChaperoneUniverse::PlayerPosition{ translation, yaw };
}

ChaperoneUniverse::Tracker parseTracker(const Json::Value& trackerJson) {
    Vec3 angularOffset = parseVec3(trackerJson["angOffset"]);
    Vec3 positionOffset = parseVec3(trackerJson["posOffset"]);
    std::string serialNumber = trackerJson["serial"].asString();
    return ChaperoneUniverse::Tracker{ angularOffset, positionOffset, serialNumber };
}

std::unique_ptr<ChaperoneUniverse> parseUniverse(const Json::Value& universe)
{
    const Json::Value& collisionBoundsJson = universe["collision_bounds"];
    std::vector<ChaperoneUniverse::BoundsRect> collisionBounds;
    for (size_t rect = 0; rect < collisionBoundsJson.size(); ++rect)
    {
        ChaperoneUniverse::BoundsRect boundsRect;
        for (int corner = 0; corner < 4; ++corner)
        {
            boundsRect.corners[corner] = parseVec3(collisionBoundsJson[rect][corner]);
        }
        collisionBounds.push_back(boundsRect);
    }

    const Json::Value& playAreaJson = universe["play_area"];
    float sizeX = playAreaJson[0].asFloat();
    float sizeZ = playAreaJson[1].asFloat();
    ChaperoneUniverse::PlayArea playArea{ sizeX, sizeZ };

    ChaperoneUniverse::PlayerPosition seatedPos = parsePlayerPosition(universe["seated"]);
    ChaperoneUniverse::PlayerPosition standingPos = parsePlayerPosition(universe["standing"]);

    std::array<ChaperoneUniverse::Tracker, 2> trackers;
    trackers[0] = parseTracker(universe["trackers"][0]);
    trackers[1] = parseTracker(universe["trackers"][1]);

    std::string date = universe["time"].asString();

    uint32_t id = universe["universeID"].asUInt();
    return std::unique_ptr<ChaperoneUniverse>(
        new ChaperoneUniverse{ collisionBounds, playArea, seatedPos, standingPos, trackers, date, id });
}

std::vector<ChaperoneUniverse> ChaperoneIo::parseFile(const std::string& fileName)
{
    std::ifstream file(fileName);
    Json::Value root;
    Json::Reader reader;
    bool success = reader.parse(file, root, false);
    if (!success)
    {
        throw std::runtime_error(
            "Failed to parse json file " + fileName + ":\n" + reader.getFormattedErrorMessages());
    }

    std::string id = root["jsonid"].asString();
    if (id != "chaperone_info")
    {
        throw std::runtime_error("Json file is not a chaperone file");
    }

    uint32_t version = root["version"].asUInt();
    if (version != 5)
    {
        throw std::runtime_error("Unsupported chaperone version");
    }

    Json::Value& universes = root["universes"];
    std::vector<ChaperoneUniverse> parsedUniverses(universes.size());
    for (size_t i = 0; i < universes.size(); ++i)
    {
        parsedUniverses.push_back(*parseUniverse(universes[i]));
    }
    return parsedUniverses;
}
