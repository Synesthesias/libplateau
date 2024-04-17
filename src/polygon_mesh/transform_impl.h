#pragma once

#include <glm.hpp>
#include <plateau/polygon_mesh/transform.h>
#include <plateau/polygon_mesh/quaternion.h>

namespace plateau::polygonMesh {

    /// Transformのうち、ライブラリの利用者からは隠蔽したい部分です
    class Transform::Impl {
    public:
        Impl(const TVec3d& local_position, const TVec3d& local_scale, const Quaternion& local_rotation) :
                local_position_(local_position),
                local_scale_(local_scale),
                local_rotation_(local_rotation) {}

        /// Transformを、GLMライブラリの変換行列にして返します。
        glm::mat4x4 toGlmMatrix() const;

        /// GLMライブラリの変換行列からTransformを作って返します。
        static Transform fromGlmMatrix(glm::mat4x4 matrix);

        TVec3d local_position_;
        TVec3d local_scale_;
        Quaternion local_rotation_;

    };
}
