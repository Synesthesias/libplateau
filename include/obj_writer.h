#pragma once
#include <string>
#include <fstream>

#include <citygml/citygml.h>

class ObjWriter {
public:
    ObjWriter() : ofs_() {
    }

    void write(const std::string& file_path, const citygml::CityModel& city_model);

private:
    unsigned int writeVertices(const std::vector<TVec3d>& vertices);
    void writeIndices(const std::vector<unsigned int>& indices, unsigned int ix_offset);
    void convertPolarToPlaneCartsian(double xyz[]);
    double ObjWriter::Merid(double phi2);

    std::ofstream ofs_;

    //計算方法は https://www.gsi.go.jp/common/000061216.pdf
    int a = 6378137, jt = 5, jt2 = 0;
    double rf = 298.257222101, m0 = 0.9999, PI = 3.14159265358979323846;
    double s2r = 0, n = 0, n15 = 0, anh = 0, nsq = 0, e2n = 0, ra = 0, ep = 1.0;
    double alp[5 + 1] = {0,0,0,0,0,0};
    double e[10 + 1] = {0,0,0,0,0,0,0,0,0,0,0};

    // 平面直角座標の座標系原点の緯度を度単位で、経度を分単位で格納
    int phi0[20] = { 0, 33, 33, 36, 33, 36, 36, 36, 36, 36, 40, 44, 44, 44, 26, 26, 26, 26, 20, 26 };
    int lmbd0[20] = { 0, 7770, 7860, 7930, 8010, 8060, 8160, 8230, 8310, 8390, 8450, 8415, 8535, 8655, 8520, 7650, 7440, 7860, 8160, 9240 };
};
