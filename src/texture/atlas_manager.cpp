
#include <plateau/texture/atlas_manager.h>

#include <jpeglib.h>

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace plateau::texture {
    bool
        AtlasInfo::getValid() const {
        return valid;
    }

    void
        AtlasInfo::clear() {
        this->set_atlas_info(false, 0, 0, 0, 0);
    }

    void
        AtlasInfo::set_atlas_info(const bool _valid, const size_t _left, const size_t _top, const size_t _width, const size_t _height) {

        valid = _valid;
        left = _left;
        top = _top;
        width = _width;
        height = _height;
    }

    int AtlasContainer::counter;

    AtlasContainer::AtlasContainer(const size_t _gap, const size_t _horizontal_range, const size_t _vertical_range) {

        gap = _gap;
        vertical_range = _vertical_range;
        root_horizontal_range = _horizontal_range;
        horizontal_range = _horizontal_range;
    }

    void
        AtlasContainer::add(const size_t _length) {

        auto g1 = gap;
        horizontal_range = root_horizontal_range;
        root_horizontal_range = _length + root_horizontal_range;
    }


    AtlasManager::AtlasManager(const size_t _width, const size_t _height) : vertical_range(0), capacity(0), coverage(0) {
        canvas_width = _width;
        canvas_height = _height;
    }

    void
        AtlasManager::update(const size_t _width, const size_t _height, const bool _is_new_container) {

        capacity += (_width * _height);
        coverage = capacity / static_cast<double>(canvas_width * canvas_height) * 100.0;

        if (_is_new_container) {
            vertical_range += _height;
        }
    }

    const AtlasInfo
        AtlasManager::insert(const size_t _width, const size_t _height) {

        atlas_info.clear();

        for (auto& container : container_list) {
            if (container.getGap() == _height) {

                if (container.getGap() == _height) {
                    auto length = container.getRootHorizontalRange();
                    if ((length + _width) <= canvas_width) {

                        container.add(_width);

                        atlas_info.set_atlas_info(true, container.getHorizontalRange(), container.getVerticalRange(), _width, _height);
                        this->update(_width, _height, false);
                        break;
                    }
                }
            }
        }

        if (!atlas_info.getValid()) {
            if ((_height + vertical_range) < canvas_height) {

                AtlasContainer container(_height, 0, vertical_range);
                container.add(_width);

                container_list.push_back(container);

                atlas_info.set_atlas_info(true, container.getHorizontalRange(), container.getVerticalRange(), _width, _height);
                this->update(_width, _height, true);
            }
        }
        return atlas_info;
    }

} // namespace atlas

