/**
 * @file fw_debug_log_type.h
 * @author Kamai Masayoshi
 */
#ifndef FW_DEBUG_LOG_TYPE_H_
#define FW_DEBUG_LOG_TYPE_H_

BEGIN_NAMESPACE_FW

/**
 * @enum DebugLogLevel
 */
enum DebugLogLevel {
    kDebugLogLevelFatalError,   ///< 致命的なエラー
    kDebugLogLevelError,        ///< エラー
    kDebugLogLevelWarning,      ///< 警告
    kDebugLogLevelInfo,         ///< 情報
    kDebugLogLevelDebug,        ///< デバッグ
    
    kDebugLogLevelCount
};

/**
 * @enum DebugLogLevelMask
 */
enum DebugLogLevelMask {
    kDebugLogLevelMaskFatalError    = BIT32(kDebugLogLevelFatalError),
    kDebugLogLevelMaskError         = BIT32(kDebugLogLevelError),
    kDebugLogLevelMaskWarning       = BIT32(kDebugLogLevelWarning),
    kDebugLogLevelMaskInfo          = BIT32(kDebugLogLevelInfo),
    kDebugLogLevelMaskDebug         = BIT32(kDebugLogLevelDebug),
    
    kDebugLogLevelMaskAll           = 0xff
};

/**
 * @enum DebugLogOptions
 */
enum DebugLogOptions {
    kDebugLogOptProcessID           = BIT32(0), ///< ProcessIDを出力に含める
    kDebugLogOptThreadID            = BIT32(1), ///< ThreadIDを出力含める
    kDebugLogOptNoWait              = BIT32(2), ///< スレッドを使用せずにすぐに書き出す

    kDebugLogOptDefault             = kDebugLogOptProcessID | kDebugLogOptThreadID
};

END_NAMESPACE_FW


#endif  // FW_DEBUG_LOG_TYPE_H_
