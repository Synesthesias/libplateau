#include <plateau/granularity_convert/node_path.h>

namespace plateau::granularityConvert {
    using namespace plateau::polygonMesh;

    NodePath::NodePath(std::vector<int> positions) : positions_(std::move(positions)) {};

    const plateau::polygonMesh::Node* NodePath::toNode(const plateau::polygonMesh::Model* model) const {
        if (positions_.empty()) return nullptr;
        auto node = &model->getRootNodeAt(positions_.at(0));
        for (int i = 1; i < positions_.size(); i++) {
            node = &node->getChildAt(positions_.at(i));
        }
        return node;
    };

    plateau::polygonMesh::Node* NodePath::toNode(plateau::polygonMesh::Model* model) {
        return const_cast<Node*>(std::as_const(*this).toNode(model));
    }

    NodePath NodePath::parent() const {
        if(positions_.size() <= 1) NodePath({});
        auto new_pos = std::vector(positions_.begin(), positions_.end() - 1);
        return {new_pos};
    }

    NodePath NodePath::plus(int next_index) const {
        auto new_pos = std::vector(positions_);
        new_pos.push_back(next_index);
        return new_pos;
    }

    NodePath NodePath::decrement() const {
        auto new_pos = std::vector(positions_);
        new_pos.at(new_pos.size()-1)--;
        return new_pos;
    }

    bool NodePath::empty() const {
        return positions_.empty();
    }

    NodePath NodePath::addChildNode(Node&& node, Model* model) {
        if (positions_.empty()) {
            model->addNode(std::move(node));
            return NodePath({(int)model->getRootNodeCount() - 1});
        }
        auto* parent = toNode(model);
        parent->addChildNode(std::move(node));
        return plus((int)parent->getChildCount() - 1);
    }

    NodePath NodePath::searchLastPrimaryNodeInPath(const Model* model) const {
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
