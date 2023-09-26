#include <plateau/polygon_mesh/node.h>
#include "plateau/polygon_mesh/model.h"
#include <algorithm>

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

    namespace {
        void getAllMeshesRecursive(std::vector<Mesh*>& meshes, const Node& node) {
            if(node.polygonExists()) {
                meshes.push_back(node.getMesh());
            }
            auto child_count = node.getChildCount();
            for(int i=0; i<child_count; i++) {
                auto& child = node.getChildAt(i);
                getAllMeshesRecursive(meshes, child);
            }
        }
    }

    std::vector<Mesh*> Model::getAllMeshes() const {
        std::vector<Mesh*> meshes;
        auto root_node_count = getRootNodeCount();
        for(int i=0; i<root_node_count; i++){
            auto& node = getRootNodeAt(i);
            getAllMeshesRecursive(meshes, node);
        }
        return meshes;
    }

    void Model::reserveRootNodes(size_t reserve_count) {
        root_nodes_.reserve(reserve_count);
    }
}
