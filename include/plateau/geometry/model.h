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
        Node & addNode(const Node& node);
        std::vector<Node*> getNodesRecursive();
        [[nodiscard]] int getRootNodesCount() const;
        [[nodiscard]] const Node& getRootNodeAt(int index) const;
    private:
        std::vector<Node> rootNodes_;
    };

}