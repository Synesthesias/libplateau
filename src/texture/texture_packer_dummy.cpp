
#include <cassert>
#include <plateau/texture/texture_packer.h>
#include <plateau/polygon_mesh/model.h>

#include <string>
#include <vector>

namespace plateau::texture {
    /**
     * texture_packer.cpp はモバイル向けのビルドが通らないので、CMakeによって texture_packer_dummy.cpp に置き換えられます。
     */

    TexturePacker::TexturePacker(size_t width, size_t height, const int internal_canvas_count)
        : canvas_width_(width)
        , canvas_height_(height) {
        throw std::runtime_error("not implemented");
    }

    void TexturePacker::process(plateau::polygonMesh::Model& model) {
        throw std::runtime_error("not implemented");
    }

    void TexturePacker::processNodeRecursive(const plateau::polygonMesh::Node& node) {
        throw std::runtime_error("not implemented");
    }

    void TexturePacker::processMesh(plateau::polygonMesh::Mesh* mesh) {
        throw std::runtime_error("not implemented");
    }

    void TexturePacker::flush(){
        throw std::runtime_error("not implemented");
    }

    AtlasInfo TexturePacker::packImage(TextureImageBase* image, const std::string& src_tex_path, int& out_target_canvas_id){
        throw std::runtime_error("not implemented");
    }

    void TexturePacker::setSaveFilePath(const std::filesystem::path& dir,
                                        const std::string& file_name_without_extension) {
        throw std::runtime_error("not implemented");
    }
} // namespace plateau::texture
