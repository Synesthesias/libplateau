#include <iostream>
#include <obj_writer.h>

#include "libplateau_c.h"

using namespace citygml;

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
    // ����I���̏ꍇ��0, �ُ�I���̏ꍇ�͂���ȊO��Ԃ��܂��B
    // ��̓I�ɂ́A���������݂��Ȃ��܂��͒l���󕶎���ł���ꍇ��-1��Ԃ��A�o�b�t�@�T�C�Y������Ȃ��ꍇ��-2��Ԃ��܂��B
    LIBPLATEAU_C_EXPORT int LIBPLATEAU_C_API plateau_object_get_attribute(const Object* object, const char* name, char* outStrBuffer, int outStrBufferSize){
        API_TRY{
            auto attrValueStr = object->getAttribute(std::string(name));
            auto attrValueChars = attrValueStr.c_str();
             if(attrValueStr.empty()){
                 // ���������݂��Ȃ��ꍇ�͋󕶎��񂪕Ԃ�̂ŁA�󕶎���̂Ƃ��Ɉُ�I���Ƃ��܂��B
                 return -1;
             }
            if(outStrBufferSize < strlen(attrValueChars)){
                // �o�b�t�@�[�T�C�Y������Ȃ��ꍇ
                return -2;
            }else{
                // �o�b�t�@�[�T�C�Y�������ꍇ
                
                // strcpy ���g���ƁAMSVC���ł� �ustrcpy_s ���g���ׂ��v�ƌx�����ł܂��B
                // �������AMSVC���łȂ���� strcpy_s �͗��p�ł����r���h���ʂ�Ȃ����߂��̌x���͖������܂��B
                strcpy(outStrBuffer, attrValueChars);
                return 0;
            }
        }
        API_CATCH;
        return -99;
    }

    /// �����ƒl�̈ꗗ�� chars* �^�Ŏ擾���܂��B
    /// key1, value1, key2, value2, ... �̏��Ԃŉ��s��؂�� keys_values �Ɋi�[���܂��B
    LIBPLATEAU_C_EXPORT void LIBPLATEAU_C_API plateau_object_get_keys_values(const Object* object, char* out_keys_values, int outBufferSize){
        API_TRY {
            auto &attrs = object->getAttributes();
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
            // TODO �o�b�t�@������Ȃ��Ƃ��̃n���h�����O
            if (outBufferSize >= strlen(chars)) {
                strcpy(out_keys_values, chars);
            }
            // test
            out_keys_values = "testtest\nfoobarfoobar";
        }
        API_CATCH;
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