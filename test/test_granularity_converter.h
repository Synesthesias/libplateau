#pragma once
#include "gtest/gtest.h"
#include "test_granularity_converter_model_for_test.h"

#include <plateau/granularity_convert/granularity_converter.h>
#include <plateau/polygon_mesh/mesh_extractor.h>

#include <utility>




class GranularityConverterTest : public ::testing::Test {
public:
    ModelForTest createTestModelOfArea();
    ModelForTest createTestModelOfAtomic();
    ModelForTest createTestModelOfPrimary();
    ModelForTest createTestModelOfArea_OnlyRoot();
};
