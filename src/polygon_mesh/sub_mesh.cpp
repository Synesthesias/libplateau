#include<plateau/polygon_mesh/sub_mesh.h>
#include <utility>
#include <stdexcept>
#include <sstream>

namespace plateau::polygonMesh {

    SubMesh::SubMesh(size_t start_index, size_t end_index, const std::string& texture_path, std::shared_ptr<const citygml::Material> material) :
            start_index_(start_index),
            end_index_(end_index),
            texture_path_(texture_path) ,
            material_(material),
            game_material_id_(-1){}

    SubMesh::SubMesh(size_t start_index, size_t end_index, const std::string& texture_path,
                     std::shared_ptr<const citygml::Material> material, int game_material_id) :
            SubMesh(start_index, end_index, texture_path, std::move(material)) {
        game_material_id_ = game_material_id;
    }


    void
    SubMesh::addSubMesh(size_t start_index, size_t end_index, const std::string& texture_path, std::shared_ptr<const citygml::Material> material, int game_material_id, std::vector<SubMesh>& vector) {
        if (end_index <= start_index) throw std::logic_error("addSubMesh : Index is invalid.");
        vector.emplace_back(start_index, end_index, texture_path, material, game_material_id);
    }

    size_t SubMesh::getStartIndex() const {
        return start_index_;
    }

    size_t SubMesh::getEndIndex() const {
        return end_index_;
    }

    const std::string& SubMesh::getTexturePath() const {
        return texture_path_;
    }

    std::shared_ptr<const citygml::Material> SubMesh::getMaterial() const {
        return material_;
    }

    void SubMesh::setTexturePath(std::string file_path) {
        texture_path_ = std::move(file_path);
    }    

    void SubMesh::setStartIndex(size_t start_index) {
        start_index_ = start_index;
    }
    void SubMesh::setEndIndex(size_t end_index) {
        end_index_ = end_index;
    }

    void SubMesh::setGameMaterialID(int id) {
        game_material_id_ = id;
    }

    int SubMesh::getGameMaterialID() const {
        return game_material_id_;
    }

    bool SubMesh::operator==(const SubMesh& other) const{
        bool ret = start_index_ == other.start_index_ &&
                   end_index_ == other.end_index_ &&
                   texture_path_ == other.texture_path_ &&
                   material_.get() == other.material_.get();
        return ret;

    }

    void SubMesh::debugString(std::stringstream& ss, int indent) const {
        for (int i = 0; i < indent; i++) ss << "    ";
        ss << "SubMesh: [" << start_index_ << ", " << end_index_ << "] texturePath = '" << texture_path_ << "'"
           << std::endl;
    }
}
