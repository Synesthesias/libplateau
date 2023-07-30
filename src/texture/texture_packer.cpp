

#include <plateau/texture/texture_packer.h>

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace imageReader;

namespace plateau::texture {

    bool
    AtlasInfo::getValid() const {
        return valid;
    }

    void
    AtlasInfo::clear() {
        this->set_atlas_info(false, 0, 0, 0, 0, 0, 0, 0, 0);
    }

    void
    AtlasInfo::set_atlas_info(const bool _valid, const size_t _left, const size_t _top, const size_t _width, const size_t _height,
        double _uPos, double _vPos, double _uFactor, double _vFactor) {
        valid = _valid;
        left = _left;
        top = _top;
        width = _width;
        height = _height;
        uPos = _uPos;
        vPos = _vPos;
        uFactor = _uFactor;
        vFactor = _vFactor;
    }

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

    int
    TexturePacker::serialNumber = 0;

    void
    TexturePacker::setSaveFilePath(std::string fileName) {
        if (saveFilePath.empty()) {


            std::stringstream ss;

            ss << std::setw(6) << std::setfill('0') << ++serialNumber;

            std::string num = ss.str();

            std::filesystem::path save_path = fileName;

            auto parent_dir = save_path.parent_path();

            saveFilePath = parent_dir.string() + "\\" + "packed_image_" + num + ".jpg";
        }
    }

    void
    TexturePacker::init(size_t width, size_t height) {
        canvas_width = width;
        canvas_height = height;
        this->clear();
    }

    void
    TexturePacker::clear() {
        vertical_range = 0;
        capacity = 0;
        container_list.clear();
        saveFilePath.clear();
        canvas.init(canvas_width, canvas_height, gray);
    }

    void
    TexturePacker::flush() {
        canvas.save(saveFilePath.string());
        this->clear();
    }

    void
    TexturePacker::process(Model& model) {

        for (int i = 0; i < model.getRootNodeCount(); ++i) {
            const auto& child_node = model.getRootNodeAt(i);
            processNodeRecursive(child_node);
        }

        if (!saveFilePath.empty()) {
            this->flush();
        }        
    }

    void
    TexturePacker::processNodeRecursive(const Node& node) {

        for (int i = 0; i < node.getChildCount(); ++i) {
            const auto& child_node = node.getChildAt(i);

            Mesh* mesh = child_node.getMesh();

            if (mesh == nullptr) {
                continue;
            }

            auto& sub_meshes = mesh->getSubMeshes();

            if (sub_meshes.empty()) {
                continue;
            }

            int index = 0;
            std::vector<SubMesh> sub_mesh_list;

            for (index = 0; index < sub_meshes.size(); ) {

                auto& sub_mesh = sub_meshes[index];
                const auto& tex_url = sub_mesh.getTexturePath();
                if (tex_url.empty()) {
                    sub_mesh_list.push_back(sub_mesh);
                    ++index;
                    continue;
                }

                auto image = TextureImage(tex_url);
                auto width = image.getWidth();
                auto height = image.getHeight();
                auto info = this->insert(width, height);
                auto delta = 1.0;

                if (info.getValid()) {

                    auto x = info.getLeft();
                    auto y = info.getTop();
                    auto w = info.getWidth();
                    auto h = info.getHeight();
                    auto u = info.getUPos();
                    auto v = info.getVPos();
                    auto ufac = info.getUFactor() * delta;
                    auto vfac = info.getVFactor() * delta;
                    auto tex = sub_mesh.getTexturePath();

                    canvas.pack(x, y, image);
                    setSaveFilePath(image.getImageFilePath());

                    SubMesh newSubMesh = sub_mesh;
                    newSubMesh.setTexturePath(saveFilePath.generic_string());
                    sub_mesh_list.push_back(newSubMesh);

                    std::vector<TVec2f> newUV1;
                    for (auto& uv1 : mesh->getUV1()) {

                        double uvx = u + (uv1.x * ufac);
                        double uvy = 1 - v - vfac + (uv1.y * vfac);
                        newUV1.push_back(TVec2f(uvx, uvy));
                    }
                    mesh->setUV1(newUV1);
                    ++index;
                }
                else {
                    if (!saveFilePath.empty()) {
                        this->flush();
                    }

                    if ((width > canvas_width) || (height > canvas_height)) {
                        sub_mesh_list.push_back(sub_mesh);
                        ++index;
                    }
                    continue;
                }
                mesh->setSubMeshes(sub_mesh_list);
            }
            processNodeRecursive(child_node);
        }
    }

    void
    TexturePacker::update(const size_t _width, const size_t _height, const bool _is_new_container) {

        capacity += (_width * _height);
        coverage = capacity / static_cast<double>(canvas_width * canvas_height) * 100.0;

        if (_is_new_container) {
            vertical_range += _height;
        }
    }

    const AtlasInfo
    TexturePacker::insert(const size_t _width, const size_t _height) {

        atlas_info.clear();
        for (auto& container : container_list) {
            if (container.getGap() == _height) {
                if (container.getGap() == _height) {
                    auto length = container.getRootHorizontalRange();
                    if ((length + _width) <= canvas_width) {

                        container.add(_width);
                        atlas_info.set_atlas_info(true, container.getHorizontalRange(), container.getVerticalRange(), _width, _height,
                            container.getHorizontalRange() / (double)canvas_width, container.getVerticalRange() / (double)canvas_height,
                            _width / (double)canvas_width, _height / (double)canvas_height);
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
                atlas_info.set_atlas_info(true, container.getHorizontalRange(), container.getVerticalRange(), _width, _height,
                    container.getHorizontalRange() / static_cast<double>(canvas_width), container.getVerticalRange() / static_cast<double>(canvas_height),
                    _width / static_cast<double>(canvas_width), _height / static_cast<double>(canvas_height));
                this->update(_width, _height, true);
            }
        }
        return atlas_info;
    }

} // namespace atlas

