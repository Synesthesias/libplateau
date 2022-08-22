#pragma once

#include <string>
#include <memory>
#include <vector>

namespace plateau::geometry{
    class ModelNode{
    private:
        std::unique_ptr<std::string> name_;
        std::vector<ModelNode> childNodes_;
    };
}