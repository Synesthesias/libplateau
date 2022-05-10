#pragma once

#include <iostream>

// P/Invoke用なためエクスポートのみ
#ifdef _WIN32
#  define LIBPLATEAU_C_API __stdcall
#  define LIBPLATEAU_C_EXPORT  [[maybe_unused]] __declspec(dllexport)
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
    std::cout << "Unknown error occurred." << std::endl;\
}


// DLLでのやりとりに使う型を定義します。
/// 文字列のサイズをDLLでやりとりする時の型です。
using dll_str_size_t = int;


/// アドレスをDLL利用者に渡す関数を生成するマクロです。
/// HANDLE_TYPE* handle から RETURN_VALUE_TYPEのアドレスを取得して引数 out に書き込みます。
/// マクロの引数は 1番目に関数名,
/// 2番目に対象クラス名,
/// 3番目に渡したいアドレスの実体型名,
/// 4番目に 対象クラス* handle から RETURN_VALUE_TYPE* を取得する処理,
/// 5番目以降は省略可能で、追加で必要な引数をカンマから記載したものです。
/// 関数の引数は 1番目が HANDLE_TYPE のハンドル(アドレス),
/// 2番目が 出力用の渡すべきアドレスの参照 となります。
/// 戻り値は API_Result です。
#define DLL_PTR_FUNC(FUNC_NAME, HANDLE_TYPE, RETURN_VALUE_TYPE, GETTER, ...) \
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API FUNC_NAME( \
            const HANDLE_TYPE* const handle, \
            const RETURN_VALUE_TYPE** out /* アドレスを参照渡しで渡したいので '*' が2つ付きます。 */\
            __VA_ARGS__ \
            ){ \
        API_TRY{ \
            *out = GETTER; /* アドレスを out に書き込みます。 */ \
            return APIResult::Success; \
        } \
        API_CATCH \
        return APIResult::ErrorUnknown; \
    }

/// 値を DLL利用者に渡す関数を生成するマクロです。
/// HANDLE_TYPE* handle から値を取得して 引数 RETURN_VALUE_TYPE* out に書き込みます。
/// DLL_PTR_FUNCとの違いは、アドレスを渡す代わりに実体を引数 *out に書き込む点です。
#define DLL_VALUE_FUNC(FUNC_NAME, HANDLE_TYPE, RETURN_VALUE_TYPE, GETTER, ...) \
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API FUNC_NAME( \
            const HANDLE_TYPE* const handle, \
            RETURN_VALUE_TYPE* const out                                       \
            __VA_ARGS__ \
            ){ \
        API_TRY{ \
            *out = GETTER; /* 渡したい値を out に書き込みます。*/ \
            return APIResult::Success; \
        } \
        API_CATCH \
        return APIResult::ErrorUnknown; \
    }

/// 文字列のアドレスを渡す関数と文字列の長さを渡す関数の2つを生成するマクロです。
/// マクロ引数の3番目は TARGET_TYPE* handle から string を取得する処理です。
/// 2つ生成する意図は、文字列をDLLの利用者に渡すとき、char*アドレスと文字列長の両方が欲しいためです。
#define DLL_STRING_PTR_FUNC2(FUNC_NAME, TARGET_TYPE, STRING_GETTER) \
    /* 文字列ポインタを渡す関数*/ \
    DLL_PTR_FUNC(FUNC_NAME, TARGET_TYPE, char, (STRING_GETTER).c_str()) \
    /* 文字列長を渡す関数 */ \
    DLL_VALUE_FUNC(FUNC_NAME ## _str_length, \
                    TARGET_TYPE, int, \
                    (dll_str_size_t)((STRING_GETTER).length()) +1 ) /* +1 は null終端文字列の分 */

namespace libplateau {
    // 処理中にエラーが発生する可能性があり、その内容をDLLの呼び出し側に伝えたい場合は、
    // このenumを戻り値にすると良いです。
    enum APIResult {
        Success, ErrorUnknown, ErrorValueNotFound
    };
}