#include <stdexcept>
#include <filesystem>
#include <cassert>

#include <plateau/mesh_writer/fbx_writer.h>

//#include <fbxsdk.h>
#include <set>

namespace fs = std::filesystem;

namespace plateau::meshWriter {
    bool FbxWriter::write(const std::string& fbx_file_path, const plateau::polygonMesh::Model& model, const FbxWriteOptions& options) {
        return false;
    }
}
