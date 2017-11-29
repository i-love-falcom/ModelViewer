/**
 * @file main_thread.h
 * @author Kamai Masayoshi
 */
#ifndef MAIN_THREAD_H_
#define MAIN_THREAD_H_


/**
 * @class MainThread
 */
class MainThread : public NAMESPACE_FW Thread {
public:
    enum {
        kDefaultStackSize = 1 * 1024 * 1024,
    };

    /**
     * @brief フレームの開始
     */
    void OnStartFrame();

    /**
     * @brief フレーム更新処理
     */
    void OnFrameUpdate(const float delta);

    /**
     * @brief フレームの終端
     */
    void OnEndFrame();


    /**
     * @brief スレッドオブジェクトのエントリポイント
     * @param[in] userArgs 任意のパラメータ
     * @return 終了コード
     */
    virtual sint32_t Invoke(void * userArgs) FW_OVERRIDE;

    /**
     * @brief メインスレッドの開始
     */
    void StartMainThread(const str_t name);

    /**
     * @brief メインスレッドの終了
     */
    void TerminateMainThread();

    /**
     * @brief コンストラクタ
     */
    MainThread();

    /**
     * @brief デストラクタ
     */
    ~MainThread();


private:
    /**
     * @internal
     * @brief メインスレッドの初期化
     */
    sint32_t Initialzie(void * userArgs);

    /**
     * @internal
     * @brief メインスレッドの初期化
     */
    void Shutdown();



    void *  args;
};

#endif  // MAIN_THREAD_H_
