#pragma once

#include <exception>
#include <utility>

#include "citygml/citymodel.h"

/**
 * \brief
 * CityModelのリソースクラスです。
 * shared_ptrをマネージドコードから明示的に生成・破棄するために使用されます。
 * 生成はplateau_load_citygml関数で.gml読み込みと同時に行われます。
 * 破棄はplateau_delete_city_model関数で明示的に行ってください。
 */
class CityModelHandle {
public:
    explicit CityModelHandle(std::shared_ptr<const citygml::CityModel> city_model)
        : city_model_(std::move(city_model)) {
    }

    const citygml::CityModel& getCityModel() const {
        return *city_model_;
    }

private:
    std::shared_ptr<const citygml::CityModel> city_model_;
};
