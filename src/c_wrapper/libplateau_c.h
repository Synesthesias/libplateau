#pragma once

#include <iostream>
#include <cstring>

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


// DLLでのやりとりに使う型を決めます。

/// 文字列のサイズをDLLでやりとりする時の型を決めます。
using dll_str_size_t = int;

/**
 * アドレスをDLL利用者に渡す関数を生成するマクロです。
 * HANDLE_TYPE* handle から RETURN_VALUE_TYPEのアドレスを取得して引数 out に書き込みます。
 * マクロの引数は 1番目に関数名,
 * 2番目に対象クラス名,
 * 3番目に渡したいアドレスの実体型名,
 * 4番目に 対象クラス* handle から RETURN_VALUE_TYPE* を取得する処理,
 * 5番目以降は省略可能で、追加で必要な引数をカンマから記載したものです。
 * 関数の引数は 1番目が HANDLE_TYPE のハンドル(アドレス),
 * 2番目が 出力用の渡すべきアドレスの参照 となります。
 * 戻り値は API_Result です。
 */
#define DLL_PTR_FUNC(FUNC_NAME, HANDLE_TYPE, RETURN_VALUE_TYPE, GETTER, ...) \
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API FUNC_NAME( \
            const HANDLE_TYPE* const handle,/* C#での "[In] IntPtr handle" に対応します。 */ \
            const RETURN_VALUE_TYPE** out /* C#での "out IntPtr outPtr" に対応します。 アドレスを参照渡しで渡したいので '*' が2つ付きます。 */\
            __VA_ARGS__ \
            ){ \
        API_TRY{ \
            *out = GETTER; /* アドレスを out に書き込みます。 */ \
            return APIResult::Success; \
        } \
        API_CATCH \
        return APIResult::ErrorUnknown; \
    }


/**
 * アドレスを DLL利用者に渡す関数を生成するマクロです。
 * クロ引数 INDEX_OUT_OF_RANGE_CONDITION (int index を利用してboolを返す式)が真のとき、 APIResult::ErrorIndexOutOfBounds を返します。
 * 偽のとき、例外がなければ GETTER (HANDLE_TYPE *handle と int index を利用してアドレスを得る式) の値を out に格納して APIResult::Success を返します。
 * 例外が起きたとき、 APIResult::ErrorUnknown を返します。
 */
#define DLL_PTR_FUNC_WITH_INDEX_CHECK(FUNC_NAME, HANDLE_TYPE, RETURN_VALUE_TYPE, GETTER, INDEX_OUT_OF_RANGE_CONDITION, ...) \
     LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API FUNC_NAME( \
            const HANDLE_TYPE* const handle,/* C#での "[In] IntPtr handle" に対応します。 */  \
            const RETURN_VALUE_TYPE** out, /* C#での "out IntPtr outTexCoords" に対応します。 アドレスを参照渡しで渡したいので '*' が2つ付きます。 */  \
            int index  \
            __VA_ARGS__ \
            ){ \
        API_TRY{  \
            if((INDEX_OUT_OF_RANGE_CONDITION)) return APIResult::ErrorIndexOutOfBounds;  \
            *out = GETTER; /* アドレスを out に書き込みます。 */ \
            return APIResult::Success; \
        } \
        API_CATCH \
        return APIResult::ErrorUnknown; \
    }

/**
 * 値を DLL利用者に渡す関数を生成するマクロです。
 * HANDLE_TYPE* handle から値を取得して 引数 RETURN_VALUE_TYPE* out に書き込みます。
 * DLL_PTR_FUNCとの違いは、アドレスを渡す代わりに実体を引数 *out に書き込む点です。
 */
