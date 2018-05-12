/**
 * @file fw_debug_log_listener.h
 * @author Kamai Masayoshi
 */
#ifndef FW_DEBUG_LOG_LISTENER_H_
#define FW_DEBUG_LOG_LISTENER_H_

#include "debug/fw_debug_log_type.h"


BEGIN_NAMESPACE_FW

/**
 * @class DebugLogListner
 */
class FW_DLL DebugLogListener {
public:
    /**
     * @brief 監視レベルマスクをセット
     */
    FW_INLINE void SetLogLevelMask(const sint32_t mask) { logLevelMask = mask;}
    
    /**
     * @brief 監視レベルマスクを取得
     */
    FW_INLINE sint32_t GetLogLevelMask() const { return logLevelMask; }

    /**
     * @brief 出力
     */
    virtual sint32_t ReceiveDebugLog(const DebugLogLevel level, const str_t msg) {
        return 0;
    }
    
    /**
     * @brief 内部バッファを全て書き出す
     */
    virtual void Flush() {
    }


protected:
    /**
     * @brief コンストラクタ
     */
    DebugLogListener() : logLevelMask(kDebugLogLevelMaskAll) {
    }
    
    /**
     * @brief デストラクタ
     */
    virtual ~DebugLogListener() {
    }
    
    
private:
    uint32_t    logLevelMask;
};

END_NAMESPACE_FW

#endif  // FW_DEBUG_LOG_LISTENER_H_
