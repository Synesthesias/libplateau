#pragma once

#include <exception>
#include <utility>

#include "citygml/citymodel.h"

/**
 * \brief
 * CityModel�̃��\�[�X�N���X�ł��B
 * shared_ptr���}�l�[�W�h�R�[�h���疾���I�ɐ����E�j�����邽�߂Ɏg�p����܂��B
 * ������plateau_load_citygml�֐���.gml�ǂݍ��݂Ɠ����ɍs���܂��B
 * �j����plateau_delete_city_model�֐��Ŗ����I�ɍs���Ă��������B
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
