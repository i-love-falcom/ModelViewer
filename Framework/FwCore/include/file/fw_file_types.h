/**
 * @file fw_file_types.h
 * @author Kamai Masayoshi
 */
#ifndef FW_FILE_TYPES_H_
#define FW_FILE_TYPES_H_

BEGIN_NAMESPACE_FW

/**
 * @enum FwFileOptionBits
 */
enum FwFileOptionBits : uint32_t {
    kFileOptAccessReadShiftBits,
    kFileOptAccessWriteShiftBites,

    kFileOptSharedReadShiftBits,
    kFileOptSharedWriteShiftBites,

    kFileOptAttributeEncryptedShiftBits,
    kFileOptAttributeHiddenShiftBits,
    kFileOptAttributeReadOnlyShiftBits,
    kFileOptAttributeTemporaryShiftBits,

    kFileOptFlagRandomAccessShiftBits,
    kFileOptFlagSequentialShiftBits,
    kFileOptFlagDeleteOnCloseShiftBits,

    kFileOptFlagNoCloseShiftBits,
};

/**
 * @enum FwFileOptions
 */
enum FwFileOptions : uint32_t {
    kFileOptAccessRead          = BIT32(kFileOptAccessReadShiftBits),           ///< 読み込みアクセス
    kFileOptAccessWrite         = BIT32(kFileOptAccessWriteShiftBites),         ///< 書き込みアクセス
    kFileOptAccessRW            = (kFileOptAccessRead | kFileOptAccessWrite),   ///< 読み書きマスク
    kFileOptAccessMask          = (kFileOptAccessRead | kFileOptAccessWrite),   ///< アクセス方法マスク

    kFileOptSharedRead          = BIT32(kFileOptSharedReadShiftBits),           ///< 他プロセスと読み込み共有
    kFileOptSharedWrite         = BIT32(kFileOptSharedWriteShiftBites),         ///< 他プロセスと書き込み共有
    kFileOptSharedRW            = (kFileOptSharedRead | kFileOptSharedWrite),   ///< 読み書き共有
    kFileOptSharedMask          = (kFileOptSharedRead | kFileOptSharedWrite),   ///< 共有方法マスク

    kFileOptAttributeEncrypted  = BIT32(kFileOptAttributeEncryptedShiftBits),   ///< 暗号化属性
    kFileOptAttributeHidden     = BIT32(kFileOptAttributeHiddenShiftBits),      ///< 隠しファイル属性
    kFileOptAttributeReadOnly   = BIT32(kFileOptAttributeReadOnlyShiftBits),    ///< 読み込み専用属性
    kFileOptAttributeTemporary  = BIT32(kFileOptAttributeTemporaryShiftBits),   ///< 一時ファイル
    kFileOptAttributeMask       = (kFileOptAttributeEncrypted | kFileOptAttributeHidden | kFileOptAttributeReadOnly | kFileOptAttributeTemporary),  ///< 属性マスク

    kFileOptFlagRandomAccess    = BIT32(kFileOptFlagRandomAccessShiftBits),     ///< ランダムアクセスに最適化
    kFileOptFlagSequential      = BIT32(kFileOptFlagSequentialShiftBits),       ///< シーケンシャルアクセスに最適化
    kFileOptFlagDeleteOnClose   = BIT32(kFileOptFlagDeleteOnCloseShiftBits),    ///< ファイルクローズ時に自動的に削除
    kFileOptFlagMask            = (kFileOptFlagRandomAccess | kFileOptFlagSequential | kFileOptFlagDeleteOnClose),    ///< フラグマスク

    kFileOptFlagNoClose         = BIT32(kFileOptFlagNoCloseShiftBits),          ///< ファイルクローズしない
};

/**
 * @enum FwSeekOrigin
 */
enum FwSeekOrigin : sint32_t {
    kSeekOriginBegin = 0,  ///< 先頭
    kSeekOriginCurrent,    ///< 現在位置
    kSeekOriginEnd         ///< 末尾
};

/**
 * @enum FwFilePriority
 */
enum FwFilePriority : sint32_t {
    kFilePriorityMin            = 256,
    kFilePriorityMax            = 1024,
    
    kFilePriorityLowest         = (kFilePriorityMax - kFilePriorityMin) * 6 / 6 + kFilePriorityMin, ///< 最低の優先度
    kFilePriorityLow            = (kFilePriorityMax - kFilePriorityMin) * 5 / 6 + kFilePriorityMin, ///< 低い優先度
    kFilePriorityBelowNormal    = (kFilePriorityMax - kFilePriorityMin) * 4 / 6 + kFilePriorityMin, ///< 基準より一段低い優先度
    kFilePriorityNormal         = (kFilePriorityMax - kFilePriorityMin) * 3 / 6 + kFilePriorityMin, ///< 基準となる優先度
    kFilePriorityAboveNormal    = (kFilePriorityMax - kFilePriorityMin) * 2 / 6 + kFilePriorityMin, ///< 基準より一段高い優先度
    kFilePriorityHigh           = (kFilePriorityMax - kFilePriorityMin) * 1 / 6 + kFilePriorityMin, ///< 高いの優先度
    kFilePriorityHighest        = (kFilePriorityMax - kFilePriorityMin) * 0 / 6 + kFilePriorityMin, ///< 最高の優先度
};

END_NAMESPACE_FW

#endif  // FW_FILE_TYPES_H_
