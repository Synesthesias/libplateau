#pragma once

#include "citygml/citymodel.h"
#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace plateau::polygonMesh {
    using CityModelVector = std::shared_ptr<std::vector<std::weak_ptr<const citygml::CityModel>>>;
}