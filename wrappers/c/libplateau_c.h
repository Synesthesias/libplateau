#pragma once

// P/Invoke用なためエクスポートのみ
#ifdef _WIN32
#  define LIBPLATEAU_C_API __stdcall
#  define LIBPLATEAU_C_EXPORT  __declspec(dllexport)
#else
#  define LIBPLATEAU_C_API
#  define LIBPLATEAU_C_EXPORT
#endif

// 実装を API_TRY と API_CATCH で囲むことで例外を出力できます。
#define API_TRY try

#define API_CATCH \
catch (std::exception& e) {\
    std::cout << e.what() << std::endl;\
}\
catch (...) {\
    std::cout << "Unknown error occured." << std::endl;\
}