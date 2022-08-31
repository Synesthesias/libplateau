#include <plateau/geometry/node.h>
#include <utility>

using namespace plateau::geometry;

Node::Node(std::string name) :
        name_(std::move(name)),
        child_nodes_(),
        mesh_(std::nullopt) {}

Node::Node(std::string name, Mesh&& mesh) :
        name_(std::move(name)),
        child_nodes_(),
        mesh_(std::forward<Mesh>(mesh)) {}

Node::Node(std::string name, std::optional<Mesh>&& optional_mesh) :
        name_(std::move(name)),
        child_nodes_(),
        mesh_(std::forward<std::optional<Mesh>>(optional_mesh)) {}

void Node::addChildNode(Node&& node) {
    child_nodes_.push_back(std::forward<Node>(node));
}

const std::string& Node::getName() const {
    return name_;
}

std::optional<Mesh>& Node::getMesh() {
    return mesh_;
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
