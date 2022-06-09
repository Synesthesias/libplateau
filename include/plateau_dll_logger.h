#pragma once
#include <libplateau_api.h>
#include <citygml/citygmllogger.h>
using namespace citygml;
typedef CityGMLLogger::LOGLEVEL DllLogLevel;
typedef void(*LogCallbackFuncPtr)(const char*);
/**
 * @brief DLL内のログをDLLの利用者に渡すことを目的として CityGMLLogger を拡張したものです。
 * log()のコール時に、コールバックによってDLL利用者に文字列を渡します。
 */
class LIBPLATEAU_EXPORT PlateauDllLogger : public CityGMLLogger{


public:
    /// Error, Warn, Info それぞれのコールバックを指定し、
    /// どのログレベル以上を表示するかを指定します。
    explicit PlateauDllLogger(
            LOGLEVEL level = DllLogLevel::LL_INFO,
            LogCallbackFuncPtr logErrorCallback = nullptr,
            LogCallbackFuncPtr logWarnCallback = nullptr,
            LogCallbackFuncPtr logInfoCallback = nullptr
            ) : CityGMLLogger(level)
            , logErrorCallback_(logErrorCallback)
            , logWarnCallback_(logWarnCallback)
            , logInfoCallback_(logInfoCallback)
            {};

    void log(DllLogLevel level, const std::string& message, const char* file=nullptr, int line=-1) const override;
    void setLogCallbacks(LogCallbackFuncPtr errorCallback, LogCallbackFuncPtr warnCallback, LogCallbackFuncPtr infoCallback);

    /// エラーレベルの log() をコールした後に例外を出します。
    void throwException(const std::string &message);

private:
    LogCallbackFuncPtr logErrorCallback_;
    LogCallbackFuncPtr logWarnCallback_;
    LogCallbackFuncPtr logInfoCallback_;

};