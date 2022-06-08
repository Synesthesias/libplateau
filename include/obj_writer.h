#pragma once
#include <string>
#include <fstream>

#include <citygml/citygml.h>
#include <libplateau_api.h>

enum class AxesConversion {
    WNU,
    RUF
};

enum class MeshGranularity {
    PerAtomicFeatureObject, // 最小地物単位(建物パーツ)
    PerPrimaryFeatureObject, // 主要地物単位(建築物、道路等)
    PerCityModelArea // 都市モデル地域単位(GMLファイル内のすべてを結合)
};

typedef void(*LogCallbackFuncType)(const char*);

class LIBPLATEAU_EXPORT ObjWriter {
public:
    ObjWriter() : ofs_(), logCallback_(nullptr) {
    }

    void write(const std::string& obj_file_path, const citygml::CityModel& city_model, const std::string& gml_file_path);
    void setDestAxes(AxesConversion value);
    AxesConversion getDestAxes() const;
    void setValidReferencePoint(const citygml::CityModel& city_model);
    void getReferencePoint(double xyz[]) const;
    void setReferencePoint(const double xyz[]);
    void setMeshGranularity(MeshGranularity value);
    MeshGranularity getMeshGranularity() const;
    void setLogCallback(void(*func)(const char*));

private:
    unsigned int writeVertices(const std::vector<TVec3d>& vertices);
    void writeIndices(const std::vector<unsigned int>& indices, unsigned int ix_offset, unsigned int tx_offset, bool tex_flg);
    unsigned int writeUVs(const std::vector<TVec2f>& uvs);
    void writeMaterial(const std::string& tex_path);
    void processChildCityObject(const citygml::CityObject& target_object, unsigned int& v_offset, unsigned int& t_offset);
    void writeCityObject(const citygml::CityObject& target_object, unsigned int& v_offset, unsigned int& t_offset, bool recursive_flg);
    void writeGeometry(const citygml::Geometry& target_geometry, unsigned int& v_offset, unsigned int& t_offset, bool recursive_flg);

    /// std::cout にログを出力し、 setLogCallback() で指定されたコールバックを実行します。
    /// コールバックの指定がなければ、コールバックは実行せず std::cout のみにログを出力します。
    /// 用途はDLLの利用者にログの文字列を渡すことです。
    void log(const char* text);

    /// 例外を投げ、log()をコールします。
    void throwException(std::string message);

    std::ofstream ofs_;
    std::ofstream ofs_mat_;
    std::string gml_file_path_, obj_file_path_;
    std::vector<std::string> mat_list_;
    AxesConversion axes_ = AxesConversion::WNU;
    double ref_point_[3] = {0,0,0};
    MeshGranularity mesh_granularity_ = MeshGranularity::PerPrimaryFeatureObject;
    void(*logCallback_)(const char*);
};
