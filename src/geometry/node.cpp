#include <plateau/geometry/node.h>

#include <utility>

using namespace plateau::geometry;

Node::Node(std::string name) :
    name_(std::move(name)),
    childNodes_(),
    mesh_(std::nullopt)
{}

Node::Node(std::string name, Mesh mesh):
    name_(std::move(name)),
    childNodes_(),
    mesh_(mesh) // TODO 本当はmoveにしたい？
{
}

Node::Node(): Node("") {
}

void Node::addChildNode(const Node& node) {
    // TODO ここがコピーになっているので効率が悪い？
    childNodes_.push_back(node);
}

const std::string &Node::getName() const {
    return name_;
}

std::optional<Mesh> & Node::getMesh(){
    return mesh_;
}

int Node::getChildCount() const {
    return (int)childNodes_.size();
}

Node & Node::getChildAt(int index) {
    return childNodes_.at(index);
}

const Node& Node::getConstChildAt(int index) const{
    return childNodes_.at(index);
}
