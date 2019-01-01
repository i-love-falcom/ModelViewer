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
    enum class DebugLogLevel : uint32_t {
    kFatalError,   ///< 致命的なエラー
    kError,        ///< エラー
    kWarning,      ///< 警告
    kInfo,         ///< 情報
    kDebug,        ///< デバッグ
    
    kCount
};

FW_DEFINE_ENUM_BITFLAG(DebugLogLevel)

/**
 * @enum DebugLogLevelMask
 */
enum DebugLogLevelMask {
    kDebugLogLevelMaskFatalError    = FW_BIT32(DebugLogLevel::kFatalError),
    kDebugLogLevelMaskError         = FW_BIT32(DebugLogLevel::kError),
    kDebugLogLevelMaskWarning       = FW_BIT32(DebugLogLevel::kWarning),
    kDebugLogLevelMaskInfo          = FW_BIT32(DebugLogLevel::kInfo),
    kDebugLogLevelMaskDebug         = FW_BIT32(DebugLogLevel::kDebug),
    
    kDebugLogLevelMaskAll           = 0xff
};

/**
 * @enum DebugLogOptions
 */
enum DebugLogOptions {
    kDebugLogOptProcessID           = FW_BIT32(0), ///< ProcessIDを出力に含める
    kDebugLogOptThreadID            = FW_BIT32(1), ///< ThreadIDを出力含める
    kDebugLogOptNoWait              = FW_BIT32(2), ///< スレッドを使用せずにすぐに書き出す

    kDebugLogOptDefault             = kDebugLogOptProcessID | kDebugLogOptThreadID
};

END_NAMESPACE_FW


#endif  // FW_DEBUG_LOG_TYPE_H_
