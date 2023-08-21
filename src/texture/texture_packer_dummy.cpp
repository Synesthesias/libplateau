
#include <cassert>
#include <plateau/texture/texture_packer.h>
#include <plateau/polygon_mesh/model.h>

#include <string>
#include <vector>

namespace plateau::texture {
    TexturePacker::~TexturePacker() = default;

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
} // namespace plateau::texture
