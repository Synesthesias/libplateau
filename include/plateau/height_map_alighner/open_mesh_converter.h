#pragma once
#define _USE_MATH_DEFINES
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>
#include <plateau/polygon_mesh/mesh.h>

namespace plateau::heightMapAligner {

    typedef OpenMesh::TriMesh_ArrayKernelT<> MeshType;

    /// plateauのメッシュとOpenMeshのメッシュを相互変換します。
    class OpenMeshConverter {
    public:
        MeshType toOpenMesh(const plateau::polygonMesh::Mesh* mesh);
        /// OpenMeshのメッシュをplateauのメッシュに変換します。引数のplateauMeshに対して破壊的に変更します。
        void toPlateauMesh(plateau::polygonMesh::Mesh* p_mesh, const MeshType& om_mesh);
    };
}
