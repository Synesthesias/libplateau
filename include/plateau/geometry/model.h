#pragma once
#include <plateau/geometry/model.h>
#include <plateau/geometry/node.h>
#include <libplateau_api.h>
namespace plateau::geometry{

    /**
     * GMLファイルをパースして得られるもののうち、ゲームエンジンに渡すべき見た目に関する情報です。
     * すなわち、3Dメッシュ、テクスチャ参照などを階層構造（Nodeの繋がり）で保持したものです。
     */
    class LIBPLATEAU_EXPORT Model{
    public:
        Model();
        Node & addNode(Node &&node);
        std::vector<Node*> GetNodesRecursive();
    private:
        std::vector<Node> rootNodes;
    };

}