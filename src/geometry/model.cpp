#include <plateau/geometry/node.h>
#include "plateau/geometry/model.h"

namespace plateau::geometry {

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
}
