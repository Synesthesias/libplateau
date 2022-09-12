#pragma once

#include <citygml/vecs.hpp>

namespace plateau::geometry {
    class  PolarToPlaneCartesian {
    public:
        void project(double xyz[], int cartesian_coordinate_system_id);
        void project(TVec3d& position, int cartesian_coordinate_system_id);
        void unproject(TVec3d& lat_lon, int cartesian_coordinate_system_id);

    private:
        double Merid(double phi2);

        int jt_;
        double n_, anh_;
    };
}