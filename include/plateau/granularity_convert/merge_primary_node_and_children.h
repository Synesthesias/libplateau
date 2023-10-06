#pragma once
#include <plateau/polygon_mesh/node.h>

namespace plateau::granularityConvert {
    class MergePrimaryNodeAndChildren {
    public:
        void merge(const plateau::polygonMesh::Node& src_node_arg, plateau::polygonMesh::Mesh& dst_mesh, int primary_id);
    };
}
