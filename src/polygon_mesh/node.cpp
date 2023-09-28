#include <plateau/polygon_mesh/node.h>
#include <utility>
#include <algorithm>

namespace plateau::polygonMesh {

    Node::Node(std::string name) :
        name_(std::move(name)),
        mesh_(nullptr),
        is_primary_(false){
    }

    Node::Node(std::string name, std::unique_ptr<Mesh>&& mesh) :
        name_(std::move(name)),
        mesh_(std::move(mesh)) {
    }

    Node& Node::addChildNode(Node&& node) {
        child_nodes_.push_back(std::forward<Node>(node));
        return child_nodes_.at(child_nodes_.size()-1);
    }

    void Node::setChildNodes(std::vector<Node>&& child_nodes) {
        child_nodes_ = std::move(child_nodes);
    }

    Node& Node::addEmptyChildNode(const std::string& name) {
        return child_nodes_.emplace_back(name);
    }

    const std::string& Node::getName() const {
        return name_;
    }

    Mesh* Node::getMesh() const {
        return mesh_.get();
    }

    void Node::setMesh(std::unique_ptr<Mesh>&& mesh) {
        mesh_ = std::move(mesh);
    }

    size_t Node::getChildCount() const {
        return child_nodes_.size();
    }

    Node& Node::getChildAt(unsigned int index) {
        return child_nodes_.at(index);
    }

    const Node& Node::getChildAt(unsigned int index) const {
        return child_nodes_.at(index);
    }

    void Node::eraseEmptyChildren() {
        const auto new_end = std::remove_if(child_nodes_.begin(), child_nodes_.end(), [](Node& child) {
            child.eraseEmptyChildren();
            if (child.getChildCount() == 0 && (!child.polygonExists())) return true;
            return false;
        });
        child_nodes_.erase(new_end, child_nodes_.end());
    }

    bool Node::polygonExists() const {
        if (mesh_ == nullptr)
            return false;
        if (mesh_->getVertices().empty())
            return false;
        if (mesh_->getIndices().empty())
            return false;
        return true;
    }

    void Node::debugString(std::stringstream& ss, int indent) const {
        for (int i = 0; i < indent; i++) ss << "    ";
        ss << "Node: " << name_ << std::endl;
        if (mesh_ != nullptr) {
            mesh_->debugString(ss, indent + 1);
        } else {
            for (int i = 0; i < indent + 1; i++) ss << "    ";
            ss << "No Mesh" << std::endl;
        }
        for (const auto& child : child_nodes_) {
            child.debugString(ss, indent + 1);
        }
    }

    void Node::setIsPrimary(bool is_primary) {
        is_primary_ = is_primary;
    }

    bool Node::isPrimary() const {
        return is_primary_;
    }

    void Node::reserveChild(size_t reserve_count) {
        child_nodes_.reserve(reserve_count);
    }
}
