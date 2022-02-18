#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iomanip>

#include <citygml/citygml.h>
#include <citygml/citymodel.h>
#include <citygml/geometry.h>
#include <citygml/polygon.h>

#include "obj_writer.h"

void ObjWriter::write(const std::string& file_path, const citygml::CityModel& city_model) {

    unsigned int v_cnt = 0, cnt;
    s2r = PI / 648000;
    n = 0.5 / (rf - 0.5);
    n15 = 1.5 * n;
    anh = 0.5 * a / (1 + n);
    nsq = n * n;
    e2n = 2 * sqrt(n) / (1 + n);
    ra = 2 * anh * m0 * (1 + nsq / 4 + nsq * nsq / 64);
    jt2 = 2 * jt;
    for (int k = 1; k <= jt; k++) {
        ep *= e[k] = n15 / k - n;
        e[k + jt] = n15 / (k + jt) - n;
    }

    // 展開パラメータの事前入力
    alp[1] = (1.0 / 2 + (-2.0 / 3 + (5.0 / 16 + (41.0 / 180 - 127.0 / 288 * n) * n) * n) * n) * n;
    alp[2] = (13.0 / 48 + (-3.0 / 5 + (557.0 / 1440 + 281.0 / 630 * n) * n) * n) * nsq;
    alp[3] = (61.0 / 240 + (-103.0 / 140 + 15061.0 / 26880 * n) * n) * n * nsq;
    alp[4] = (49561.0 / 161280 - 179.0 / 168 * n) * nsq * nsq;
    alp[5] = 34729.0 / 80640 * n * nsq * nsq;

    ofs_ = std::ofstream(file_path);
    if (!ofs_.is_open()) {
        throw std::runtime_error(std::string("Failed to open : ") + file_path);
    }

    ofs_ << std::fixed << std::setprecision(6);
    int rc = city_model.getNumRootCityObjects();
    std::cout << "NumRootCityObjects : " << rc << std::endl;
    for (int h = 0; h < rc; h++) {
        std::cout << "建物ID : " << city_model.getRootCityObject(h).getAttribute("建物ID") << std::endl;
        int cc = city_model.getRootCityObject(h).getChildCityObjectsCount();
        std::cout << "ChildCityObjectsCount : " << cc << std::endl;
        for (int i = 0; i < cc; i++) {
            const auto& target_object = city_model.getRootCityObject(h).getChildCityObject(i);
            ofs_ << "g " << target_object.getId() << std::endl;
            std::cout << "ID : " << target_object.getId() << std::endl;
            int gc = target_object.getGeometriesCount();
            for (int j = 0; j < gc; j++) {
                int pc = target_object.getGeometry(j).getPolygonsCount();
                for (int k = 0; k < pc; k++) {
                    cnt = writeVertices(target_object.getGeometry(j).getPolygon(k)->getVertices());
                    writeIndices(target_object.getGeometry(j).getPolygon(k)->getIndices(), v_cnt);
                    v_cnt += cnt;
                }
            }
        }
    }
    ofs_.close();
}

unsigned int ObjWriter::writeVertices(const std::vector<TVec3d>& vertices) {
    int cnt = 0;
    std::for_each(vertices.cbegin(), vertices.cend(), [&](const TVec3d& v) {
        double xyz[3];
        for (int i = 0; i < 3; i++) xyz[i] = v[i];
        convertPolarToPlaneCartsian(xyz);
        ofs_ << "v " << xyz[0] << " " << xyz[1] << " " << xyz[2] << std::endl;
        cnt++;
    });
    return cnt;
}

void ObjWriter::writeIndices(const std::vector<unsigned int>& indices, unsigned int ix_offset) {
    int i = 0;
    
    for (auto itr = indices.begin(); itr != indices.end(); itr++) {
        if(i == 0) ofs_ << "f ";
        ofs_ << *itr + 1 + ix_offset << " ";
        i++;
        if (i == 3) {
            i = 0;
            ofs_ << std::endl;
        }
    }
}

void ObjWriter::convertPolarToPlaneCartsian(double xyz[]) {
    int num = 9;//座標系番号を9に設定　https://www.gsi.go.jp/sokuchikijun/jpc.html
    double phirad = xyz[0] * PI / 180; //緯度を十進法度単位（ラジアン）に直す
    double lmbdsec = xyz[1] * 3600; //経度を秒単位（deg）に直す

    double sphi = sin(phirad); 
    double nphi = (1 - n) / (1 + n) * tan(phirad);
    double dlmbd = (lmbdsec - lmbd0[num] * 60) * s2r;
    double sdlmbd = sin(dlmbd);
    double cdlmbd = cos(dlmbd);
    double tchi = sinh(std::atanh(sphi) - e2n * std::atanh(e2n * sphi)); 
    double cchi = sqrt(1 + tchi * tchi);
    double xip = atan2(tchi, cdlmbd);
    double xi = xip; 
    double etap = std::atanh(sdlmbd / cchi);
    double eta = etap; 
    double sgm = 1.0; 
    double tau = 0.0;
    for (int j = sizeof(alp) / sizeof(double); --j; ) {
        double alsin = alp[j] * sin(2 * j * xip);
        double alcos = alp[j] * cos(2 * j * xip);
        xi += alsin * cosh(2 * j * etap);
        eta += alcos * sinh(2 * j * etap);
        sgm += 2 * j * alcos * cosh(2 * j * etap);
        tau += 2 * j * alsin * sinh(2 * j * etap);
    }
    double x = ra * xi - m0 * Merid(2 * phi0[num] * 3600 * s2r); 
    double y = ra * eta;

    xyz[0] = y;
    xyz[1] = x;
}

// 該当緯度の 2 倍角の入力により赤道からの子午線弧長を求める関数
double ObjWriter::Merid(double phi2) {
    double dc = 2.0 * cos(phi2); 
    double s[10+2], t[10+1];
    s[0] = 0.0;
    s[1] = sin(phi2);
    for (int i = 1; i <= jt2; i++) {
        s[i + 1] = dc * s[i] - s[i - 1];
        t[i] = (1.0 / i - 4.0 * i) * s[i];
    }
    double sum = 0.0;
    double c1 = ep;
    int j = jt;
    while (j) {
        double c2 = phi2;
        double c3 = 2.0;
        int l = j;
        int m = 0;
        while (l) {
            c3 /= e[l--];
            c2 += c3 * t[++m];
            c3 *= e[2 * j - l];
            c2 += c3 * t[++m];
        }
        sum += c1 * c1 * c2;
        c1 /= e[j--];
    }
    return anh * (sum + phi2);
}