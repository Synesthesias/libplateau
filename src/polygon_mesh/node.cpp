#include <plateau/polygon_mesh/node.h>
#include <utility>

namespace plateau::polygonMesh {

    Node::Node(const std::string& name) :
        name_(name),
        child_nodes_(),
        mesh_(std::nullopt) {
    }

    Node::Node(std::string name, Mesh&& mesh) :
        name_(std::move(name)),
        child_nodes_(),
        mesh_(std::move(mesh)) {
    }

    Node::Node(std::string name, std::optional<Mesh>&& optional_mesh) :
        name_(std::move(name)),
        child_nodes_(),
        mesh_(std::move(optional_mesh)) {
    }

    void Node::addChildNode(Node&& node) {
        child_nodes_.push_back(std::forward<Node>(node));
    }

    Node& Node::addEmptyChildNode(const std::string& name) {
        return child_nodes_.emplace_back(name);
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
}
