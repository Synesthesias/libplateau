#pragma once
#include <citygml/vecs.hpp>

class polar_to_plane_cartesian {
public:
    void convert(double xyz[]);
    void convert(TVec3d& position);
private:
    double Merid(double phi2);

    int jt_ = 5;
    double n_ = 0, anh_ = 0;
};
