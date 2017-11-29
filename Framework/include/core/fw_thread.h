/**
 * @file fw_thread.h
 * @author Kamai Masayoshi
 */
#ifndef FW_THREAD_H_
#define FW_THREAD_H_

BEGIN_NAMESPACE_FW
struct ThreadInfo;

#if defined(FW_PLATFORM_WIN32)
typedef uintptr_t                   threadId_t;
typedef uintptr_t                   threadHandle_t;
#else
typedef std::thread::id             threadId_t;
typedef std::therad::native_handle  threadHandle_t;
#endif
typedef uint64_t                    threadAffinity_t;

static const size_t             MaxThreadNameLen = 255;
static const uint32_t           DefaultThreadFlags = 0;
static const uint32_t           DefaultThreadStackSize = 128 * 1024;
static const threadAffinity_t   DefaultThreadAffinity = static_cast<threadAffinity_t>(0xffffffffffffffff);


/**
 * @enum threadPriority_t
 */
enum threadPriority_t {
#if defined(FW_PLATFORM_WIN32)
    kThreadPriorityMin          = THREAD_BASE_PRIORITY_IDLE,
    kThreadPriorityMax          = THREAD_PRIORITY_TIME_CRITICAL,
    
    kTheadPriorityLowest        = THREAD_PRIORITY_LOWEST,           ///< 最低の優先度
    kTheadPriorityBelowNormal   = THREAD_PRIORITY_BELOW_NORMAL,     ///< 基準より一段低い優先度
    kTheadPriorityNormal        = THREAD_PRIORITY_NORMAL,           ///< 基準となる優先度
    kTheadPriorityAboveNormal   = THREAD_PRIORITY_ABOVE_NORMAL,     ///< 基準より一段高い優先度
    kTheadPriorityHighest       = THREAD_PRIORITY_HIGHEST,          ///< 最高の優先度
#else
    kThreadPriorityMin          = 256,
    kThreadPriorityMax          = 1024,
    
    kTheadPriorityLowest        = (kThreadPriorityMax - kThreadPriorityMin) * 1 / 6 + kThreadPriorityMin,    ///< 最低の優先度
    kTheadPriorityBelowNormal   = (kThreadPriorityMax - kThreadPriorityMin) * 2 / 6 + kThreadPriorityMin,    ///< 基準より一段低い優先度
    kTheadPriorityNormal        = (kThreadPriorityMax - kThreadPriorityMin) * 3 / 6 + kThreadPriorityMin,    ///< 基準となる優先度
    kTheadPriorityAboveNormal   = (kThreadPriorityMax - kThreadPriorityMin) * 4 / 6 + kThreadPriorityMin,    ///< 基準より一段高い優先度
    kTheadPriorityHighest       = (kThreadPriorityMax - kThreadPriorityMin) * 5 / 6 + kThreadPriorityMin,    ///< 最高の優先度
#endif
};

/**
 * @struct ThreadDesc
 */
class ThreadDesc {
public:
    void *              userArgs;
    uint32_t            flags;
    uint32_t            stackSize;
    sint32_t            priority;
    threadAffinity_t    affinity;
    char_t              name[MaxThreadNameLen + 1];

    /**
     * @brief 初期化
     */
    void Init() {
        userArgs = nullptr;
        flags = DefaultThreadFlags;
        stackSize = DefaultThreadStackSize;
        priority = kTheadPriorityNormal;
        affinity = DefaultThreadAffinity;
        name[0] = _T('\0');
    }
};

/**
 * @class Thread
 */
class FW_DLL Thread {
public:
    /**
     * @brief スレッドオブジェクトのエントリポイント
     * @param[in] userArgs 任意のパラメータ
     * @return 終了コード
     */
    virtual sint32_t Invoke(void * userArgs) = 0;

    /**
     * @brief 終了処理
     */
    void Shutdown();

    /**
     * @brief スレッドを開始する
     * @param[in] ThreadDesc スレッド詳細
     */
    void Start(const ThreadDesc * desc);
    
    /**
     * @brief ワーカースレッドを開始する
     * @param[in] ThreadDesc スレッド詳細
     */
    void StartWorker(const ThreadDesc * desc);
    
    /**
     * @brief スレッドを終了してリソースを解放する
     */
    void TerminateThread();
    
    /**
     * @brief ワーカースレッドを起こす
     */
    void RaiseWorkerThread();
    
    /**
     * @brief スレッド処理の終了を待つ
     */
    sint32_t WaitThread(const uint32_t millisecond = FW_WAIT_INFINITE);
    
    /**
     * @brief 終了コードを取得
     */
    uint32_t GetExitCode();
    
    /**
     * @brief スレッドIDを取得
     */
    FW_INLINE threadId_t GetThreadId() const { return threadId; }

    /**
     * @brief スレッドハンドルを取得
     */
    FW_INLINE threadHandle_t GetThreadHandle() const { return threadHandle; }

    /**
     * @brief ThreadDescを取得
     */
    FW_INLINE const ThreadDesc & GetDesc() const { return desc; }

    /**
     * @brief ThreadInfoを取得
     * @memo 内部使用
     */
    FW_INLINE ThreadInfo * GetThreadInfo() const { return threadInfo; }

    /**
     * @brief 現在のスレッドIDを取得
     */
    static threadId_t GetCurrentThreadId();

    /**
     * @brief 現在のスレッドを終了する
     * @param[in] exitCode 終了コード
     */
    static void Exit(uint32_t exitCode);
    
    /**
     * @brief 現在のスレッドを休止する
     * @param[in] millisecond 休止する時間(ミリ秒)
     */
    static void Sleep(uint32_t millisecond);
    
    /**
     * @brief 現在のスレッドに割り当てられた残り時間を、別のスレッドに明け渡す
     */
    static void YieldThread();
    
    /**
     * @brief 指定したスレッドのいずれかが終了するまで待つ
     * @param[in] threadArray スレッドの配列
     * @param[in] count       threadArrayに格納されるスレッド数
     * @param[in] result      結果コード
     * @param[in] millisecond タイムアウト時間(msec)
     */
    static void WaitAny(const Thread * threadArray[], const uint32_t count, sint32_t * results = nullptr, const uint32_t millisecond = FW_WAIT_INFINITE);

    /**
     * @brief 指定した全てのスレッドが終了するまで待つ
     * @param[in] threadArray スレッドの配列
     * @param[in] count       threadArrayに格納されるスレッド数
     * @param[in] result      結果コード
     * @param[in] millisecond タイムアウト時間(msec)
     */
    static void WaitAll(const Thread * threadArray[], const uint32_t count, sint32_t * results = nullptr, const uint32_t millisecond = FW_WAIT_INFINITE);


protected:
    /**
     * @brief コンストラクタ
     */
    Thread();
    
    /**
     * @brief デストラクタ
     */
    ~Thread();
    
    
    threadId_t      threadId;
    threadHandle_t  threadHandle;
    ThreadDesc      desc;
    ThreadInfo *    threadInfo;

#if defined(FW_PLATFORM_WIN32)
#else
    std::thread     threadInstance;
#endif

 private:
     void StartThread(const bool workerThread);
};

END_NAMESPACE_FW

#endif  // FW_THREAD_H_
