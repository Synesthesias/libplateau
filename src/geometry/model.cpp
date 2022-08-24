#include <plateau/geometry/node.h>
#include "plateau/geometry/model.h"


using namespace plateau::geometry;


Model::Model() :
        rootNodes_()
{

}

Node & Model::addNode(const Node& node) { // TODO moveにしたほうが早いか？
    rootNodes_.push_back(node);
    return rootNodes_.back();
}

std::vector<Node *> Model::getNodesRecursive() {
    auto children =  std::vector<Node *>();
    for(auto& node : rootNodes_){
        node.GetChildrenRecursive(children);
    }
    return children;
}

int Model::getRootNodesCount() const {
    return (int)rootNodes_.size();
}

Node & Model::getRootNodeAt(int index){
    return rootNodes_.at(index);
}

const Node &Model::getRootNodeAt(int index) const {
    return rootNodes_.at(index);
}
