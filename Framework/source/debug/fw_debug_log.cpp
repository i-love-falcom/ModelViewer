/**
 * @file fw_debug_log.cpp
 * @author Kamai Masayoshi
 */
#include "precompiled.h"
#include "debug/fw_debug_log.h"
#include "debug/fw_debug_log_stream.h"


BEGIN_NAMESPACE_FW

/**
 * @class DebugLogImpl
 */
class DebugLogImpl {
public:
    enum DebugLogImplFlags {
        DEBUGLOGIMPL_FLAGS_INIT = BIT32(31),
    };
    enum {
        MAX_REGISTERD_STREAM_COUNT = 16,
    };

    /**
     * @brief 初期化
     */
    void Init(const sint32_t options);
    
    /**
     * @brief 終了処理
     */
    void Shutdown();

    /**
     * @brief 既に初期化されているか調べる
     */
    bool IsInit();

    /**
     * @brief デバッグログストリームを登録する
     */
    bool RegisterDebugLogStream(DebugLogStream * stream);

    /**
     * @brief デバッグログストリームの登録を解除する
     */
    bool UnregisterDebugLogStream(DebugLogStream * stream);
    
    /**
     * @brief 出力
     */
    void PrintLine(const sint32_t level, const str_t fmt, va_list ap);
    
    /**
     * @brief 内部バッファを全て書き出す
     */
    void Flush();
       
    /**
     * @brief コンストラクタ
     */
    DebugLogImpl() : options(0) {
    }
    
    /**
     * @brief コンストラクタ
     */
    ~DebugLogImpl() {
    }

private:
    typedef array<DebugLogStream *, MAX_REGISTERD_STREAM_COUNT> DebugLogStreamArray_t;

    DebugLogStreamArray_t   debugLogStreams;
    uint32_t                options;
};

END_NAMESPACE_FW