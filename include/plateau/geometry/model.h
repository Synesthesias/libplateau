#pragma once
#include <plateau/geometry/model.h>
#include <plateau/geometry/node.h>
#include <libplateau_api.h>
namespace plateau::geometry{

    /**
     * Model は GMLファイルパーサーから読み取った3Dメッシュ情報を各ゲームエンジンに渡すための中間データ構造として設計されており、
     * そのデータ構造の階層のトップに位置します。
     *
     * 中間データ構造の階層 :
     * Model -> 所有(0個以上) -> Node(階層構造) -> 所有(0or1個) -> Mesh -> 所有(1個以上) -> SubMesh
     *
     * GMLファイルの内容を元に、Unity や Unreal Engine でメッシュやゲームオブジェクトを生成するために必要な情報が入るよう意図されています。
     * Model が所有する Node の階層関係は、ゲームエンジン側でのゲームオブジェクトの階層関係に対応します。
     * Node が所有する Mesh は、そのゲームオブジェクトが保持する3Dメッシュに対応します。
     * Mesh が所有する SubMesh は、そのメッシュのサブメッシュ（テクスチャパスを含む）に対応します。
     */
    class LIBPLATEAU_EXPORT Model{
    public:
        Model();
        Node & addNode(const Node& node);
        [[nodiscard]] int getRootNodesCount() const;
        Node & getRootNodeAt(int index);
        [[nodiscard]] const Node& getRootNodeAt(int index) const;
    private:
        std::vector<Node> rootNodes_;
    };

}