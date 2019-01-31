/**
 * @file fw_file_types.h
 * @author Kamai Masayoshi
 */
#ifndef FW_FILE_TYPES_H_
#define FW_FILE_TYPES_H_

BEGIN_NAMESPACE_FW

using FwFileOpt         = uint32_t;
using FwSeekOrigin      = uint8_t;
using FwFilePriority    = uint32_t;

//------------------------------------------------------------------------------------------------------
// ファイル属性
static const FwFileOpt FwFileOptAccessRead           = 0x00000001;   ///< 読み込みアクセス
static const FwFileOpt FwFileOptAccessWrite          = 0x00000002;   ///< 書き込みアクセス
static const FwFileOpt FwFileOptAccessRW             = 0x00000003;   ///< 読み書きアクセス
static const FwFileOpt FwFileOptAccessMask           = 0x00000003;   ///< 読み書きマスク

static const FwFileOpt FwFileOptSharedRead           = 0x00000004;   ///< 他プロセスと読み込み共有
static const FwFileOpt FwFileOptSharedWrite          = 0x00000008;   ///< 他プロセスと書き込み共有
static const FwFileOpt FwFileOptSharedRW             = 0x0000000c;   ///< 他プロセスとの読み書き共有
static const FwFileOpt FwFileOptSharedMask           = 0x0000000c;   ///< 読み書き共有マスク

static const FwFileOpt FwFileOptAttributeEncrypted   = 0x00000010;   ///< 暗号化属性
static const FwFileOpt FwFileOptAttributeHidden      = 0x00000020;   ///< 隠しファイル属性
static const FwFileOpt FwFileOptAttributeReadOnly    = 0x00000040;   ///< 読み込み専用属性
static const FwFileOpt FwFileOptAttributeTemporary   = 0x00000080;   ///< 一時ファイル
static const FwFileOpt FwFileOptAttributeMask        = 0x000000f0;   ///< 属性マスク

static const FwFileOpt FwFileOptFlagRandomAccess     = 0x00000100;   ///< ランダムアクセスに最適化
static const FwFileOpt FwFileOptFlagSequential       = 0x00000200;   ///< シーケンシャルアクセスに最適化
static const FwFileOpt FwFileOptFlagDeleteOnClose    = 0x00000400;   ///< ファイルクローズ時に自動的に削除
static const FwFileOpt FwFileOptFlagNoClose          = 0x00000800;   ///< ファイルクローズしない
static const FwFileOpt FwFileOptFlagMask             = 0x00000f00;   ///< フラグマスク
//------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------
// ファイルシーク
static const FwSeekOrigin FwSeekOriginBegin     = 0;    ///< 先頭
static const FwSeekOrigin FwSeekOriginCurrent   = 1;    ///< 現在位置
static const FwSeekOrigin FwSeekOriginEnd       = 2;    ///< 末尾
//------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------
// ファイル優先度
static const FwFilePriority FwFilePriorityMin             = 256;
static const FwFilePriority FwFilePriorityMax             = 1024;
static const FwFilePriority FwFilePriorityLowest         = (FwFilePriorityMax - FwFilePriorityMin) * 6 / 6 + FwFilePriorityMin;   ///< 最低の優先度
static const FwFilePriority FwFilePriorityLow            = (FwFilePriorityMax - FwFilePriorityMin) * 5 / 6 + FwFilePriorityMin;   ///< 低い優先度
static const FwFilePriority FwFilePriorityBelowNormal    = (FwFilePriorityMax - FwFilePriorityMin) * 4 / 6 + FwFilePriorityMin;   ///< 基準より一段低い優先度
static const FwFilePriority FwFilePriorityNormal         = (FwFilePriorityMax - FwFilePriorityMin) * 3 / 6 + FwFilePriorityMin;   ///< 基準となる優先度
static const FwFilePriority FwFilePriorityAboveNormal    = (FwFilePriorityMax - FwFilePriorityMin) * 2 / 6 + FwFilePriorityMin;   ///< 基準より一段高い優先度
static const FwFilePriority FwFilePriorityHigh           = (FwFilePriorityMax - FwFilePriorityMin) * 1 / 6 + FwFilePriorityMin;   ///< 高いの優先度
static const FwFilePriority FwFilePriorityHighest        = (FwFilePriorityMax - FwFilePriorityMin) * 0 / 6 + FwFilePriorityMin;   ///< 最高の優先度
//------------------------------------------------------------------------------------------------------


END_NAMESPACE_FW

#endif  // FW_FILE_TYPES_H_
