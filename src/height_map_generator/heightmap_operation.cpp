#include <plateau/height_map_generator/heightmap_operation.h>
#include <filesystem>
#include <algorithm>

namespace plateau::heightMapGenerator {

    double HeightmapOperation::getPositionFromPercent(const double percent, const double min, const double max) {
        const double dist = abs(max - min);
        const auto pos = min + (dist * percent);
        const auto clamped = std::clamp(pos, min, max);
        return clamped;
    }

    TVec2d HeightmapOperation::getPositionFromPercent(const TVec2d& percent, const TVec2d& min, const TVec2d& max) {
        return TVec2d(getPositionFromPercent(percent.x, min.x, max.x), getPositionFromPercent(percent.y, min.y, max.y));
    }

} // namespace heightMapGenerator