#define DLL_VALUE_FUNC(FUNC_NAME, HANDLE_TYPE, RETURN_VALUE_TYPE, GETTER, ...) \
    LIBPLATEAU_C_EXPORT libplateau::APIResult LIBPLATEAU_C_API FUNC_NAME( \
            HANDLE_TYPE* const handle, /* C# では引数   [In] IntPtr handle に対応します。 */ \
            RETURN_VALUE_TYPE* const out /* C# では引数 out OutType ret に対応します。 */ \
            __VA_ARGS__ \
            ){ \
        API_TRY{ \
            *out = GETTER; /* 渡したい値を out に書き込みます。*/ \
            return libplateau::APIResult::Success; \
        } \
        API_CATCH \
        return libplateau::APIResult::ErrorUnknown; \
    }


/**
 * インデックスのチェックをしてから値をDLLの利用者に返す関数を作るマクロです。
 * マクロ引数 INDEX_OUT_OF_RANGE_CONDITION (int index を利用してboolを返す式)が真のとき、 APIResult::ErrorIndexOutOfBounds を返します。
 * 偽のとき、例外がなければ GETTER (HANDLE_TYPE *handle と int index を利用して値を得る式) の値を out に格納して APIResult::Success を返します。
 * 例外が起きたとき、 APIResult::ErrorUnknown を返します。
 */
#define DLL_VALUE_FUNC_WITH_INDEX_CHECK(FUNC_NAME, HANDLE_TYPE, RETURN_VALUE_TYPE, GETTER, INDEX_OUT_OF_RANGE_CONDITION, ...) \
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API FUNC_NAME( \
            const HANDLE_TYPE* const handle, \
            RETURN_VALUE_TYPE* const out, \
            int index \
            __VA_ARGS__ \
            ){   \
        API_TRY{  \
            if((INDEX_OUT_OF_RANGE_CONDITION)) return APIResult::ErrorIndexOutOfBounds;  \
            *out = GETTER; \
            return APIResult::Success; \
        } \
        API_CATCH \
        return APIResult::ErrorUnknown;  \
    }

/// 文字列のアドレスと文字列長を渡す関数を生成するマクロです。
/// マクロ引数の3番目は TARGET_TYPE* handle から string を取得する処理です。
#define DLL_STRING_PTR_FUNC(FUNC_NAME, TARGET_TYPE, STRING_GETTER, ...) \
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API FUNC_NAME( \
        TARGET_TYPE* const handle,/* C#では "[In] IntPtr handle" に対応します。 */ \
        const char** const out_chars_ptr, /* C#では "out IntPtr outStrPtr" に対応します。アドレスを参照渡しで渡すので'*'が2つ付きます。 */ \
        dll_str_size_t* out_str_length /* C#では "out int strLength" に対応します。*/\
        __VA_ARGS__ \
    ){ \
        API_TRY{\
            auto& str = (STRING_GETTER); \
            *out_chars_ptr = str.c_str();\
            *out_str_length = (dll_str_size_t)str.length() + 1; /* +1 は null終端文字列の分です。 */\
            return APIResult::Success;                             \
        }                                                          \
        API_CATCH                                                  \
        return APIResult::ErrorUnknown;\
    }

