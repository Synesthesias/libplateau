#include "PolarToPlaneCartesian.h"
#include <citygml/citygml.h>

void PolarToPlaneCartesian::convert(double xyz[]) {
    //平面直角座標変換の計算方法は https://www.gsi.go.jp/common/000061216.pdf
    int a = 6378137;
    double rf = 298.257222101, m0 = 0.9999, PI = 3.14159265358979323846;
    double s2r = 0, nsq = 0, e2n = 0, ra = 0;
    double alp[5 + 1] = { 0,0,0,0,0,0 };

    // 平面直角座標の座標系原点の緯度を度単位で、経度を分単位で格納
    int phi0[20] = { 0, 33, 33, 36, 33, 36, 36, 36, 36, 36, 40, 44, 44, 44, 26, 26, 26, 26, 20, 26 };
    int lmbd0[20] = { 0, 7770, 7860, 7930, 8010, 8060, 8160, 8230, 8310, 8390, 8450, 8415, 8535, 8655, 8520, 7650, 7440, 7860, 8160, 9240 };

    s2r = PI / 648000;
    n_ = 0.5 / (rf - 0.5);

    anh_ = 0.5 * a / (1 + n_);
    nsq = n_ * n_;
    e2n = 2 * sqrt(n_) / (1 + n_);
    ra = 2 * anh_ * m0 * (1 + nsq / 4 + nsq * nsq / 64);

    // 展開パラメータの事前入力
    alp[1] = (1.0 / 2 + (-2.0 / 3 + (5.0 / 16 + (41.0 / 180 - 127.0 / 288 * n_) * n_) * n_) * n_) * n_;
    alp[2] = (13.0 / 48 + (-3.0 / 5 + (557.0 / 1440 + 281.0 / 630 * n_) * n_) * n_) * nsq;
    alp[3] = (61.0 / 240 + (-103.0 / 140 + 15061.0 / 26880 * n_) * n_) * n_ * nsq;
    alp[4] = (49561.0 / 161280 - 179.0 / 168 * n_) * nsq * nsq;
    alp[5] = 34729.0 / 80640 * n_ * nsq * nsq;

    int num = 9;//座標系番号を9に設定　https://www.gsi.go.jp/sokuchikijun/jpc.html
    double phirad = xyz[0] * PI / 180; //緯度を十進法度単位（ラジアン）に直す
    double lmbdsec = xyz[1] * 3600; //経度を秒単位（deg）に直す

    double sphi = sin(phirad);
    double nphi = (1 - n_) / (1 + n_) * tan(phirad);
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
double PolarToPlaneCartesian::Merid(double phi2) {
    int jt2 = 2 * jt_;
    double ep = 1.0;
    double e[10 + 1] = { 0,0,0,0,0,0,0,0,0,0,0 };
    double n15 = 1.5 * n_;

    for (int k = 1; k <= jt_; k++) {
        ep *= e[k] = n15 / k - n_;
        e[k + jt_] = n15 / (k + jt_) - n_;
    }

    double dc = 2.0 * cos(phi2);
    double s[10 + 2], t[10 + 1];
    s[0] = 0.0;
    s[1] = sin(phi2);
    for (int i = 1; i <= jt2; i++) {
        s[i + 1] = dc * s[i] - s[i - 1];
        t[i] = (1.0 / i - 4.0 * i) * s[i];
    }
    double sum = 0.0;
    double c1 = ep;
    int j = jt_;
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
    return anh_ * (sum + phi2);
}