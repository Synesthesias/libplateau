#include "libplateau_c.h"

#include <plateau/udx/udx_file_collection.h>

#include "citygml/address.h"

using namespace libplateau;
using namespace plateau::udx;

extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_gml_file_info(
        GmlFileInfo** gml_file_info_ptr, const char* path
    ) {
        *gml_file_info_ptr = new GmlFileInfo(path);
        return APIResult::Success;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_delete_gml_file_info(
        const GmlFileInfo* gml_file_info
    ) {
        delete gml_file_info;
        return APIResult::Success;
    }

    DLL_STRING_PTR_FUNC(plateau_gml_file_info_get_path,
                        GmlFileInfo,
                        handle->getPath())

    DLL_STRING_PTR_FUNC(plateau_gml_file_info_get_feature_type_str,
                        GmlFileInfo,
                        handle->getFeatureType())
}
