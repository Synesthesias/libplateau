#pragma once
#include <plateau/polygon_mesh/model.h>

/// ルートノードから何番目の子をたどっていけばノードに行き着くかをvector<int>で表現したノードパスです。
class NodePath {
public:
    NodePath(std::vector<int> positions) : positions_(std::move(positions)) {};

    plateau::polygonMesh::Node* toNode(plateau::polygonMesh::Model* model) const {
        if (positions_.empty()) return nullptr;
        auto node = &model->getRootNodeAt(positions_.at(0));
        for (int i = 1; i < positions_.size(); i++) {
            node = &node->getChildAt(positions_.at(i));
        }
        return node;
    };

    NodePath parent() const {
        auto new_pos = std::vector(positions_.begin(), positions_.end() - 1);
        return {new_pos};
    }

    NodePath plus(int next_index) const {
        auto new_pos = std::vector(positions_);
        new_pos.push_back(next_index);
        return new_pos;
    }

    NodePath next() const {
        auto ret = NodePath(positions_);
        ret.positions_.at(ret.positions_.size() - 1) = ret.positions_.at(ret.positions_.size() - 1) + 1;
        return ret;
    }

    bool empty() const {
        return positions_.empty();
    }

    plateau::polygonMesh::Node& addChildNode(plateau::polygonMesh::Node&& node, plateau::polygonMesh::Model* model) const {
        if (positions_.empty()) {
            return model->addNode(std::move(node));
        }
        return toNode(model)->addChildNode(std::move(node));
    }

    // パス中にプライマリノードがあればそのパスを返し、なければ空のNodePathを返します。
    // パス中に複数ある場合は、最後のものを返します。
    NodePath searchLastPrimaryNodeInPath(plateau::polygonMesh::Model* model) {
        int found_path_pos = -1;
        if(positions_.empty()) return NodePath({});
        if(model->getRootNodeCount() >= positions_.at(0)) return NodePath({});
        auto node = &model->getRootNodeAt(positions_.at(0));
        if(node->isPrimary()) found_path_pos = 0;
        for(int i=1; i<positions_.size(); i++) {
            if(positions_.at(i) >= node->getChildCount()) break;
            node = &node->getChildAt(positions_.at(i));
            if(node->isPrimary()) found_path_pos = i;
        }
        if(found_path_pos < 0) return NodePath({});
        auto ret_path = NodePath(std::vector<int>(positions_.begin(), positions_.begin() + found_path_pos + 1));
        return ret_path;
    }

    bool operator==(const NodePath& other) const {
        return positions_ == other.positions_;
    }

private:
    std::vector<int> positions_;
};
