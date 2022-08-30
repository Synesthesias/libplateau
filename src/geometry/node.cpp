#include <plateau/geometry/node.h>
#include <utility>

using namespace plateau::geometry;

Node::Node(std::string name) :
    name_(std::move(name)),
    childNodes_(),
    mesh_(std::nullopt)
    {}

Node::Node(std::string name, Mesh &&mesh):
    name_(std::move(name)),
    childNodes_(),
    mesh_(std::forward<Mesh>(mesh))
    {}

Node::Node(std::string name, std::optional<Mesh> &&optionalMesh):
    name_(std::move(name)),
    childNodes_(),
    mesh_(std::forward<std::optional<Mesh>>(optionalMesh))
    {}

void Node::addChildNode(Node &&node) {
    childNodes_.push_back(std::forward<Node>(node));
}

const std::string& Node::getName() const {
    return name_;
}

std::optional<Mesh>& Node::getMesh(){
    return mesh_;
}

const std::optional<Mesh>& Node::getMesh() const{
    return mesh_;
}

size_t Node::getChildCount() const {
    return childNodes_.size();
}

Node& Node::getChildAt(unsigned int index) {
    return childNodes_.at(index);
}

const Node& Node::getChildAt(unsigned int index) const{
    return childNodes_.at(index);
}
