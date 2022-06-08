#include <plateau_dll_logger.h>
#include "libplateau_c.h"
using namespace libplateau;
extern "C"{

    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_dll_logger_set_callbacks(
            PlateauDllLogger* handle,
            LogCallbackFuncPtr errorCallback,
            LogCallbackFuncPtr warnCallback,
            LogCallbackFuncPtr infoCallback
            ){
        API_TRY{
            handle->setLogCallbacks(errorCallback, warnCallback, infoCallback);
            return APIResult::Success;
        }API_CATCH;
        return APIResult::ErrorUnknown;
    }

}