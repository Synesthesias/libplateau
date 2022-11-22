// TODO あとで消す
//#include <gtest/gtest.h>
//#include <plateau/udx/local_dataset_accessor.h>
//#include <plateau/udx/dataset_accessor_p_invoke.h>
//namespace plateau::udx{
//    using namespace geometry;
//    class DatasetAccessorPInvokeTest : public ::testing::Test{
//
//    };
//
//    TEST_F(DatasetAccessorPInvokeTest, get_gml_files_returns_valid_value){ // NOLINT
//        auto local = LocalDatasetAccessor::find("../data");
//        auto accessor = DatasetAccessorPInvoke(local);
//        accessor.getGmlFiles(Extent(
//                GeoCoordinate(35.53, 139.77, -9999),
//                GeoCoordinate(35.54,139.78,9999)),
//                             PredefinedCityModelPackage::Building);
//        ASSERT_EQ(accessor.resultOfGetGmlFilesCount(PredefinedCityModelPackage::Building), 1);
//        ASSERT_EQ(accessor.resultOfGetGmlFiles(PredefinedCityModelPackage::Building, 0)->isValid(), true);
//    }
//}
