
#include <cassert>
#include <plateau/texture/texture_packer.h>

#include <string>
#include <vector>

namespace plateau::texture {
    using namespace polygonMesh;

    bool AtlasInfo::getValid() const {
        return valid;
    }

    void AtlasInfo::clear() {
        this->set_atlas_info(false, 0, 0, 0, 0, 0, 0, 0, 0);
    }

    void AtlasInfo::set_atlas_info(const bool _valid, const size_t _left, const size_t _top, const size_t _width, const size_t _height,
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

    void AtlasContainer::add(const size_t _length) {
        horizontal_range = root_horizontal_range;
        root_horizontal_range = _length + root_horizontal_range;
    }

    void TextureAtlasCanvas::setSaveFilePathIfEmpty(const std::string& original_file_path) {
        if (!save_file_path_.empty())
            return;

        auto original_path = std::filesystem::u8path(original_file_path);
        const auto original_filename_without_ext = original_path.filename().replace_extension("").u8string();

        for (int cnt = 0;; ++cnt) {
            std::stringstream ss;
            ss << std::setw(6) << std::setfill('0') << cnt;
            std::string num = ss.str();

            const auto new_filename = std::string("packed_image_").append(original_filename_without_ext).append("_").append(num).append(".jpg");
            const auto& path = original_path.replace_filename(new_filename);
            if (!std::filesystem::is_regular_file(path)) {
                save_file_path_ = path.u8string();
                break;
            }
        }
    }

    const std::string& TextureAtlasCanvas::getSaveFilePath() const {
        return save_file_path_;
    }

    void TextureAtlasCanvas::init(size_t width, size_t height) {
        canvas_width = width;
        canvas_height = height;
        this->clear();
    }

    void TextureAtlasCanvas::clear() {
        vertical_range = 0;
        capacity = 0;
        container_list.clear();
        save_file_path_.clear();
        canvas.init(canvas_width, canvas_height, gray);
    }

    void TextureAtlasCanvas::flush() {
        canvas.save(save_file_path_);
        this->clear();
    }

    void TexturePacker::process(Model& model) {
        for (size_t i = 0; i < model.getRootNodeCount(); ++i) {
            const auto& child_node = model.getRootNodeAt(i);
            processNodeRecursive(child_node);
        }

        for (auto& canvas : canvases_) {
            if (!canvas.getSaveFilePath().empty()) {
                canvas.flush();
            }
        }
    }

    void TexturePacker::processNodeRecursive(const Node& node) {
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

                auto image = TextureImage(tex_url, canvas_height);
                if (image.getTextureType() == TextureImage::TextureType::None) {
                    ++index;
                    continue;
                }

                const auto width = image.getWidth();
                const auto height = image.getHeight();

                if (width > canvas_width || height >= canvas_height) {
                    sub_mesh_list.push_back(sub_mesh);
                    ++index;
                    continue;
                }

                // canvasのどれかにパックできるか確認
                TextureAtlasCanvas* target_canvas;
                AtlasInfo info;
                for (auto& canvas : canvases_) {
                    info = canvas.insert(width, height);
                    if (info.getValid()) {
                        target_canvas = &canvas;
                        break;
                    }
                }

                // どこにもパック出来なかった場合
                if (target_canvas == nullptr) {
                    // 占有率最大のcanvasを取得
                    double max_coverage = 0.0;
                    for (auto& canvas : canvases_) {
                        if (max_coverage < canvas.getCoverage()) {
                            max_coverage = canvas.getCoverage();
                            target_canvas = &canvas;
                        }
                    }

                    // flushして空にしてから後でパックする。
                    target_canvas->flush();
                    info = target_canvas->insert(width, height);
                    assert(info.getValid());
                }

                constexpr auto delta = 1.0;
                const auto x = info.getLeft();
                const auto y = info.getTop();
                const auto u = info.getUPos();
                const auto v = info.getVPos();
                const auto u_fac = info.getUFactor() * delta;
                const auto v_fac = info.getVFactor() * delta;
                auto tex = sub_mesh.getTexturePath();

                target_canvas->getCanvas().pack(x, y, image);
                target_canvas->setSaveFilePathIfEmpty(image.getImageFilePath());

                SubMesh newSubMesh = sub_mesh;
                newSubMesh.setTexturePath(target_canvas->getSaveFilePath());
                sub_mesh_list.push_back(newSubMesh);

                std::vector<TVec2f> newUV1;
                for (auto& uv1 : mesh->getUV1()) {
                    const double uv_x = u + (uv1.x * u_fac);
                    const double uv_y = 1 - v - v_fac + (uv1.y * v_fac);
                    newUV1.emplace_back(uv_x, uv_y);
                }
                mesh->setUV1(newUV1);
                ++index;
                mesh->setSubMeshes(sub_mesh_list);
            }
            processNodeRecursive(child_node);
        }
    }

    void TextureAtlasCanvas::update(const size_t _width, const size_t _height, const bool _is_new_container) {

        capacity += (_width * _height);
        coverage = capacity / static_cast<double>(canvas_width * canvas_height) * 100.0;

        if (_is_new_container) {
            vertical_range += _height;
        }
    }

    AtlasInfo TextureAtlasCanvas::insert(const size_t width, const size_t height) {
        AtlasInfo atlas_info;

        for (auto& container : container_list) {
            if (container.getGap() != height)
                continue;

            if (container.getRootHorizontalRange() + width > canvas_width)
                continue;

            container.add(width);
            atlas_info.set_atlas_info(
                true, container.getHorizontalRange(), container.getVerticalRange(), width, height,
                container.getHorizontalRange() / static_cast<double>(canvas_width), container.getVerticalRange() / static_cast<double>(canvas_height),
                width / static_cast<double>(canvas_width), height / static_cast<double>(canvas_height));
            this->update(width, height, false);

            break;
        }

        if (!atlas_info.getValid() && height + vertical_range < canvas_height) {
            AtlasContainer container(height, 0, vertical_range);
            container.add(width);
            container_list.push_back(container);
            atlas_info.set_atlas_info(true, container.getHorizontalRange(), container.getVerticalRange(), width, height,
                                      container.getHorizontalRange() / static_cast<double>(canvas_width), container.getVerticalRange() / static_cast<double>(canvas_height),
                                      width / static_cast<double>(canvas_width), height / static_cast<double>(canvas_height));
            this->update(width, height, true);
        }

        return atlas_info;
    }
} // namespace plateau::texture
