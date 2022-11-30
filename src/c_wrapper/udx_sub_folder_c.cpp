#include "libplateau_c.h"
#include <plateau/dataset/city_model_package.h>
#include <plateau/dataset/local_dataset_accessor.h>

using namespace plateau::dataset;
extern "C" {
DLL_2_ARG_FUNC(plateau_udx_sub_folder_feature_type_to_package,
               const char* const, // feature_type
               PredefinedCityModelPackage*, // out_package
               *arg_2 = UdxSubFolder::getPackage(arg_1))
}
