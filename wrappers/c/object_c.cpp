#include <iostream>
#include <obj_writer.h>

#include "libplateau_c.h"

using namespace citygml;
using namespace libplateau;

extern "C" {
    LIBPLATEAU_C_EXPORT const char* LIBPLATEAU_C_API plateau_object_get_id(
        const Object* object
    ) {
        API_TRY{
            return object->getId().c_str();
        }
        API_CATCH;
        return nullptr;
    }


    // ���������瑮���l���擾���A outStrBuffer �Ɋi�[���܂��B
    // ���ʂ̐��ۂ�߂�l�ɂ��܂��B
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_object_get_attribute(const Object* object, const char* name, char* outStrBuffer, int outStrBufferSize){
        API_TRY{
            auto attrValueStr = object->getAttribute(std::string(name));
            auto attrValueChars = attrValueStr.c_str();
             if(attrValueStr.empty()){
                 // ���������݂��Ȃ��ꍇ�͋󕶎��񂪕Ԃ�̂ŁA�󕶎���̂Ƃ��Ɉُ�I���Ƃ��܂��B
                 return APIResult::ErrorValueNotFound;
             }
            if(outStrBufferSize < strlen(attrValueChars)){
                // �o�b�t�@�[�T�C�Y������Ȃ��ꍇ
                return APIResult::ErrorLackOfBufferSize;
            }else{
                // �o�b�t�@�[�T�C�Y�������ꍇ
                
                // strcpy ���g���ƁAMSVC���ł� �ustrcpy_s ���g���ׂ��v�ƌx�����ł܂��B
                // �������AMSVC���łȂ���� strcpy_s �͗��p�ł����r���h���ʂ�Ȃ����߂��̌x���͖������܂��B
                strcpy(outStrBuffer, attrValueChars);
                return APIResult::Success;
            }
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }

    /// �����ƒl�̈ꗗ�� chars* �^�Ŏ擾���܂��B
    /// key1, value1, key2, value2, ... �̏��Ԃŉ��s��؂�� keys_values �Ɋi�[���܂��B
    LIBPLATEAU_C_EXPORT APIResult LIBPLATEAU_C_API plateau_object_get_keys_values(const Object* object, char* out_keys_values, int outBufferSize){
        API_TRY {
            auto &attrs = object->getAttributes();
            // �����̈ꗗ�𕶎���ɕϊ����܂��B
            auto keys_vals_str = std::string("");
            for (auto attr: attrs) {
                const auto &key = attr.first;
                const auto &value = attr.second.asString();
                keys_vals_str += key;
                keys_vals_str += "\n";
                keys_vals_str += value;
                keys_vals_str += "\n";
            }
            // �Ō�̉��s���폜���܂��B
            if (!keys_vals_str.empty()) keys_vals_str.pop_back();

            auto chars = keys_vals_str.c_str();
            if (outBufferSize >= strlen(chars)) {
                // �擾����������������R�s�[���ďI�����܂��B
                strcpy(out_keys_values, chars);
                return APIResult::Success;
            }else{
                // �o�b�t�@�T�C�Y������Ȃ��ꍇ
                return APIResult::ErrorLackOfBufferSize;
            }
        }
        API_CATCH;
        return APIResult::ErrorUnknown;
    }


    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_object_set_attribute(
        Object* object,
        const char* name,
        const char* value,
        const AttributeType type,
        bool overwrite
    ) {
        API_TRY{
            object->setAttribute(name, value, type, overwrite);
        }
        API_CATCH;
    }

    
}