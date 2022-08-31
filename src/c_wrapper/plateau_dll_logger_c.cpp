#include <plateau_dll_logger.h>
#include "libplateau_c.h"
using namespace libplateau;

extern "C" {
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_create_dll_logger(PlateauDllLogger** logger) {
        API_TRY{
            *logger = new PlateauDllLogger;
            return APIResult::Success;
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_delete_dll_logger(PlateauDllLogger** logger) {
        API_TRY{
            delete logger;
        }
        API_CATCH;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_dll_logger_set_callbacks(
            PlateauDllLogger* handle,
            LogCallbackFuncPtr error_callback,
            LogCallbackFuncPtr warn_callback,
            LogCallbackFuncPtr info_callback
            ){
        API_TRY{
            handle->setLogCallbacks(error_callback, warn_callback, info_callback);
            return APIResult::Success;
        }API_CATCH;
        return APIResult::ErrorUnknown;
    }

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_dll_logger_set_log_level(
            PlateauDllLogger* handle,
            DllLogLevel log_level
            ){
        API_TRY{
            handle->setLogLevel(log_level);
            return APIResult::Success;
        }API_CATCH;
        return APIResult::ErrorUnknown;
    }

}
