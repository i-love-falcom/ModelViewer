/**
 * @file fw_debug_log.h
 * @author Kamai Masayoshi
 */
#ifndef FW_DEBUG_LOG_H_
#define FW_DEBUG_LOG_H_

#include <stdarg.h>
#include "debug/fw_debug_log_type.h"

BEGIN_NAMESPACE_FW

class DebugLogListener;

/**
 * @class DebugLog
 */
class FW_DLL DebugLog {
public:
    enum {
        kMaxLogLen      = 1010,
        kMaxIdentLen    = 63,

    };

    /**
     * @brief 初期化
     * @attansion スレッドセーフではありません
     */
    static void Init(const sint32_t options = kDebugLogOptDefault);

    /**
     * @brief 終了処理
     * @attansion スレッドセーフではありません
     */
    static void Shutdown();

    /**
     * @brief デバッグログリスナを登録する
     * @attansion スレッドセーフではありません
     * @param[in] listener 登録するデバッグログリスナ
     * @return 登録できればtrueを返す
     */
    static bool RegisterDebugLogListener(DebugLogListener * listener);

    /**
     * @brief デバッグログリスナの登録を解除する
     * @attansion スレッドセーフではありません
     * @param[in] listener 登録解除するデバッグログリスナ
     * @return 登録解除できればtrueを返す
     */
    static bool UnregisterDebugLogListener(DebugLogListener * listener);

    /**
     * @brief メッセージを出力
     */
    static void VSendDebugLog(const DebugLogLevel level, const str_t fmt, va_list ap);

    /**
     * @brief 内部バッファを全て書き出す
     */
    static void Flush();

    /**
     * @brief メッセージを出力
     */
    static FW_INLINE void SendDebugLog(const DebugLogLevel level, const str_t fmt, ...) {
        va_list vl;
        va_start(vl, fmt);
        VSendDebugLog(level, fmt, vl);
        va_end(vl);
    }
    
    /**
     * @brief 致命的なエラーレベルのメッセージを出力
     */
    static FW_INLINE void FatalError(const str_t fmt, ...) {
        va_list vl;
        va_start(vl, fmt);
        VSendDebugLog(kDebugLogLevelFatalError, fmt, vl);
        va_end(vl);
    }

    /**
     * @brief エラーレベルのメッセージを出力
     */
    static FW_INLINE void Error(const str_t fmt, ...) {
        va_list vl;
        va_start(vl, fmt);
        VSendDebugLog(kDebugLogLevelError, fmt, vl);
        va_end(vl);
    }

    /**
     * @brief 警告レベルのメッセージを出力
     */
    static FW_INLINE void Warning(const str_t fmt, ...) {
        va_list vl;
        va_start(vl, fmt);
        VSendDebugLog(kDebugLogLevelWarning, fmt, vl);
        va_end(vl);
    }

    /**
     * @brief 情報レベルのメッセージを出力
     */
    static FW_INLINE void Info(const str_t fmt, ...) {
        va_list vl;
        va_start(vl, fmt);
        VSendDebugLog(kDebugLogLevelInfo, fmt, vl);
        va_end(vl);
    }

    /**
     * @brief デバッグレベルのメッセージを出力
     */
    static FW_INLINE void Debug(const str_t fmt, ...) {
        va_list vl;
        va_start(vl, fmt);
        VSendDebugLog(kDebugLogLevelDebug, fmt, vl);
        va_end(vl);
    }


private:
    DebugLog & operator =(const DebugLog &);
    DebugLog(const DebugLog &);
    DebugLog();
};

END_NAMESPACE_FW

#endif  // FW_DEBUG_LOG_H_
