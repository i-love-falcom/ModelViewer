/**
* @file app.cpp
* @author Kamai Masayoshi
*/
#include "stdafx.h"
#include "../resource/resource.h"
#include "sys/app.h"
#include "sys/app_wndproc.h"



App::App()
: clientWidth(1920)
, clientHeight(1080)
#if defined(FW_PLATFORM_WIN32)
, hWnd(nullptr)
#endif
{
}

App & App::Instance() {
    static App s_instance;
    return s_instance;
}

void App::Initialize(AppParms & parms, CommandLineArgs & args) {
#if defined(FW_PLATFORM_WIN32)
    hInstance       = parms.hInstance;
    hPrevInstance   = parms.hPrevInstance;
    cmdShow         = parms.cmdShow;

    // アプリ名をセット
    NAMESPACE_FW tstring::SPrintf(appName, ARRAY_SIZEOF(appName), _T("ModelViewer"));

    WNDCLASSEX wcex;

    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MODELVIEWER));
    wcex.hIconSm        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = appName;

    RegisterClassEx(&wcex);


    hWnd = CreateWindow(appName, appName, WS_CAPTION | WS_SYSMENU | WS_BORDER | WS_DLGFRAME,
        0, 0, clientWidth, clientHeight, nullptr, nullptr, hInstance, nullptr);
    assert(hWnd != nullptr);

    ShowWindow(hWnd, parms.cmdShow);
    UpdateWindow(hWnd);
#endif
}

void App::Shutdown() {
#if defined(FW_PLATFORM_WIN32)
    ShowWindow(hWnd, SW_HIDE);
    DestroyWindow(hWnd);

    UnregisterClass(appName, hInstance);
#endif
}

BEGIN_NAMESPACE_NONAME
/**
 * @internal
 * @class DefaultDebugLogListener
 */
class DefaultDebugLogListener : public NAMESPACE_FW DebugLogListener {
public:
    virtual sint32_t ReceiveDebugLog(const DebugLogLevel level, const str_t msg) FW_OVERRIDE {
#if defined(FW_PLATFORM_WIN32)
        ::OutputDebugString(msg);
#else
        fprintf(stderr, msg);
#endif
        return 0;
    }
};

static DefaultDebugLogListener  s_debugLogListener;
END_NAMESPACE_NONAME

int App::Run() {
    // デバッグログリスナ登録
    NAMESPACE_FW DebugLog::RegisterDebugLogListener(&s_debugLogListener);
    
    //! @todo メインスレッド生成

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void App::OnSuspend(void * args) {
    NAMESPACE_FW DebugLog::Info(_T("OnSuspend\n"));
}

void App::OnResume(void * args) {
    NAMESPACE_FW DebugLog::Info(_T("OnResume\n"));
}

#if defined(FW_PLATFORM_WIN32)
void App::OnDropFiles(DropFileArgs * args) {
    NAMESPACE_FW DebugLog::Info(_T("OnDropFiles\n"));
}

void App::OnSetFocus(void * args) {
    NAMESPACE_FW DebugLog::Info(_T("OnSetFocus\n"));
}

void App::OnKillFocus(void * args) {
    NAMESPACE_FW DebugLog::Info(_T("OnKillFocus\n"));
}
#endif
