#include <plateau/geometry/node.h>
#include "plateau/geometry/model.h"

using namespace plateau::geometry;

Model::Model() :
        rootNodes_(){}

void Model::addNode(const Node& node) { // TODO moveにしたほうが早いか？
    rootNodes_.push_back(node);
}

size_t Model::getRootNodesCount() const {
    return rootNodes_.size();
}

Node& Model::getRootNodeAt(size_t index){
    return rootNodes_.at(index);
}

const Node &Model::getRootNodeAt(size_t index) const {
    return rootNodes_.at(index);
}
