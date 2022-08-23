#include <plateau/geometry/node.h>

#include <utility>

using namespace plateau::geometry;

Node::Node(std::string name) :
    name_(std::move(name)),
    childNodes_(),
    mesh_(std::nullopt)
{}

Node::Node(std::string name, Mesh mesh):
    name_(name),
    childNodes_(),
    mesh_(std::move(mesh))
{
}

Node::Node(): Node("") {
}

void Node::addChildNode(Node node) {
    childNodes_.push_back(node);
}

void Node::GetChildrenRecursive(std::vector<Node *> &childVector) {
    childVector.push_back(this);
    for(auto& child : childNodes_){
        child.GetChildrenRecursive(childVector);
    }
}

const std::string &Node::getName() const {
    return name_;
}

std::optional<Mesh> & Node::getMesh() {
    return mesh_;
}

int Node::getChildCount() const {
    return childNodes_.size();
}

const Node &Node::getChildAt(int index) const {
    return childNodes_.at(index);
}
