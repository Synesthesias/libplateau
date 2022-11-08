#include <plateau/polygon_mesh/node.h>
#include "plateau/polygon_mesh/model.h"

namespace plateau::polygonMesh {

    Model::Model() :
        root_nodes_() {
    }

    std::shared_ptr<Model> Model::createModel() {
        return std::make_shared<Model>();
    }

    void Model::addNode(Node&& node) {
        root_nodes_.push_back(std::move(node));
    }

    Node& Model::addEmptyNode(const std::string& name) {
        return root_nodes_.emplace_back(name);
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
