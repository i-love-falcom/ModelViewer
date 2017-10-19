/**
 * @file fw_debug_log.h
 * @author Kamai Masayoshi
 */
#ifndef FW_DEBUG_LOG_H_
#define FW_DEBUG_LOG_H_

#include "debug/fw_debug_log_type.h"

BEGIN_NAMESPACE_FW

class DebugLogStream;

/**
 * @class DebugLog
 */
class FW_EXPORT DebugLog {
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
     * @brief 既にログが初期化されているか調べる
     */
    static bool IsInit();

    /**
     * @brief デバッグログストリームを登録する
     * @attansion スレッドセーフではありません
     * @param[in] stream 登録するデバッグログストリーム
     * @return 登録できればtrueを返す
     */
    static bool RegisterDebugLogStream(DebugLogStream * stream);

    /**
     * @brief デバッグログストリームの登録を解除する
     * @attansion スレッドセーフではありません
     * @param[in] stream 登録解除するデバッグログストリーム
     * @return 登録解除できればtrueを返す
     */
    static bool UnregisterDebugLogStream(DebugLogStream * stream);

    /**
     * @brief メッセージを出力
     */
    static void PrintLine(const sint32_t level, const str_t fmt, ...);
    
    /**
     * @brief メッセージを出力
     */
    static void VPrintLine(const sint32_t level, const str_t fmt, va_list ap);

    /**
     * @brief 致命的なエラーレベルのメッセージを出力
     */
    static void FatalError(const str_t fmt, ...);

    /**
     * @brief エラーレベルのメッセージを出力
     */
    static void Error(const str_t fmt, ...);

    /**
     * @brief 警告レベルのメッセージを出力
     */
    static void Warning(const str_t fmt, ...);

    /**
     * @brief 情報レベルのメッセージを出力
     */
    static void Info(const str_t fmt, ...);

    /**
     * @brief デバッグレベルのメッセージを出力
     */
    static void Debug(const str_t fmt, ...);

    /**
     * @brief 内部バッファを全て書き出す
     */
    static void Flush();

private:
    DebugLog & operator =(const DebugLog &);
    DebugLog(const DebugLog &);
    DebugLog();
};

END_NAMESPACE_FW

#endif  // FW_DEBUG_LOG_H_
