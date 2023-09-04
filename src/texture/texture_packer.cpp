
#include <cassert>
#include <plateau/texture/texture_packer.h>

#include <string>
#include <vector>
#include <set>

namespace plateau::texture {
    using namespace polygonMesh;

    bool AtlasInfo::getValid() const {
        return valid_;
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

    void TextureAtlasCanvas::flush() {
        bool result = canvas_->save(save_file_path_);
        if(!result) {
            throw std::runtime_error("failed to write image file.");
        }
    }


    TexturePacker::TexturePacker(size_t width, size_t height, const int internal_canvas_count)
        : canvas_width_(width)
        , canvas_height_(height) {
        for (auto i = 0; i < internal_canvas_count; ++i) {
            canvases_.push_back(std::make_shared<TextureAtlasCanvas>(width, height));
        }
    }

    void TexturePacker::process(Model& model) {
        for (size_t i = 0; i < model.getRootNodeCount(); ++i) {
            const auto& child_node = model.getRootNodeAt(i);
            processNodeRecursive(child_node);
        }

        for (auto& canvas : canvases_) {
            if (!canvas->getSaveFilePath().empty()) {
                canvas->flush();
            }
        }
    }

    void TexturePacker::processNodeRecursive(const Node& node) {
        Mesh* mesh = node.getMesh();
        processMesh(mesh);
        for (int i = 0; i < node.getChildCount(); ++i) {
            const auto& child_node = node.getChildAt(i);
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

        std::vector<SubMesh> sub_mesh_list;

        for (int index = 0; index < sub_meshes.size(); ) { // TODO continue前やループ末尾の++indexはこのforの(括弧)内に移動できるのでは？

            auto& sub_mesh = sub_meshes[index];
            const auto& tex_url = sub_mesh.getTexturePath();
            if (tex_url.empty()) {
                sub_mesh_list.push_back(sub_mesh);
                ++index;
                continue;
            }


            bool texture_load_succeed = false;
            auto image = TextureImageBase::tryCreateFromFile(tex_url, canvas_height_, texture_load_succeed);
            if (!texture_load_succeed) {
                sub_mesh_list.push_back(sub_mesh);
                ++index;
                continue;
            }

            const auto width = image->getWidth();
            const auto height = image->getHeight();

            if (width > canvas_width_ || height >= canvas_height_) {
                sub_mesh_list.push_back(sub_mesh);
                ++index;
                continue;
            }

            // canvasのどれかにパックできるか確認
            int target_canvas_id = -1;
            AtlasInfo info = AtlasInfo::empty();
            for (int i=0; i<canvases_.size(); i++) {
                auto& canvas = canvases_.at(i);
                info = canvas->insert(width, height);
                if (info.getValid()) {
                    target_canvas_id = i;
                    break;
                }
            }

            // どこにもパック出来なかった場合
            if (target_canvas_id < 0) {
                // 占有率最大のcanvasを取得
                double max_coverage = 0.0;
                size_t max_coverage_index = 0;
                for (auto i=0; i<canvases_.size(); i++) {
                    auto& canvas = canvases_[i];
                    if (max_coverage < canvas->getCoverage()) {
                        max_coverage = canvas->getCoverage();
                        max_coverage_index = i;
                    }
                }
                // flushして空にしてから後でパックする。
                target_canvas_id = (int)max_coverage_index;
                canvases_.at(max_coverage_index)->flush();
                canvases_.at(max_coverage_index) = std::make_shared<TextureAtlasCanvas>(canvas_width_, canvas_height_);
                info = canvases_.at(max_coverage_index)->insert(width, height);
            }

            assert(info.getValid());

            constexpr auto delta = 1.0;
            const auto x = info.getLeft();
            const auto y = info.getTop();
            const auto u = info.getUPos();
            const auto v = info.getVPos();
            const auto u_fac = info.getUFactor() * delta;
            const auto v_fac = info.getVFactor() * delta;
            auto tex = sub_mesh.getTexturePath();

            auto& target_canvas = canvases_.at(target_canvas_id);
            image->packTo(&target_canvas->getCanvas(), x, y);
            target_canvas->setSaveFilePathIfEmpty(image->getFilePath());

            SubMesh new_sub_mesh = sub_mesh;
            new_sub_mesh.setTexturePath(target_canvas->getSaveFilePath());
            sub_mesh_list.push_back(new_sub_mesh);

            // SubMesh中に含まれる頂点番号を求めます。
            const auto& indices = mesh->getIndices();
            std::set<size_t> vertices_in_sub_mesh;
            for(auto i = sub_mesh.getStartIndex(); i <= sub_mesh.getEndIndex(); i++) {
                vertices_in_sub_mesh.insert(indices.at(i));
            }
            // SubMesh中に含まれる頂点について、UVを変更します。
            auto& uv1 = mesh->getUV1();
            for(const auto vs : vertices_in_sub_mesh) {
                const double uv_x = u + (uv1.at(vs).x * u_fac);
                const double uv_y = 1 - v - v_fac + (uv1.at(vs).y * v_fac);
                uv1[vs] = TVec2f{(float)uv_x, (float)uv_y};
            }

            ++index;
        }
        mesh->setSubMeshes(sub_mesh_list);
    }

    void TextureAtlasCanvas::update(const size_t width, const size_t height, const bool is_new_container) {

        capacity_ += (width * height);
        coverage_ = (double)capacity_ / static_cast<double>(canvas_width_ * canvas_height_) * 100.0;

        if (is_new_container) {
            vertical_range_ += height;
        }
    }

    AtlasInfo TextureAtlasCanvas::insert(const size_t width, const size_t height) {
        AtlasInfo atlas_info = AtlasInfo::empty();

        for (auto& container : container_list_) {
            if (container.getGap() != height)
                continue;

            if (container.getRootHorizontalRange() + width > canvas_width_)
                continue;

            container.add(width);
            atlas_info = AtlasInfo(
                true, container.getHorizontalRange(), container.getVerticalRange(), width, height,
                (double)container.getHorizontalRange() / static_cast<double>(canvas_width_),
                (double)container.getVerticalRange() / static_cast<double>(canvas_height_),
                (double)width / static_cast<double>(canvas_width_), (double)height / static_cast<double>(canvas_height_));
            this->update(width, height, false);

            break;
        }

        if (!atlas_info.getValid() && height + vertical_range_ < canvas_height_) {
            AtlasContainer container(height, 0, vertical_range_);
            container.add(width);
            container_list_.push_back(container);
            atlas_info = AtlasInfo(true, container.getHorizontalRange(), container.getVerticalRange(), width, height,
                                    (double)container.getHorizontalRange() / static_cast<double>(canvas_width_), (double)container.getVerticalRange() / static_cast<double>(canvas_height_),
                                    (double)width / static_cast<double>(canvas_width_), (double)height / static_cast<double>(canvas_height_));
            this->update(width, height, true);
        }

        return atlas_info;
    }
} // namespace plateau::texture
