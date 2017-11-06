/**
 * @file fw_error.h
 * @author Kamai Masayoshi
 */
#ifndef FW_ERROR_H_
#define FW_ERROR_H_

#define FW_OK                       (0x00000000)    ///< 正常に成功
#define FW_FAILED                   (0x00000001)    ///< 正常に失敗

#define ERR_INVALID                 (0x80000010)    ///< 無効
#define ERR_INVALID_FUNCTION        (0x80000011)    ///< 無効な関数呼び出し
#define ERR_INVALID_PARMS           (0x80000012)    ///< 無効なパラメータ
#define ERR_INVALID_DEVICE          (0x80000013)    ///< 無効なデバイス

#define ERR_FAILED                  (0x80000020)    ///< 失敗
#define ERR_UNKNOWN                 (0x80000021)    ///< 不明
#define ERR_NOSUPPORT               (0x80000022)    ///< 未サポート
#define ERR_TIMEOUT                 (0x80000024)    ///< タイムアウト
#define ERR_OVERFLOW                (0x80000025)    ///< 最大値を超えた
#define ERR_FILE_EXIST              (0x80000026)    ///< ファイルが既に存在する
#define ERR_FILE_NOEXIST            (0x80000027)    ///< ファイルが存在しない
#define ERR_EOF                     (0x80000028)    ///< ファイル終端に到達
#define ERR_BUSY                    (0x80000029)    ///< 処理中
#define ERR_UNINITIALIZED           (0x8000002a)    ///< 未初期化
#define ERR_ALREADY_INITIALIZED     (0x8000002b)    ///< 既に初期化済み
#define ERR_OUT_OF_MEMORY           (0x8000002c)    ///< メモリが足りない
#define ERR_NOT_ENOUGH_BUFFERS      (0x8000002d)    ///< バッファが足りません
#define ERR_DEVICE_NOT_SUPPORTED    (0x8000002e)    ///< デバイスがサポートされていない
#define ERR_DEVICE_NOT_FOUND        (0x8000002f)    ///< デバイスが見つからない
#define ERR_DEVICE_NOT_ACQUIRED     (0x80000030)    ///< デバイスを獲得できなかった
#define ERR_DEVICE_LOST             (0x80000031)    ///< デバイスを見失った
#define ERR_DEVICE_BUSY             (0x80000032)    ///< デバイスが稼働中でアクセスできない
#define ERR_PENDING                 (0x80000033)    ///< まだ使用できない


#endif  // FW_ERROR_H_
