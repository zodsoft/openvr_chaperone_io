#include <chaperone_io.h>

#include <json.h>
#include <memory>
#include <fstream>

Json::Value toJson(const Vec3& vec)
{
    Json::Value value;
    value.append(vec.x);
    value.append(vec.y);
    value.append(vec.z);
    return value;
}

Json::Value toJson(const ChaperoneUniverse::BoundsRect& rect)
{
    Json::Value value;
    for (int i = 0; i < 4; ++i)
    {
        value.append(toJson(rect.corners[i]));
    }
    return value;
}

Json::Value toJson(const ChaperoneUniverse::PlayerPosition& pos) {
    Json::Value posJson;
    posJson["translation"] = toJson(pos.translation);
    posJson["yaw"] = pos.yaw;
    return posJson;
}

Json::Value toJson(const ChaperoneUniverse::Tracker& tracker) {
    Json::Value trackerJson;
    trackerJson["angOffset"] = toJson(tracker.angularOffset);
    trackerJson["posOffset"] = toJson(tracker.positionOffset);
    trackerJson["serial"] = tracker.serialNumber;
    return trackerJson;
}

Json::Value toJson(const ChaperoneUniverse& universe) {
    Json::Value universeJson;

    Json::Value collisionBoundsJson;
    for (const ChaperoneUniverse::BoundsRect rect : universe.collisionBounds) {
        collisionBoundsJson.append(toJson(rect));
    }
    universeJson["collision_bounds"] = collisionBoundsJson;

    Json::Value playAreaJson;
    playAreaJson.append(universe.playArea.sizeX);
    playAreaJson.append(universe.playArea.sizeZ);
    universeJson["play_area"] = playAreaJson;

    universeJson["seated"] = toJson(universe.seated);
    universeJson["standing"] = toJson(universe.standing);

    universeJson["time"] = universe.date;

    universeJson["trackers"].append(toJson(universe.trackers[0]));
    universeJson["trackers"].append(toJson(universe.trackers[1]));

    universeJson["universeID"] = universe.id;

    return universeJson;
    
}

void ChaperoneIo::writeToFile(const std::string& file, const std::vector<ChaperoneUniverse>& universes)
{
    Json::Value jsonRoot;

    jsonRoot["jsonid"] = "chaperone_info";

    Json::Value jsonUniverses;
    for (const ChaperoneUniverse& universe : universes)
    {
        jsonUniverses.append(toJson(universe));
    }
    jsonRoot["universes"] = jsonUniverses;

    jsonRoot["version"] = 5;

    std::ofstream out(file);
    Json::StreamWriterBuilder builder;
    builder.settings_["indentation"] = "    ";
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    writer->write(jsonRoot, &out);
}
