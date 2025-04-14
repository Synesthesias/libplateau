#include "plateau/dataset/grid_code.h"
#include "plateau/dataset/mesh_code.h"
#include "plateau/dataset/standard_map_grid.h"
#include <stdexcept>

namespace plateau::dataset {

bool GridCode::isMeshCode() const {
    return dynamic_cast<const MeshCode*>(this) != nullptr;
}

std::shared_ptr<GridCode> GridCode::create(const std::string& code) {
    // Try creating as MeshCode
    try {
        auto meshCode = std::make_shared<MeshCode>(code);
        if (meshCode->isValid()) {
            return meshCode;
        }
    } catch (const std::invalid_argument&) {
        // Ignore and try StandardMapGrid
    }

    // Try creating as StandardMapGrid
    try {
        auto standardMapGrid = std::make_shared<StandardMapGrid>(code);
        if (standardMapGrid->isValid()) {
            return standardMapGrid;
        }
    } catch (const std::invalid_argument&) {
        // Ignore
    }

    throw std::invalid_argument("Invalid code format: " + code);
}

GridCode* GridCode::createRaw(const std::string& code) {
    // Try creating as MeshCode
    try {
        auto* meshCode = new MeshCode(code);
        if (meshCode->isValid()) {
            return meshCode;
        }
        delete meshCode;
    } catch (const std::invalid_argument&) {
        // Ignore and try StandardMapGrid
    }

    // Try creating as StandardMapGrid
    try {
        auto* standardMapGrid = new StandardMapGrid(code);
        if (standardMapGrid->isValid()) {
            return standardMapGrid;
        }
        delete standardMapGrid;
    } catch (const std::invalid_argument&) {
        // Ignore
    }

    throw std::invalid_argument("Invalid code format: " + code);
}

} 