#include "libplateau_c.h"
#include <plateau/mesh_writer/fbx_writer.h>
#include <plateau/polygon_mesh/model.h>

using namespace plateau::meshWriter;
using namespace plateau::polygonMesh;
extern "C" {
DLL_4_ARG_FUNC(plateau_fbx_writer_write,
               const char* const fbx_file_path_utf8,
               const Model* const model,
               FbxWriteOptions options,
               bool* out_succeed,
               *out_succeed = FbxWriter().write(fbx_file_path_utf8, *model, options))
}
