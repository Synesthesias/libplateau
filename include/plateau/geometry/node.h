#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "mesh.h"

namespace plateau::geometry{
    /**
     * Model 以下の階層構造を構成するノードです。
     * Node は 0個以上の 子Node を持つため階層構造になります。
     *
     * 詳しくは Model クラスのコメントをご覧ください。
     *
     * Node::name_ はゲームエンジン側ではゲームオブジェクトの名前として解釈されることが想定され、
     * Node::mesh_ はそのゲームオブジェクトの持つメッシュとして解釈されることが想定されます。
     */
    class LIBPLATEAU_EXPORT Node{
    public:
        explicit Node(std::string name);

        /// メッシュは move で渡すことを想定しています。
        Node(std::string name, Mesh &&mesh);
        Node(std::string name, std::optional<Mesh> &&optionalMesh);

        /// コピーを禁止します。そうでないとメッシュまでコピーされて重くなります。
        /// ムーブのみ許可します。
        Node(const Node& node) = delete;
        Node& operator=(const Node& node) = delete;
        Node(Node&& node) = default;
        Node& operator=(Node&& node) = default;

        [[nodiscard]] const std::string& getName() const;
        [[nodiscard]] std::optional<Mesh> & getMesh();
        void addChildNode(Node &&node);
        [[nodiscard]] size_t getChildCount() const;

        // TODO const版とそうでない版で2通りあるけど、まとめる方法がある気がする
        [[nodiscard]] Node & getChildAt(int index);
        [[nodiscard]] const Node &getConstChildAt(int index) const;
    private:
        std::string name_;
        std::vector<Node> childNodes_;
        std::optional<Mesh> mesh_;
    };
}