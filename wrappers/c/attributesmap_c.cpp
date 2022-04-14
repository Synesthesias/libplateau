#include <citygml/attributesmap.h>
#include <string.h>
#include "libplateau_c.h"

using namespace citygml;

extern "C"{

    /// AttributesMapの要素数を返します。
    LIBPLATEAU_C_EXPORT int LIBPLATEAU_C_API plateau_attributes_map_get_key_count(
            const AttributesMap* const attributesMap){
        API_TRY {
            return attributesMap->size();
        }
        API_CATCH;
        return -1;
    }

    /// AttributesMapの各キーの文字列のバイト数をint配列 out_sizes に格納します。
    /// out_sizes は AttributesMap の要素数以上のメモリが確保されていることが前提であり、そうでないとアクセス違反です。
    /// DLLで文字列をやりとりするときに長さ情報が欲しいための関数です。
    LIBPLATEAU_C_EXPORT void plateau_attributes_map_get_key_sizes(
            const AttributesMap* const attributesMap,
            int* const out_sizes){

        API_TRY {
            int i = 0;
            for (const auto &pair: *attributesMap) {
                out_sizes[i] = pair.first.size();
                i++;
            }
        }
        API_CATCH;
    }

    /// AttributesMapの各キーを文字列の配列 out_keys に格納します。
    /// DLLの利用者が out_keys の各文字列ポインタから何バイト読み出せば良いかを知るには
    /// 上の関数から要素数と各keyのバイト数を取得すれば良いです。
    /// out_keys の各要素が必要なメモリを確保していなければアクセス違反となります。
    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_attributes_map_get_keys(
            const AttributesMap* const attributesMap,
            char** out_keys
            ){

        API_TRY{
            int i=0;
            for(const auto &pair : *attributesMap){
                char* string_i = out_keys[i];
                strcpy(string_i, pair.first.c_str());
                i++;
            }
        }
        API_CATCH;
    }


}