#pragma once

#include <plateau/polygon_mesh/transform.h>
#include "transform_impl.h"
#include <vector>

namespace plateau::meshWriter {
    /// ゲームオブジェクトの位置のスタックです。
    /// Nodeの親から子へと処理が進むたびpushし、子から親へ処理が戻るたびpopします。
    /// こうすることで、親から子まで累積的にTransformを適用することができます。
    class TransformStack {
    public:
        void push(const polygonMesh::Transform& transform) {
            stack_.push_back(transform);
        }

        polygonMesh::Transform pop() {
            if (stack_.empty()) {
                throw std::runtime_error("TransformStack is empty.");
            }
            auto transform = stack_.at(stack_.size() - 1);
            stack_.pop_back();
            return transform;
        }

        /// スタック内の全Transformを掛け合わせて積を計算します。
        polygonMesh::Transform CalcProduct() {
            auto current = glm::mat4(1.0f);
            for(int i=stack_.size() - 1; i>=0; i--) {
                current = stack_.at(i).impl_->toGlmMatrix() * current;
            }
            return polygonMesh::Transform::Impl::fromGlmMatrix(current);
        }


    private:
        std::vector<polygonMesh::Transform> stack_;
    };
}
