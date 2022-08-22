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
        std::string& getName();
        std::optional<Mesh> & getMesh();
        void addChildNode(Node &&node);
        void GetChildrenRecursive(std::vector<Node *> &childVector);
    private:
        std::string name_;
        std::vector<Node> childNodes_;
        std::optional<Mesh> mesh_;
    };
}