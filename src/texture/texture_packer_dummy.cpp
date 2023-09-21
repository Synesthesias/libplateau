
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
        // Do nothing
        return;
    }

    void TexturePacker::process(plateau::polygonMesh::Model& model) {
        // Do nothing
        return;
    }

    void TexturePacker::processNodeRecursive(const plateau::polygonMesh::Node& node) {
        return;
    }

    void TexturePacker::processMesh(plateau::polygonMesh::Mesh* mesh) {
        return;
    }

    void TexturePacker::flush(){
        return;
    }

    AtlasInfo TexturePacker::packImage(TextureImageBase* image, const std::string& src_tex_path, int& out_target_canvas_id){
        return AtlasInfo::empty();
    }

    void TexturePacker::setSaveFilePath(const std::filesystem::path& dir,
                                        const std::string& file_name_without_extension) {
        return;
    }
} // namespace plateau::texture
