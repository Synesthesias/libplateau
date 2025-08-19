#pragma once

#include <memory>
#include <vector>

namespace plateau::polygonMesh {
    class citygml::CityModel;
    using CityModelVector = std::shared_ptr<std::vector<std::weak_ptr<const citygml::CityModel>>>;
}