#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "mesh.h"

namespace plateau::geometry{
    class Node{
    public:
        explicit Node(std::string& name);
    private:
        std::string name_;
        std::vector<Node> childNodes_;
        std::optional<Mesh> mesh_;
    };
}