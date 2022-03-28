#include <exception>
#include <iostream>
#include <utility>

#include <obj_writer.h>

#include "libplateau_c.h"

class CityModelHandle {
public:
    explicit CityModelHandle(std::shared_ptr<const citygml::CityModel> city_model)
    : city_model_(std::move(city_model)) {}

    const citygml::CityModel& getCityModel() const
    {
        return *city_model_;
    }

private:
    std::shared_ptr<const citygml::CityModel> city_model_;
};