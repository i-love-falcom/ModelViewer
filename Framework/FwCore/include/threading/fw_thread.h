﻿/**
 * @file fw_thread.h
 * @author Kamai Masayoshi
 */
#ifndef FW_THREAD_H_
#define FW_THREAD_H_

#define FW_THREAD_UNKNOWN       (0)
#define FW_THREAD_STL           (1)
#define FW_THREAD_WIN32         (2)

#if FW_BUILD_CONFIG_FORCE_USE_STD_THREAD
#define FW_THREAD           FW_THREAD_STL
#elif defined(FW_PLATFORM_WIN32)
#define FW_THREAD           FW_THREAD_WIN32
#else
#define FW_THREAD           FW_THREAD_STL
#endif

#if FW_THREAD == FW_THREAD_STL
#include <thread>
#elif FW_THREAD == FW_THREAD_UNKNOWN
#error unsupported platform
#endif

BEGIN_NAMESPACE_FW
class FwThreadInfo;

#if FW_THREAD == FW_THREAD_WIN32
using FwThreadId        = uintptr_t;
using FwThreadHandle    = uintptr_t;
#elif FW_THREAD == FW_THREAD_STL
using FwThreadId        = std::thread::id;
using FwThreadHandle    = std::thread::native_handle_type;
#endif
using FwThreadAffinity = uint64_t;
using FwThreadPriority = sint32_t;

static const size_t             FwMaxThreadNameLen          = 255;
static const uint32_t           DefaultFwThreadFlags        = 0;
static const uint32_t           DefaultFwThreadStackSize    = 128 * 1024;
static const FwThreadAffinity   DefaultFwThreadAffinity     = static_cast<FwThreadAffinity>(0xffffffffffffffff);

static const FwThreadPriority   FwThreadPriorityMin         = 256;
static const FwThreadPriority   FwThreadPriorityMax         = 1024;
static const FwThreadPriority   FwThreadPriorityLowest      = (FwThreadPriorityMax - FwThreadPriorityMin) * 1 / 6 + FwThreadPriorityMin;    ///< 最低の優先度
static const FwThreadPriority   FwThreadPriorityBelowNormal = (FwThreadPriorityMax - FwThreadPriorityMin) * 2 / 6 + FwThreadPriorityMin;    ///< 基準より一段低い優先度
static const FwThreadPriority   FwThreadPriorityNormal      = (FwThreadPriorityMax - FwThreadPriorityMin) * 3 / 6 + FwThreadPriorityMin;    ///< 基準となる優先度
static const FwThreadPriority   FwThreadPriorityAboveNormal = (FwThreadPriorityMax - FwThreadPriorityMin) * 4 / 6 + FwThreadPriorityMin;    ///< 基準より一段高い優先度
static const FwThreadPriority   FwThreadPriorityHighest     = (FwThreadPriorityMax - FwThreadPriorityMin) * 5 / 6 + FwThreadPriorityMin;    ///< 最高の優先度


/**
 * @struct ThreadDesc
 */
class FwThreadDesc {
public:
    void *              userArgs;
    uint32_t            flags;
    uint32_t            stackSize;
    FwThreadPriority    priority;
    FwThreadAffinity    affinity;
    char_t              name[FwMaxThreadNameLen + 1];

    /**
     * @brief 初期化
     */
    void Init() {
        userArgs    = nullptr;
        flags       = DefaultFwThreadFlags;
        stackSize   = DefaultFwThreadStackSize;
        priority    = FwThreadPriorityNormal;
        affinity    = DefaultFwThreadAffinity;
        name[0]     = _T('\0');
    }
};

/**
 * @class FwThread
 */
class FW_DLL FwThread {
public:
    /**
     * @brief スレッドオブジェクトの初期化処理
     * @param[in] userArgs 任意のパラメータ
     */
    virtual void InitializeThreadFunc(void * userArgs) {}

    /**
     * @brief スレッドオブジェクトのエントリポイント
     * @param[in] userArgs 任意のパラメータ
     * @return 終了コード
     */
    virtual sint32_t ThreadFunc(void * userArgs) = 0;

    /**
     * @brief スレッドオブジェクトの終了処理
     * @param[in] returnValue ThreadFuncで返した終了コード
     * @param[in] userArgs    任意のパラメータ
     */
    virtual void ShutdownThreadFunc(const sint32_t returnValue, void * userArgs) {}

    /**
     * @brief 終了処理
     */
    void Shutdown();

    /**
     * @brief スレッドを開始する
     * @param[in] ThreadDesc スレッド詳細
     */
    void Start(const FwThreadDesc * desc);
    
    /**
     * @brief ワーカースレッドを開始する
     * @param[in] ThreadDesc スレッド詳細
     */
    void StartWorker(const FwThreadDesc * desc);
    
    /**
     * @brief スレッドを終了してリソースを解放する
     */
    void TerminateThread();
    
    /**
     * @brief ワーカースレッドを起こして再実行する
     */
    void RestartThread();
    
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
    FwThreadId GetThreadId() const;

    /**
     * @brief スレッドハンドルを取得
     */
    FwThreadHandle GetThreadHandle();

    /**
     * @brief ThreadDescを取得
     */
    FW_INLINE const FwThreadDesc & GetDesc() const { return desc; }

    /**
     * @brief ThreadInfoを取得
     * @memo 内部使用
     */
    FW_INLINE FwThreadInfo * GetThreadInfo() const { return threadInfo; }

    /**
     * @brief 現在のスレッドIDを取得
     */
    static FwThreadId GetCurrentThreadId();

    /**
     * @brief 現在のスレッドを休止する
     * @param[in] millisecond 休止する時間(ミリ秒)
     */
    static void Sleep(uint32_t millisecond);
    
    /**
     * @brief 現在のスレッドに割り当てられた残り時間を、別のスレッドに明け渡す
     */
    static void YieldThread();
    

protected:
    /**
     * @brief コンストラクタ
     */
    FwThread();
    
    /**
     * @brief デストラクタ
     */
    ~FwThread();
    
    
    FwThreadDesc    desc;
    FwThreadInfo *  threadInfo;

#if FW_THREAD == FW_THREAD_WIN32
    FwThreadId      threadId;
    FwThreadHandle  threadHandle;
#endif

 private:
     void StartThread(const bool workerThread);
};

END_NAMESPACE_FW

#endif  // FW_THREAD_H_
