#include <plateau/texture/atlas_container.h>

namespace plateau::texture {
    AtlasContainer::AtlasContainer(const size_t _gap, const size_t _horizontal_range, const size_t _vertical_range) {
        gap = _gap;
        vertical_range = _vertical_range;
        root_horizontal_range = _horizontal_range;
        horizontal_range = _horizontal_range;
    }

    void AtlasContainer::add(const size_t _length) {
        horizontal_range = root_horizontal_range;
        root_horizontal_range = _length + root_horizontal_range;
    }
}
