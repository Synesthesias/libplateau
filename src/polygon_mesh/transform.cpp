#include <plateau/polygon_mesh/transform.h>
#include "transform_impl.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/matrix_decompose.hpp>

namespace plateau::polygonMesh {

    Transform::Transform() :
            Transform(
                    TVec3d(0.0, 0.0, 0.0),
                    TVec3d(1.0, 1.0, 1.0),
                    Quaternion()) {}

    Transform::Transform(const TVec3d& local_position, const TVec3d& local_scale, const Quaternion& local_rotation) :
            impl_(std::make_shared<Impl>(local_position, local_scale, local_rotation)) {}

    TVec3d Transform::getLocalPosition() const { return impl_->local_position_; }
    TVec3d Transform::getLocalScale() const {return impl_->local_scale_; }
    Quaternion Transform::getLocalRotation() const { return impl_->local_rotation_; }
    void Transform::setLocalPosition(TVec3d local_position) { impl_->local_position_ = local_position; }
    void Transform::setLocalScale(TVec3d local_scale) { impl_->local_scale_ = local_scale; }
    void Transform::setLocalRotation(Quaternion local_rotation) { impl_->local_rotation_ = local_rotation; }

    Transform Transform::apply(const Transform& other) const {
        auto src_matrix = impl_->toGlmMatrix();
        auto other_matrix = other.impl_->toGlmMatrix();
        auto combined = other_matrix * src_matrix;
        return impl_->fromGlmMatrix(combined);
    }

    TVec3d Transform::apply(TVec3d point) const {
        auto matrix = impl_->toGlmMatrix();
        auto vec = glm::vec4(point.x, point.y, point.z, 1.0f);
        auto result = matrix * vec;
        return TVec3d(result.x, result.y, result.z);
    }

    glm::mat4x4 Transform::Impl::toGlmMatrix() const {
        // 平行移動行列
        auto translation = glm::translate(glm::mat4(1.0f), glm::vec3(local_position_.x, local_position_.y, local_position_.z));
        // 回転行列
        auto quaternion = glm::quat(local_rotation_.getW(), local_rotation_.getX(), local_rotation_.getY(), local_rotation_.getZ());
        auto rotation = glm::toMat4(quaternion);
        // 拡大縮小行列
        auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(local_scale_.x, local_scale_.y, local_scale_.z));
        // 上記を組み合わせ
        auto matrix = translation * rotation * scale;
        return matrix;
    }

    Transform Transform::Impl::fromGlmMatrix(glm::mat4x4 matrix) {
        glm::vec3 scale, position, skew;
        glm::quat rotation;
        glm::vec4 perspective;
        glm::decompose(matrix, scale, rotation, position, skew, perspective);
        return Transform(
                TVec3d(position.x, position.y, position.z),
                TVec3d(scale.x, scale.y, scale.z),
                Quaternion(rotation.x, rotation.y, rotation.z, rotation.w)
        );
    }
}
