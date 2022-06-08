#include "plateau_dll_logger.h"
#include <iostream>

void PlateauDllLogger::log(CityGMLLogger::LOGLEVEL level, const std::string &message, const char *file, int line) const {
    if(!isEnabledFor(level)) return;
    std::ostream& stream = level == LOGLEVEL::LL_ERROR ? std::cerr : std::cout;
    LogCallbackFuncPtr callback;
    switch(level){
        case LOGLEVEL::LL_ERROR:
            callback = logErrorCallback_;
            break;
        case LOGLEVEL::LL_WARNING:
            callback = logWarnCallback_;
            break;
        default:
            callback = logInfoCallback_;
            break;
    }
    stream << message << std::endl;
    if(callback != nullptr) callback(message.c_str());
}

void PlateauDllLogger::throwException(const std::string &message) {
    log(DllLogLevel::LL_ERROR, message.c_str());
    throw std::runtime_error(message);
}

void PlateauDllLogger::setLogCallbacks(LogCallbackFuncPtr errorCallback, LogCallbackFuncPtr warnCallback, LogCallbackFuncPtr infoCallback) {
    logErrorCallback_ = errorCallback;
    logWarnCallback_ = warnCallback;
    logInfoCallback_ = infoCallback;
}
