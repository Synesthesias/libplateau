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
    PerAtomicFeatureObject, // �ŏ��n���P��(�����p�[�c)
    PerPrimaryFeatureObject, // ��v�n���P��(���z���A���H��)
    PerCityModelArea // �s�s���f���n��P��(GML�t�@�C�����̂��ׂĂ�����)
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
    /// Print log to std::cout, and execute method passed by setLogCallback().
    /// If callback is not set, callback is not called.
    /// This is used to pass log text to DLL user.
    void log(const char* text);
    /// Throw Exception and call log().
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
