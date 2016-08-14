#pragma once

#include "chaperone_universe.h"
#include <vector>

namespace ChaperoneIo
{
std::vector<ChaperoneUniverse> parseFile(const std::string& fileName);

void writeToFile(const std::string& file, const std::vector<ChaperoneUniverse>& universes);
}
