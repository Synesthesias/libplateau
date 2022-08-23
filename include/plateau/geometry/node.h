#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "mesh.h"

namespace plateau::geometry{
    class LIBPLATEAU_EXPORT Node{
    public:
        Node();
        explicit Node(std::string &&name);
        Node(std::string &&name, Mesh &&mesh);
        [[nodiscard]] const std::string& getName() const;
        std::optional<Mesh> & getMesh();
        void addChildNode(Node &&node);
        void GetChildrenRecursive(std::vector<Node *> &childVector);
        [[nodiscard]] int getChildCount() const;
        [[nodiscard]] const Node& getChildAt(int index) const;
    private:
        std::string name_;
        std::vector<Node> childNodes_;
        std::optional<Mesh> mesh_;
    };
}