/// 文字列のコピーを渡したい時に利用するマクロです。
/// 関数を2つ作成します。
/// 関数1つ目は FUNC_NAME_size という名称で、文字列のバイト数を渡します。
/// 関数2つ目は FUNC_NAME という名称で、与えられたアドレスに文字列のコピーを書き込みます。
/// DLL_STRING_PTR_FUNC との違いは、文字列のポインタではなく実体のコピーを渡すので
/// C++側での文字列の寿命が短い場合でも利用できる点です。
#define DLL_STRING_VALUE_FUNC(FUNC_NAME, TARGET_TYPE, STRING_GETTER, ...) \
    DLL_VALUE_FUNC(FUNC_NAME ## _size, TARGET_TYPE, int, (int)(STRING_GETTER).length()+1, __VA_ARGS__ ) /* +1 はnull終端文字列の分 */ \
    LIBPLATEAU_C_EXPORT libplateau::APIResult LIBPLATEAU_C_API FUNC_NAME( \
            const TARGET_TYPE* const handle, \
            char* const out_str_ptr __VA_ARGS__){ \
        API_TRY{ \
            auto str = (STRING_GETTER); \
            auto chars = str.c_str(); \
            auto len = (dll_str_size_t) (str.length()); \
            strncpy(out_str_ptr, chars, len); \
            out_str_ptr[len] = '\0'; /* 最後はnull終端文字*/ \
            return libplateau::APIResult::Success; \
        } \
        API_CATCH \
        return libplateau::APIResult::ErrorUnknown; \
    }

/// 文字列のポインタの配列を渡したいときに利用するマクロです。
/// 2つの関数を生成します。
///
/// 生成される関数について:
/// 関数1つ目は FUNC_NAME_count という名前で、配列の要素数を渡します。
/// 関数2つ目は FUNC_NAME という名前で、
/// 数値配列 out_str_lengths に各文字列のサイズを配列で格納します。
/// また char** out_str_ptrs に文字のポインタの配列を格納します。
///
/// 前提:
/// 情報を格納するのに十分なメモリ領域をDLLの利用者が確保していなければアクセス違反となります。
///
/// マクロとしての引数について:
/// ARRAY_LENGTH_GETTER は TARGET_TYPE* handle から要素数を取得するための処理を記述します。
/// FOR_RANGE は TARGET_TYPE* handle から文字列をイテレートするための範囲for文の(小括弧)の中身を記載します。
/// STRING_GETTER は範囲for文の{中}で string を取得する処理を記載します。
/// 関数に追加の引数を持たせたい場合は __VA_ARGS__ にカンマから記載します。
///
/// 類似マクロとの使い分けについて:
/// 似たマクロに DLL_STRING_VALUE_ARRAY_FUNC2 があります。
/// 同マクロとの違いは、文字列のポインタの配列を渡すのか、文字列のコピーを渡すのかです。
/// これらは文字列の寿命によって使い分けます。
/// 文字列がC++側で保持されている場合は、
/// このマクロによって文字列のポインタをDLLの利用者に渡せば文字列を読んでもらうことができます。
/// 一方で、文字列の寿命が短く、C++側で一時的にしか保持されない文字列を渡したい場合は、
/// 文字列をコピーする必要があります。
#define DLL_STRING_PTR_ARRAY_FUNC2(FUNC_NAME, TARGET_TYPE, ARRAY_LENGTH_GETTER, FOR_RANGE, STRING_GETTER, ...) \
    /* 配列の要素数を渡す関数です。*/                \
    DLL_VALUE_FUNC(FUNC_NAME ## _count,\
                    TARGET_TYPE,\
                    int,\
                    (ARRAY_LENGTH_GETTER)\
                    , __VA_ARGS__)\
    /* 文字列のアドレスの配列について、各アドレスを 引数 out_str_ptrs に書き込み、*/\
    /* 各文字列の長さの配列を out_array_length に書き込む関数です。*/\
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API FUNC_NAME( \
            const TARGET_TYPE *const handle,\
            const char ** const out_str_ptrs, /* C#では '[In, Out] IntPtr[] strPointers' に対応します。 */\
            int* const out_str_lengths/* C#では '[Out] int[] outStrSizes' に対応します。' */\
            __VA_ARGS__\
        ) {\
            API_TRY {\
                int i = 0;\
                for (FOR_RANGE) { \
                    auto& str = (STRING_GETTER); \
                    out_str_ptrs[i] = str.c_str();\
                    out_str_lengths[i] = str.length() + 1; /* +1 はnull終端文字の分 */\
                    i++; \
                }\
                return APIResult::Success;\
            }\
            API_CATCH\
            return APIResult::ErrorUnknown;\
        }

/// 文字列の配列について、各文字列の長さを数値型の配列にして渡す関数を生成するマクロです。
/// マクロ引数について、
/// FOR_RANGE は TARGET_TYPE* handle から範囲for文で各文字列にアクセスするための範囲部分を記載します。
/// STRING_GETTER は範囲forの中で文字列にアクセスするための処理を記載します。
/// 追加で渡したい引数があるときは、 __VA_ARGS__ にカンマから始めて記載します。
#define DLL_STRINGS_SIZE_ARRAY(FUNC_NAME, TARGET_TYPE,FOR_RANGE, STRING_GETTER, ...) \
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API FUNC_NAME(\
        const TARGET_TYPE *const handle,\
        dll_str_size_t *const out_size_array\
        __VA_ARGS__\
        ) {\
        API_TRY {\
            int i = 0;\
            for (FOR_RANGE) {\
                out_size_array[i] = (dll_str_size_t)((STRING_GETTER).size()) + 1; /* +1 は null終端文字の分です。*/\
                i++;\
            }\
            return APIResult::Success;\
        }\
        API_CATCH\
        return APIResult::ErrorUnknown;\
    }

/// 文字列の配列をコピーして渡したい時に利用するマクロです。
/// 3つの関数を生成します。
/// 1つ目は配列の要素数を渡す関数で、FUNC_NAME_count という名称です。
/// 2つ目は各文字列のサイズを数値配列で渡す関数で、 FUNC_NAME_str_sizes という名称です。
/// 3つ目は各文字列のコピーを与えられたメモリ領域に書き込む関数で、 名前は FUNC_NAME です。
///
/// 類似マクロ DLL_STRING_PTR_ARRAY_FUNC2 との使い分けについては
/// そちらのコメントを参照してください。
#define DLL_STRING_VALUE_ARRAY_FUNC3(FUNC_NAME, TARGET_TYPE, ARRAY_LENGTH_GETTER, FOR_RANGE, STRING_GETTER, ...) \
    /* 文字列のポインタの配列の要素数を渡す関数です。*/ \
    DLL_VALUE_FUNC(FUNC_NAME ## _count,\
                    TARGET_TYPE,\
                    int,\
                    (ARRAY_LENGTH_GETTER)\
                    , __VA_ARGS__)\
    \
    /* 各文字列のサイズを配列で渡す関数です。 */\
    DLL_STRINGS_SIZE_ARRAY(FUNC_NAME ## _str_sizes,\
                            TARGET_TYPE,\
                            FOR_RANGE,\
                            STRING_GETTER\
                            , __VA_ARGS__)                                                                       \
    \
   /* 各文字列のコピーを与えられたメモリ領域に書き込む関数です。*/ \
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API FUNC_NAME(\
            const TARGET_TYPE *const handle,\
            char** out_strs\
            __VA_ARGS__\
    ){\
        API_TRY {\
            int i = 0;\
            for (FOR_RANGE) {\
                /*文字列を out_strs[i] にコピーします。*/ \
                auto chars = (STRING_GETTER).c_str();\
                auto len = (dll_str_size_t) (strlen(chars));\
                strncpy(out_strs[i], chars, len);\
                out_strs[i][len] = '\0'; /* 最後はnull終端文字*/\
                i++;\
            }\
            return APIResult::Success;\
        }\
        API_CATCH\
        return APIResult::ErrorUnknown;\
    }

/**
 * CREATE_TYPE を new し、そのアドレスを引数の out_ptr に書き込む関数を生成するマクロです。
 */
#define DLL_CREATE_FUNC(FUNC_NAME, CREATE_TYPE) \
    LIBPLATEAU_C_EXPORT libplateau::APIResult LIBPLATEAU_C_API FUNC_NAME ( \
            CREATE_TYPE ** out_ptr /* C# では out IntPtr outPtr に対応します。*/ \
    ) { \
        API_TRY { \
            *out_ptr = new CREATE_TYPE (); \
            return libplateau::APIResult::Success;                                            \
        }API_CATCH \
        return libplateau::APIResult::ErrorUnknown; \
    }

/**
 * DELETE_TYPE を delete する関数を生成するマクロです。
 */
#define DLL_DELETE_FUNC(FUNC_NAME, DELETE_TYPE) \
    LIBPLATEAU_C_EXPORT libplateau::APIResult LIBPLATEAU_C_API FUNC_NAME ( \
            const DELETE_TYPE * ptr /* C# では [In] IntPtr ptr に対応します。 */ \
    ) { \
        delete ptr; \
        return libplateau::APIResult::Success; \
    }

/**
 * ARG_DEF (型 引数名) を引数にとり、 PREDICATE を実行する関数を生成するマクロです。
 */
#define DLL_1_ARG_FUNC(FUNC_NAME, ARG_DEF, PREDICATE) \
LIBPLATEAU_C_EXPORT libplateau::APIResult LIBPLATEAU_C_API FUNC_NAME ( \
        ARG_DEF \
){ \
    API_TRY{ \
        { PREDICATE ;} \
        return libplateau::APIResult::Success; \
    }API_CATCH \
    return libplateau::APIResult::ErrorUnknown; \
}

/**
 * ARG_1_DEF と ARG_2_DEF (型　引数名) を引数にとり、 PREDICATE を実行する関数を生成するマクロです。
 */
#define DLL_2_ARG_FUNC(FUNC_NAME, ARG_1_DEF, ARG_2_DEF, PREDICATE) \
LIBPLATEAU_C_EXPORT libplateau::APIResult LIBPLATEAU_C_API FUNC_NAME ( \
        ARG_1_DEF, \
        ARG_2_DEF \
){ \
    API_TRY{ \
        { PREDICATE ;} \
        return libplateau::APIResult::Success; \
    }API_CATCH \
    return libplateau::APIResult::ErrorUnknown; \
}

/**
 * ARG_1_DEF と ARG_2_DEF, ARG_3_DEF(型 引数名)を引数にとり、 PREDICATE を実行する関数を生成するマクロです。
 */
#define DLL_3_ARG_FUNC(FUNC_NAME, ARG_1_DEF, ARG_2_DEF, ARG_3_DEF, PREDICATE) \
LIBPLATEAU_C_EXPORT libplateau::APIResult LIBPLATEAU_C_API FUNC_NAME ( \
        ARG_1_DEF, \
        ARG_2_DEF, \
        ARG_3_DEF  \
){ \
    API_TRY{ \
        { PREDICATE ;} \
        return libplateau::APIResult::Success; \
    }API_CATCH \
    return libplateau::APIResult::ErrorUnknown; \
}

/**
 * ARG_1_DEF と ARG_2_DEF, ARG_3_DEF, ARG_4_DEF (型 引数名) を引数にとり、 PREDICATE を実行する関数を生成するマクロです。
 */
#define DLL_4_ARG_FUNC(FUNC_NAME, ARG_1_DEF, ARG_2_DEF, ARG_3_DEF, ARG_4_DEF, PREDICATE) \
LIBPLATEAU_C_EXPORT libplateau::APIResult LIBPLATEAU_C_API FUNC_NAME ( \
        ARG_1_DEF, \
        ARG_2_DEF, \
        ARG_3_DEF, \
        ARG_4_DEF \
){ \
    API_TRY{ \
        { PREDICATE ;} \
        return libplateau::APIResult::Success; \
    }API_CATCH \
    return libplateau::APIResult::ErrorUnknown; \
}

namespace libplateau {
    // 処理中にエラーが発生する可能性があり、その内容をDLLの呼び出し側に伝えたい場合は、
    // このenumを戻り値にすると良いです。
    enum APIResult {
        Success, ErrorUnknown, ErrorValueNotFound, ErrorLoadingCityGml, ErrorIndexOutOfBounds, ErrorFileSystem,
        ErrorInvalidArgument
    };
}
