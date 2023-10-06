#include <plateau/granularity_convert/node_path.h>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    NodePath::NodePath(std::vector<int> positions) : positions_(std::move(positions)) {};

    plateau::polygonMesh::Node* NodePath::toNode(plateau::polygonMesh::Model* model) const {
        if (positions_.empty()) return nullptr;
        auto node = &model->getRootNodeAt(positions_.at(0));
        for (int i = 1; i < positions_.size(); i++) {
            node = &node->getChildAt(positions_.at(i));
        }
        return node;
    };

    NodePath NodePath::parent() const {
        auto new_pos = std::vector(positions_.begin(), positions_.end() - 1);
        return {new_pos};
    }

    NodePath NodePath::plus(int next_index) const {
        auto new_pos = std::vector(positions_);
        new_pos.push_back(next_index);
        return new_pos;
    }

    bool NodePath::empty() const {
        return positions_.empty();
    }

    Node& NodePath::addChildNode(Node&& node, Model* model) const {
        if (positions_.empty()) {
            return model->addNode(std::move(node));
        }
        return toNode(model)->addChildNode(std::move(node));
    }

    NodePath NodePath::searchLastPrimaryNodeInPath(Model* model) {
        int found_path_pos = -1;
        if (positions_.empty()) return NodePath({});
        if (model->getRootNodeCount() >= positions_.at(0)) return NodePath({});
        auto node = &model->getRootNodeAt(positions_.at(0));
        if (node->isPrimary()) found_path_pos = 0;
        for (int i = 1; i < positions_.size(); i++) {
            if (positions_.at(i) >= node->getChildCount()) break;
            node = &node->getChildAt(positions_.at(i));
            if (node->isPrimary()) found_path_pos = i;
        }
        if (found_path_pos < 0) return NodePath({});
        auto ret_path = NodePath(std::vector<int>(positions_.begin(), positions_.begin() + found_path_pos + 1));
        return ret_path;
    }

    bool NodePath::operator==(const NodePath& other) const {
        return positions_ == other.positions_;
    }
}
