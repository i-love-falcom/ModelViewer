/**
* @file app.cpp
* @author Kamai Masayoshi
*/
#include "stdafx.h"
#include "../resource/resource.h"
#include "sys/app.h"
#include "sys/app_wndproc.h"
#include "main_thread.h"

USING_NAMESPACE_FW


BEGIN_NAMESPACE_NONAME
static const str_t  s_debugLogLevelString[kDebugLogLevelCount] = {
    _T("FatalError"),
    _T("Error"),
    _T("Warning"),
    _T("Info"),
    _T("Debug"),
};

/**
 * @internal
 * @class DefaultDebugLogListener
 */
class DefaultDebugLogListener : public DebugLogListener {
public:
    virtual sint32_t ReceiveDebugLog(const DebugLogLevel level, const str_t msg) FW_OVERRIDE {
        char_t log[DebugLog::kMaxLogLen + 1];
        tstring::SPrintf(log, ARRAY_SIZEOF(log), _T("%s : %s"), s_debugLogLevelString[level], msg);

#if defined(FW_PLATFORM_WIN32)
        ::OutputDebugString(log);
#else
        uint64_t writeSize = tstring::Length(log) * sizeof(char_t);
        FwFileWrite(FwFile::StdOutputDevice(), log, writeSize, nullptr);
#endif
        return 0;
    }
};

static DefaultDebugLogListener  s_debugLogListener;
END_NAMESPACE_NONAME


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
    // デバッグログリスナ登録
    DebugLog::RegisterDebugLogListener(&s_debugLogListener);
    
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
    FwAssert(hWnd != nullptr);

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


int App::Run() {
    MainThread mainThread;
    mainThread.StartMainThread(_T("ModelViewerMain"));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    mainThread.TerminateMainThread();

    return (int)msg.wParam;
}

void App::OnSuspend(void * args) {
    DebugLog::Info(_T("OnSuspend\n"));
}

void App::OnResume(void * args) {
    DebugLog::Info(_T("OnResume\n"));
}

#if defined(FW_PLATFORM_WIN32)
void App::OnDropFiles(DropFileArgs * args) {
    DebugLog::Info(_T("OnDropFiles\n"));
}

void App::OnSetFocus(void * args) {
    DebugLog::Info(_T("OnSetFocus\n"));
}

void App::OnKillFocus(void * args) {
    DebugLog::Info(_T("OnKillFocus\n"));
}
#endif
