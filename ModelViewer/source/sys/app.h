/**
 * @file app.h
 * @author Kamai Masayoshi
 */
#ifndef APP_H_
#define APP_H_

/**
 * @struct AppParms
 */
struct AppParms {
    HINSTANCE   hInstance;
    HINSTANCE   hPrevInstance;
    int         cmdShow;
};

// コマンドライン
typedef NAMESPACE_FW vector<NAMESPACE_FW tstring>   CommandLineArgs;


#if defined(FW_PLATFORM_WIN32)
/**
 * @struct 
 */
struct DropFileArgs {
    POINT                                       dropPoint;
    NAMESPACE_FW vector<NAMESPACE_FW tstring>   dropFiles;
};

/**
 * @struct 
 */
struct MouseStatus {

};

#endif


/**
 * @class App
 */
class App {
public:
    enum {
        kMaxNameLen = 127
    };

    /**
     * @brief 
     */
    static App & Instance();

    /*
     * @brief 初期化
     * @param[in] args 引数
     */
    void Initialize(AppParms & parms, CommandLineArgs & args);

    /**
     * @brief 終了処理
     */
    void Shutdown();

    /**
     * @brief 実行
     * @return アプリケーションの戻り値
     */
    int Run();

    /**
     * @brief クライアント領域の横幅
     */
    uint16_t GetClientWidth() const { return clientWidth; }
    
    /**
     * @brief クライアント領域の高さ
     */
    uint16_t GetClientHeight() const { return clientHeight; }

#if defined(FW_PLATFORM_WIN32)
    /**
     * @brief 規定のウィンドウハンドルを取得
     */
    HWND GetWindowHandle() const { return hWnd; }
#endif


    /**
     * @brief サスペンドへの移行処理
     */
    void OnSuspend(void * args);

    /**
     * @brief サスペンドからの復帰処理
     */
    void OnResume(void * args);

#if defined(FW_PLATFORM_WIN32)
    /**
     * @brief ファイルがドロップされた際の処理
     */
    void OnDropFiles(DropFileArgs * args);

    /**
     * @brief フォーカス取得時の処理
     */
    void OnSetFocus(void * args);

    /**
     * @brief フォーカス喪失時の処理
     */
    void OnKillFocus(void * args);
#endif



    /**
     * @brief コンストラクタ
     */
    App();


private:
    uint16_t    clientWidth;
    uint16_t    clientHeight;


#if defined(FW_PLATFORM_WIN32)
    HINSTANCE   hInstance;
    HINSTANCE   hPrevInstance;
    int         cmdShow;

    HWND        hWnd;
    char_t      appName[kMaxNameLen + 1];

    friend LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
#endif
};

#endif  // APP_H_
