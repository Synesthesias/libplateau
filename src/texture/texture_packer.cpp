
#include <cassert>
#include <plateau/texture/texture_packer.h>

#include <string>
#include <vector>

namespace plateau::texture {
    using namespace polygonMesh;

    bool AtlasInfo::getValid() const {
        return valid_;
    }

    void AtlasInfo::clear() {
        this->setAtlasInfo(false, 0, 0, 0, 0, 0, 0, 0, 0);
    }

    void AtlasInfo::setAtlasInfo(
            const bool valid, const size_t left, const size_t top,
            const size_t width, const size_t height,
            double u_pos, double v_pos, double u_factor, double v_factor) {
        valid_ = valid;
        left_ = left;
        top_ = top;
        width_ = width;
        height_ = height;
        u_pos_ = u_pos;
        v_pos_ = v_pos;
        u_factor_ = u_factor;
        v_factor_ = v_factor;
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
        canvas_width_ = width;
        canvas_height_ = height;
        this->clear();
    }

    void TextureAtlasCanvas::clear() {
        vertical_range_ = 0;
        capacity_ = 0;
        container_list_.clear();
        save_file_path_.clear();
        canvas_.init(canvas_width_, canvas_height_, gray);
    }

    void TextureAtlasCanvas::flush() {
        canvas_.save(save_file_path_);
        this->clear();
    }

    TexturePacker::~TexturePacker() = default;

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
            processMesh(mesh);
            processNodeRecursive(child_node);
        }
    }

    void TexturePacker::processMesh(plateau::polygonMesh::Mesh* mesh) {
        if (mesh == nullptr) {
            return;
        }

        auto& sub_meshes = mesh->getSubMeshes();

        if (sub_meshes.empty()) {
            return;
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

            auto image = TextureImage(tex_url, canvas_height_);
            if (image.getTextureType() == TextureImage::TextureType::None) {
                sub_mesh_list.push_back(sub_mesh);
                ++index;
                continue;
            }

            const auto width = image.getWidth();
            const auto height = image.getHeight();

            if (width > canvas_width_ || height >= canvas_height_) {
                sub_mesh_list.push_back(sub_mesh);
                ++index;
                continue;
            }

            // canvasのどれかにパックできるか確認
            TextureAtlasCanvas* target_canvas = nullptr;
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

            SubMesh new_sub_mesh = sub_mesh;
            new_sub_mesh.setTexturePath(target_canvas->getSaveFilePath());
            sub_mesh_list.push_back(new_sub_mesh);

            std::vector<TVec2f> new_uv1;
            for (auto& uv1 : mesh->getUV1()) {
                const double uv_x = u + (uv1.x * u_fac);
                const double uv_y = 1 - v - v_fac + (uv1.y * v_fac);
                new_uv1.emplace_back(uv_x, uv_y);
            }
            mesh->setUV1(new_uv1);
            ++index;
        }
        mesh->setSubMeshes(sub_mesh_list);
    }

    void TextureAtlasCanvas::update(const size_t width, const size_t height, const bool is_new_container) {

        capacity_ += (width * height);
        coverage_ = capacity_ / static_cast<double>(canvas_width_ * canvas_height_) * 100.0;

        if (is_new_container) {
            vertical_range_ += height;
        }
    }

    AtlasInfo TextureAtlasCanvas::insert(const size_t width, const size_t height) {
        AtlasInfo atlas_info;

        for (auto& container : container_list_) {
            if (container.getGap() != height)
                continue;

            if (container.getRootHorizontalRange() + width > canvas_width_)
                continue;

            container.add(width);
            atlas_info.setAtlasInfo(
                true, container.getHorizontalRange(), container.getVerticalRange(), width, height,
                container.getHorizontalRange() / static_cast<double>(canvas_width_), container.getVerticalRange() / static_cast<double>(canvas_height_),
                width / static_cast<double>(canvas_width_), height / static_cast<double>(canvas_height_));
            this->update(width, height, false);

            break;
        }

        if (!atlas_info.getValid() && height + vertical_range_ < canvas_height_) {
            AtlasContainer container(height, 0, vertical_range_);
            container.add(width);
            container_list_.push_back(container);
            atlas_info.setAtlasInfo(true, container.getHorizontalRange(), container.getVerticalRange(), width, height,
                                      container.getHorizontalRange() / static_cast<double>(canvas_width_), container.getVerticalRange() / static_cast<double>(canvas_height_),
                                      width / static_cast<double>(canvas_width_), height / static_cast<double>(canvas_height_));
            this->update(width, height, true);
        }

        return atlas_info;
    }
} // namespace plateau::texture
