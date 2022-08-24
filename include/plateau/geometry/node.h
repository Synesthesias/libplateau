#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "mesh.h"

namespace plateau::geometry{
    /**
     * 見た目に関する情報であり、 Model 以下の階層構造を構成するノードです。
     * ノードは名前、メッシュ、子Nodeを持ちます。
     * メッシュは頂点、UV、テクスチャを持ちます。
     */
    struct LIBPLATEAU_EXPORT Node{
    public:
        Node();
        explicit Node(std::string name);
        Node(std::string name, Mesh mesh);
        [[nodiscard]] const std::string& getName() const;
        [[nodiscard]] std::optional<Mesh> & getMesh();
        void addChildNode(const Node& node);
        void GetChildrenRecursive(std::vector<Node *> &childVector);
        [[nodiscard]] int getChildCount() const;

        // TODO const版とそうでない版で2通りあるけど、まとめる方法がある気がする
        [[nodiscard]] Node & getChildAt(int index);
        [[nodiscard]] const Node &getConstChildAt(int index) const;
    private:
        std::string name_;
        std::vector<Node> childNodes_;
        std::optional<Mesh> mesh_;
    };
}