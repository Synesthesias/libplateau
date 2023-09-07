
#include <cassert>
#include <plateau/texture/texture_packer.h>
#include <plateau/texture/atlas_container.h>

#include <string>
#include <vector>
#include <set>

namespace plateau::texture {
    using namespace polygonMesh;

    TexturePacker::TexturePacker(size_t width, size_t height, const int internal_canvas_count)
            : canvas_width_(width)
            , canvas_height_(height) {
        for (auto i = 0; i < internal_canvas_count; ++i) {
            canvases_.push_back(std::make_shared<TextureAtlasCanvas>(width, height));
        }
    }


    bool AtlasInfo::getValid() const {
        return valid_;
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

    void TexturePacker::processNodeRecursive(const Node& node) { // NOLINT(misc-no-recursion)
        Mesh* mesh = node.getMesh();
        processMesh(mesh);
        for (int i = 0; i < node.getChildCount(); ++i) {
            const auto& child_node = node.getChildAt(i);
            processNodeRecursive(child_node);
        }
    }

    namespace {
        void updateUVOfSubMesh(Mesh* mesh, const SubMesh& sub_mesh, const AtlasInfo& info) {
            constexpr auto delta = 1.0;
            const auto u = info.getUPos();
            const auto v = info.getVPos();
            const auto u_fac = info.getUFactor() * delta;
            const auto v_fac = info.getVFactor() * delta;



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
            const auto& tex_url = std::filesystem::u8path(sub_mesh.getTexturePath()).make_preferred().u8string();
            if (tex_url.empty()) {
                sub_mesh_list.push_back(sub_mesh);
                ++index;
                continue;
            }

            // すでにパック済みならばそれを利用
            AtlasInfo packed_info = AtlasInfo::empty();
            TextureAtlasCanvas* packed_atlas_canvas_ptr = nullptr;
            if(isTexturePacked(tex_url, packed_atlas_canvas_ptr, packed_info)) {
                if(packed_atlas_canvas_ptr != nullptr) {
                    SubMesh new_sub_mesh = sub_mesh;
                    new_sub_mesh.setTexturePath(packed_atlas_canvas_ptr->getSaveFilePath());
                    updateUVOfSubMesh(mesh, sub_mesh, packed_info);
                    sub_mesh_list.push_back(new_sub_mesh);
                    ++index;
                    continue;
                }
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
                info = canvas->insert(width, height, tex_url);
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
                info = canvases_.at(max_coverage_index)->insert(width, height, tex_url);
            }

            assert(info.getValid());


            auto& target_canvas = canvases_.at(target_canvas_id);
            image->packTo(&target_canvas->getCanvas(), info.getLeft(), info.getTop());
            target_canvas->setSaveFilePathIfEmpty(image->getFilePath());
            SubMesh new_sub_mesh = sub_mesh;
            new_sub_mesh.setTexturePath(target_canvas->getSaveFilePath());
            sub_mesh_list.push_back(new_sub_mesh);

            updateUVOfSubMesh(mesh, sub_mesh, info);

            ++index;
        }
        mesh->setSubMeshes(sub_mesh_list);
    }

    void TextureAtlasCanvas::update(const size_t width, const size_t height, const bool is_new_container, const AtlasInfo& packed_texture_info) {

        capacity_ += (width * height);
        coverage_ = (double)capacity_ / static_cast<double>(canvas_width_ * canvas_height_) * 100.0;
        packed_textures_info.push_back(packed_texture_info);

        if (is_new_container) {
            vertical_range_ += height;
        }
    }

    AtlasInfo TextureAtlasCanvas::insert(const size_t width, const size_t height, const std::string& src_texture_path) {
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
                (double)width / static_cast<double>(canvas_width_), (double)height / static_cast<double>(canvas_height_),
                src_texture_path);
            this->update(width, height, false, atlas_info);

            break;
        }

        if (!atlas_info.getValid() && height + vertical_range_ < canvas_height_) {
            AtlasContainer container(height, 0, vertical_range_);
            container.add(width);
            container_list_.push_back(container);
            atlas_info = AtlasInfo(true, container.getHorizontalRange(), container.getVerticalRange(), width, height,
                                    (double)container.getHorizontalRange() / static_cast<double>(canvas_width_), (double)container.getVerticalRange() / static_cast<double>(canvas_height_),
                                    (double)width / static_cast<double>(canvas_width_), (double)height / static_cast<double>(canvas_height_),
                                    src_texture_path);
            this->update(width, height, true, atlas_info);
        }

        return atlas_info;
    }



    bool TexturePacker::isTexturePacked(const std::string& src_file_path, TextureAtlasCanvas*& out_contained_canvas_ptr, AtlasInfo& out_atlas_info) {
        out_contained_canvas_ptr = nullptr;
        for(const auto& canvas : canvases_) {
            auto packed_info = AtlasInfo::empty();
            if(canvas->isTexturePacked(src_file_path, packed_info)) {
                out_contained_canvas_ptr = canvas.get();
                out_atlas_info = packed_info;
                return true;
            }
        }
        return false;
    }

} // namespace plateau::texture
