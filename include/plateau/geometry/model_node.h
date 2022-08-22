#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>
#include "mesh.h"

namespace plateau::geometry{
    class ModelNode{
    public:
        explicit ModelNode(std::string& name);
    private:
        std::string name_;
        std::vector<ModelNode> childNodes_;
        std::optional<Mesh> mesh_;
    };
}