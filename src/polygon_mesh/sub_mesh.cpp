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

    bool SubMesh::isSameAs(const SubMesh& other) const {
        // ゲームマテリアルがあるなら、それ同士の比較
        if(game_material_id_ >= 0 || other.game_material_id_ >= 0) {
            return game_material_id_ == other.game_material_id_;
        }
        // materialがあるなら、それ同士の比較
        if(material_ != nullptr && other.material_ != nullptr) {
            return material_->getId() == other.material_->getId();
        }
        if(material_ == nullptr && other.material_ != nullptr || (material_ != nullptr && other.material_ == nullptr)) {
            return false;
        }
        // 最後にテクスチャパスの比較
        return texture_path_ == other.texture_path_;
    }

    bool SubMesh::operator==(const SubMesh& other) const {
        bool ret = start_index_ == other.start_index_ &&
                   end_index_ == other.end_index_ &&
                   isAppearanceEqual(other);
        return ret;

    }


    namespace {
        bool compareTVec3f(const TVec3f& l, const TVec3f& r) {
            // lがrより小さいときにtrueを返します。
            constexpr float epsilon = 0.0001f;
            if(std::abs(l.x - l.y) > epsilon) return l.x < r.x;
            if(std::abs(l.y - l.y) > epsilon) return l.y < r.y;
            if(std::abs(l.z - l.z) > epsilon) return l.z < r.z;
            return false; // areEqualTVec3f() がtrueを返すときにこの行が実行されるようにします。
        }

        bool areEqualTVec3f(const TVec3f& l, const TVec3f& r) {
            // compareTVec3f() の最後の行のreturn falseが実行されるケースを記述します。
            constexpr float epsilon = 0.0001f;
            return std::abs(l.x - r.x) <= epsilon && std::abs(l.y - r.y) <= epsilon && std::abs(l.z - r.z) <= epsilon;
        }
    }

    bool SubMesh::isAppearanceEqual(const plateau::polygonMesh::SubMesh& other) const {

        // 留意: ここを編集するときは、SubMeshCompareByAppearance::operator() も対応するように編集してください。
        if(getGameMaterialID() != other.getGameMaterialID()) return false;

        // ゲームマテリアルIDがセットされているなら、他のすべてに優先してこれだけで比較します。
        if(getGameMaterialID() >= 0){
            return true;
        }


        if(getTexturePath() != other.getTexturePath()) return false;
        const auto mat_s = getMaterial();
        const auto mat_o = other.getMaterial();
        if(mat_s.get() == mat_o.get()) return true;
        if(!mat_s && mat_o) return false;
        if(mat_s && !mat_o) return false;
        if(mat_s->getDiffuse() != mat_o->getDiffuse()) return false;
        if(mat_s->getEmissive() != mat_o->getEmissive()) return false;
        if(mat_s->getSpecular() != mat_o->getSpecular()) return false;
        if(mat_s->getAmbientIntensity() != mat_o->getAmbientIntensity()) return false;
        if(mat_s->getShininess() != mat_o->getShininess()) return false;
        if(mat_s->getTransparency() != mat_o->getTransparency()) return false;
        if(mat_s->isSmooth() != mat_o->isSmooth()) return false;
        return true;
    }

    bool SubMeshCompareByAppearance::operator()(const plateau::polygonMesh::SubMesh& lhs,
                                                const plateau::polygonMesh::SubMesh& rhs) const {

        // 留意: ここを編集するときは、SubMesh::isAppearanceEqual も対応するように編集してください。
        //      lhsがrhsよりも小さい場合にtrueを返します。等しい場合はfalseです。

        // gameMaterialの比較
        if(lhs.getGameMaterialID() != rhs.getGameMaterialID()) return lhs.getGameMaterialID() < rhs.getGameMaterialID();

        // GameMaterialIDがセットされている場合、他のすべてに優先してこれだけでの比較とします。
        if(lhs.getGameMaterialID() >= 0) return false; // 等しい

        // テクスチャパスの比較
        const auto& tex_path_l = lhs.getTexturePath();
        const auto& tex_path_r = rhs.getTexturePath();
        if(tex_path_l != tex_path_r) {
            return tex_path_l < tex_path_r;
        }

        // 数値上のマテリアルの比較
        constexpr float epsilon = 0.0001f;
        const auto mat_l = lhs.getMaterial();
        const auto mat_r = rhs.getMaterial();
        if(!mat_l && !mat_r) return false;
        if(mat_l && !mat_r) return false;
        if(!mat_l && mat_r) return true;
        const auto diffuse_l = mat_l->getDiffuse();
        const auto diffuse_r = mat_r->getDiffuse();
        if(!areEqualTVec3f(diffuse_l, diffuse_r)) return compareTVec3f(diffuse_l, diffuse_r);
        const auto emissive_l = mat_l->getEmissive();
        const auto emissive_r = mat_r->getEmissive();
        if(!areEqualTVec3f(emissive_l, emissive_r)) return compareTVec3f(emissive_l, emissive_r);
        const auto specular_l = mat_l->getSpecular();
        const auto specular_r = mat_r->getSpecular();
        if(!areEqualTVec3f(specular_l, specular_r)) return compareTVec3f(specular_l, specular_r);
        const auto ambient_l = mat_l->getAmbientIntensity();
        const auto ambient_r = mat_r->getAmbientIntensity();
        if(std::abs(ambient_l - ambient_r) > epsilon) return ambient_l < ambient_r;
        const auto shiness_l = mat_l->getShininess();
        const auto shiness_r = mat_r->getShininess();
        if(std::abs(shiness_l - shiness_r) > epsilon) return shiness_l < shiness_r;
        const auto transparency_l = mat_l->getTransparency();
        const auto transparency_r = mat_r->getTransparency();
        if(std::abs(transparency_l - transparency_r) > epsilon) return transparency_l < transparency_r;
        const auto smooth_l = mat_l->isSmooth();
        const auto smooth_r = mat_r->isSmooth();
        if(smooth_l != smooth_r) return smooth_l < smooth_r;

        return false; // 等しい
    }



    void SubMesh::debugString(std::stringstream& ss, int indent) const {
        for (int i = 0; i < indent; i++) ss << "    ";
        ss << "SubMesh: [" << start_index_ << ", " << end_index_ << "] texturePath = '" << texture_path_ << "'"
           << std::endl;
    }
}
