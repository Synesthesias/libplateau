#include <plateau/polygon_mesh/node.h>
#include "plateau/polygon_mesh/model.h"
#include <algorithm>

namespace plateau::polygonMesh {

    Model::Model() :
            root_nodes_() {}

    void Model::addNode(Node&& node) {
        root_nodes_.push_back(std::move(node));
    }

    size_t Model::getRootNodeCount() const {
        return root_nodes_.size();
    }

    Node& Model::getRootNodeAt(size_t index) {
        return root_nodes_.at(index);
    }

    const Node& Model::getRootNodeAt(size_t index) const {
        return root_nodes_.at(index);
    }

    void Model::eraseEmptyNodes() {
        auto new_end = std::remove_if(root_nodes_.begin(), root_nodes_.end(), [](Node& node) {
            node.eraseEmptyChildren();
            if (node.getChildCount() == 0 && (!node.polygonExists())) return true;
            return false;
        });
        root_nodes_.erase(new_end, root_nodes_.end());
    }

    std::string Model::debugString() const {
        std::stringstream ss;
        ss << "Model:" << std::endl;
        for (const auto& node: root_nodes_) {
            node.debugString(ss, 1);
        }
        return ss.str();
    }
}
