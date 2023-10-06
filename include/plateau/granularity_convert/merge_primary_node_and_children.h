#pragma once
#include <plateau/polygon_mesh/node.h>

namespace plateau::granularityConvert {
    class MergePrimaryNodeAndChildren {
    public:
        /// 主要地物のノードとその子ノードを結合したものを、引数dst_meshに格納します。
        /// 引数に渡されるsrc_node_argはPrimaryであることを前提とします。
        void merge(const plateau::polygonMesh::Node& src_node_arg, plateau::polygonMesh::Mesh& dst_mesh, int primary_id);
    };
}
