#pragma once

#include <string>

#include <citygml/citygml.h>
#include <libplateau_api.h>
#include <stack>

#include <plateau/polygon_mesh/mesh_extractor.h>

namespace plateau::meshWriter {

    /// ゲームオブジェクトの位置
    class Transform {
    public:
        Transform(const TVec3d& local_position) : local_position_(local_position) {}
        TVec3d getLocalPosition() const { return local_position_; }
    private:
        TVec3d local_position_;
    };

    /// ゲームオブジェクトの位置のスタックです。
    /// Nodeの親から子へと処理が進むたびpushし、子から親へ処理が戻るたびpopします。
    /// こうすることで、親から子まで累積的にTransformを適用することができます。
    class TransformStack {
    public:
        void push(const Transform& transform) {
            stack_.push(transform);
            sum_of_position = sum_of_position + transform.getLocalPosition();
        }

        Transform pop() {
            if (stack_.empty()) {
                throw std::runtime_error("TransformStack is empty.");
            }
            auto transform = stack_.top();
            stack_.pop();
            sum_of_position = sum_of_position - transform.getLocalPosition();
            return transform;
        }

        TVec3d getSumOfPosition() const {
            return sum_of_position;
        }

    private:
        std::stack<Transform> stack_;
        TVec3d sum_of_position = TVec3d(0,0,0);
    };

    /// ModelをもとにOBJファイルを書き出すクラスです。
    class LIBPLATEAU_EXPORT ObjWriter {
    public:
        ObjWriter() :
            v_offset_(0), uv_offset_(0), required_materials_() {
        }

        bool write(const std::string& obj_file_path, const plateau::polygonMesh::Model& model);

    private:
        // OBJ書き出し
        void writeObj(const std::string& obj_file_path, const plateau::polygonMesh::Node& node,
                      TransformStack& transform_stack);
        void writeCityObjectRecursive(std::ofstream& ofs, const plateau::polygonMesh::Node& node, TransformStack& transform_stack);
        void writeCityObject(std::ofstream& ofs, const plateau::polygonMesh::Node& node, TransformStack& transform_stack);
        static void writeVertices(std::ofstream& ofs, const std::vector<TVec3d>& vertices, TransformStack& transform_stack);
        void writeIndicesWithUV(std::ofstream& ofs, const std::vector<unsigned int>& indices) const;
        static void writeUVs(std::ofstream& ofs, const std::vector<TVec2f>& uvs);
        void writeMaterialReference(std::ofstream& ofs, const std::string& texUrl);

        // MTL書き出し
        void writeMtl(const std::string& obj_file_path);


        unsigned v_offset_, uv_offset_;
        std::map<std::string, std::string> required_materials_;

    };
}
