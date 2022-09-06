#pragma once

#include <citygml/vecs.hpp>

class polar_to_plane_cartesian {
public:
    void convert(double xyz[], int cartesian_coordinate_system_id);
    void convert(TVec3d& position, int cartesian_coordinate_system_id);
private:
    double Merid(double phi2);

    int jt_ = 5;
    double n_ = 0, anh_ = 0;
};
