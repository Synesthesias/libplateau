#include <plateau/polygon_mesh/transform.h>

#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/matrix_decompose.hpp>

namespace plateau::polygonMesh {

    Transform Transform::apply(const Transform& other) const {
        auto src_matrix = toGlmMatrix();
        auto other_matrix = other.toGlmMatrix();
        auto combined = other_matrix * src_matrix;
        return fromGlmMatrix(combined);
    }

    TVec3d Transform::apply(TVec3d point) const {
        auto matrix = toGlmMatrix();
        auto vec = glm::vec4(point.x, point.y, point.z, 1.0f);
        auto result = matrix * vec;
        return TVec3d(result.x, result.y, result.z);
    }

    glm::mat4x4 Transform::toGlmMatrix() const {
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

    Transform Transform::fromGlmMatrix(glm::mat4x4 matrix) {
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
