#include <plateau/polygon_mesh/node.h>
#include <utility>
#include <algorithm>

namespace plateau::polygonMesh {

    Node::Node(std::string name) :
            name_(std::move(name)),
            child_nodes_(),
            mesh_(std::nullopt) {}

    Node::Node(std::string name, Mesh&& mesh) :
            name_(std::move(name)),
            child_nodes_(),
            mesh_(std::move(mesh)) {}

    Node::Node(std::string name, std::optional<Mesh>&& optional_mesh) :
            name_(std::move(name)),
            child_nodes_(),
            mesh_(std::move(optional_mesh)) {}

    void Node::addChildNode(Node&& node) {
        child_nodes_.push_back(std::forward<Node>(node));
    }

    const std::string& Node::getName() const {
        return name_;
    }

    std::optional<Mesh>& Node::getMesh() {
        return mesh_;
    }

    void Node::setMesh(Mesh&& mesh) {
        mesh_ = std::optional<Mesh>(std::move(mesh));
    }

    const std::optional<Mesh>& Node::getMesh() const {
        return mesh_;
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
        auto new_end = std::remove_if(child_nodes_.begin(), child_nodes_.end(), [](Node& child) {
            child.eraseEmptyChildren();
            if (child.getChildCount() == 0 && (!child.polygonExists())) return true;
            return false;
        });
        child_nodes_.erase(new_end, child_nodes_.end());
    }

    bool Node::polygonExists() {
        const auto& mesh_optional = getMesh();
        if (!mesh_optional.has_value()) return false;
        const auto& mesh = mesh_optional.value();
        if (mesh.getVertices().empty()) return false;
        if (mesh.getIndices().empty()) return false;
        return true;
    }

    void Node::debugString(std::stringstream& ss, int indent) const {
        for (int i = 0; i < indent; i++) ss << "    ";
        ss << "Node: " << name_ << std::endl;
        if (mesh_.has_value()) {
            mesh_.value().debugString(ss, indent + 1);
        } else {
            for (int i = 0; i < indent + 1; i++) ss << "    ";
            ss << "No Mesh" << std::endl;
        }
        for (const auto& child: child_nodes_) {
            child.debugString(ss, indent + 1);
        }
    }
}
