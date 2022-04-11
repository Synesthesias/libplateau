#pragma once

// P/Invoke�p�Ȃ��߃G�N�X�|�[�g�̂�
#ifdef _WIN32
#  define LIBPLATEAU_C_API __stdcall
#  define LIBPLATEAU_C_EXPORT  __declspec(dllexport)
#else
#  define LIBPLATEAU_C_API
#  define LIBPLATEAU_C_EXPORT
#endif

// ������ API_TRY �� API_CATCH �ň͂ނ��Ƃŗ�O���o�͂ł��܂��B
#define API_TRY try

#define API_CATCH \
catch (std::exception& e) {\
    std::cout << e.what() << std::endl;\
}\
catch (...) {\
    std::cout << "Unknown error occured." << std::endl;\
}