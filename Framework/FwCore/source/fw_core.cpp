/**
 * @file fw_core.cpp
 * @author Kamai Masayoshi
 */
#include "precompiled.h"

#if defined(FW_PLATFORM_WIN32) && !defined(STATIC_LIB)
BEGIN_NAMESPACE_NONAME
/**
 * @brief 現在のプロセス空間にDLLをロードした際に呼ばれる
 */
BOOL OnProcessAttach(HINSTANCE hInstance, DWORD reason, LPVOID reserved) {
    return TRUE;
}

/**
 * @brief 現在のプロセス空間からDLLをアンロードした際に呼ばれる
 */
BOOL OnProcessDetach(HINSTANCE hInstance, DWORD reason, LPVOID reserved) {
    return TRUE;
}

/**
 * @brief 新しいスレッド生成時に呼ばれる
 */
BOOL OnThreadAttach(HINSTANCE hInstance, DWORD reason, LPVOID reserved) {
    return TRUE;
}

/**
 * @brief 既存スレッド終了時に呼ばれる
 */
BOOL OnThreadDetach(HINSTANCE hInstance, DWORD reason, LPVOID reserved) {
    return TRUE;
}
END_NAMESPACE_NONAME

/**
 * @brief DLLエントリポイント
 */
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:    return OnProcessAttach(hInstance, reason, reserved);
    case DLL_PROCESS_DETACH:    return OnProcessDetach(hInstance, reason, reserved);
    case DLL_THREAD_ATTACH:     return OnThreadAttach(hInstance, reason, reserved);
    case DLL_THREAD_DETACH:     return OnThreadDetach(hInstance, reason, reserved);
    default:                    break;
    }
    return TRUE;
}
#endif

