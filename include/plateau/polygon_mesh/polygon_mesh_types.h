#pragma once

#include <memory>
#include <vector>

namespace citygml { class CityModel; }

namespace plateau::polygonMesh {
    using CityModelVector = std::shared_ptr<std::vector<std::weak_ptr<const citygml::CityModel>>>;
}