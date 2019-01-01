/**
 * @file fw_debug_log.cpp
 * @author Kamai Masayoshi
 */
#include "precompiled.h"
#include "debug/fw_debug_log.h"
#include "debug/fw_debug_log_listener.h"
#include "container/fw_array.h"

BEGIN_NAMESPACE_FW
BEGIN_NAMESPACE_NONAME

/**
 * @class DebugLogImpl
 */
class DebugLogImpl {
public:
    enum {
        MAX_LISTENER_COUNT = 16,
    };

    /**
     * @brief 初期化
     */
    void Init(const sint32_t opts) {
        Shutdown();

        options = opts;
    }

    /**
     * @brief 終了処理
     */
    void Shutdown() {
        debugLogListeners.clear();
    }

    /**
     * @brief デバッグログリスナを登録する
     */
    bool RegisterDebugLogListener(DebugLogListener * listener) {
        if (listener != nullptr) {
            for (int i = 0; i < debugLogListeners.size(); ++i) {
                if (debugLogListeners[i] == nullptr) {
                    debugLogListeners[i] = listener;
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @brief デバッグログリスナの登録を解除する
     */
    bool UnregisterDebugLogListener(DebugLogListener * listener) {
        bool foundListener = false;
        if (listener != nullptr) {
            for (int i = 0; i < debugLogListeners.size(); ++i) {
                if (debugLogListeners[i] == listener) {
                    debugLogListeners[i] = nullptr;
                    foundListener = true;
                }
            }
        }
        return foundListener;
    }

    /**
     * @brief 出力
     */
    void VSendDebugLog(const DebugLogLevel level, const str_t fmt, va_list ap) {
        char_t logBuffer[DebugLog::kMaxLogLen + 1];
        tstring::VSPrintf(logBuffer, FW_ARRAY_SIZEOF(logBuffer), fmt, ap);

        for (auto & listener : debugLogListeners) {
            if (listener != nullptr && (listener->GetLogLevelMask() & static_cast<sint32_t>(level)) != 0) {
                listener->ReceiveDebugLog(level, logBuffer);
            }
        }
    }

    /**
     * @brief 内部バッファを全て書き出す
     */
    void Flush() {
        for (auto & listener : debugLogListeners) {
            if (listener != nullptr) {
                listener->Flush();
            }
        }
    }
       
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
    typedef array<DebugLogListener *, MAX_LISTENER_COUNT> DebugLogListenerArray_t;

    DebugLogListenerArray_t     debugLogListeners;
    uint32_t                    options;
};

// DebugLogインスタンス
static DebugLogImpl     s_debugLogImpl;

END_NAMESPACE_NONAME

//--------------------------------------------------------
//
// DebugLog
//
//--------------------------------------------------------
void DebugLog::Init(const sint32_t options) {
    s_debugLogImpl.Init(options);
}

void DebugLog::Shutdown() {
    s_debugLogImpl.Shutdown();
}

bool DebugLog::RegisterDebugLogListener(DebugLogListener * listener) {
    return s_debugLogImpl.RegisterDebugLogListener(listener);
}

bool DebugLog::UnregisterDebugLogListener(DebugLogListener * listener) {
    return s_debugLogImpl.UnregisterDebugLogListener(listener);
}

void DebugLog::VSendDebugLog(const DebugLogLevel level, const str_t fmt, va_list ap) {
    s_debugLogImpl.VSendDebugLog(level, fmt, ap);
}

void DebugLog::Flush() {
    s_debugLogImpl.Flush();
}

END_NAMESPACE_FW