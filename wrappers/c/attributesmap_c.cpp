#include <citygml/attributesmap.h>
#include <string.h>
#include "libplateau_c.h"

using namespace citygml;

extern "C"{

    /// AttributesMap�̗v�f����Ԃ��܂��B
    LIBPLATEAU_C_EXPORT int LIBPLATEAU_C_API plateau_attributes_map_get_key_count(
            const AttributesMap* const attributesMap){
        API_TRY {
            return attributesMap->size();
        }
        API_CATCH;
        return -1;
    }

    /// AttributesMap�̊e�L�[�̕�����̃o�C�g����int�z�� out_sizes �Ɋi�[���܂��B
    /// out_sizes �� AttributesMap �̗v�f���ȏ�̃��������m�ۂ���Ă��邱�Ƃ��O��ł���A�����łȂ��ƃA�N�Z�X�ᔽ�ł��B
    /// DLL�ŕ���������Ƃ肷��Ƃ��ɒ�����񂪗~�������߂̊֐��ł��B
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

    /// AttributesMap�̊e�L�[�𕶎���̔z�� out_keys �Ɋi�[���܂��B
    /// DLL�̗��p�҂� out_keys �̊e������|�C���^���牽�o�C�g�ǂݏo���Ηǂ�����m��ɂ�
    /// ��̊֐�����v�f���Ɗekey�̃o�C�g�����擾����Ηǂ��ł��B
    /// out_keys �̊e�v�f���K�v�ȃ��������m�ۂ��Ă��Ȃ���΃A�N�Z�X�ᔽ�ƂȂ�܂��B
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