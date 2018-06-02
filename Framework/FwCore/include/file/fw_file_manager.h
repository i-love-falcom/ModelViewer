/**
 * @file fw_file_manager.h
 * @author Kamai Masayoshi
 */
#ifndef FW_FILE_MANAGER_H_
#define FW_FILE_MANAGER_H_

#include "core/fw_thread.h"
#include "file/fw_file_types.h"
#include "file/fw_path.h"
#include "misc/fw_noncopyable.h"

BEGIN_NAMESPACE_FW

class FwFileStream;


/**
 * @struct FwFileManagerDesc
 */
struct FwFileManagerDesc {
    FwThreadAffinity    _threadAffinity;
    FwThreadPriority    _threadPriority;

    FW_INLINE void Init() {
        _threadAffinity = DefaultFwThreadAffinity;
        _threadPriority = FwThreadPriority::kTheadPriorityNormal;
    }
};

/**
 * @class FwFileManager
 */
class FwFileManager : public NonCopyable<FwFileManager> {
public:
    /**
     * @brief 終了処理
     */
    FW_INLINE void Shutdown() {
        DoShutdown();
    }

    /**
     * @brief ファイルを非同期で開く
     * @param[in] relativePath  基準パス位置からの相対パス
     * @param[in] fileName      ファイル名
     * @param[in] options       オプション
     * @param[in] priority      処理優先度
     * @return ファイルストリームオブジェクト
     */
    FW_INLINE FwFileStream * FileStreamOpen(const str_t relativePath, const str_t fileName, const sint32_t options, const uint32_t priority = kFilePriorityNormal) {
        char_t tmpPath[FwPath::kMaxPathLen + 1];
        return DoFileStreamOpen(FwPath::Combine(tmpPath, ARRAY_SIZEOF(tmpPath), relativePath, fileName), options, priority);
    }

    /**
     * @brief ファイルを開く
     * @param[in] fileName ファイル名
     * @param[in] options  オプション
     * @return ファイルストリームオブジェクト
     */
    FW_INLINE FwFileStream * FileStreamOpen(const str_t fileName, const sint32_t options) {
        return DoFileStreamOpen(fileName, options, kFilePriorityNormal);
    }

    /**
     * @brief 特別なパスをセット
     * @param[in] kind          パスの種類 
     * @param[in] absolutePath  パス位置（絶対パス）
     */
    FW_INLINE void SetSpecialPath(const FwPath::FwFilePath kind, const str_t absolutePath) {
        DoSetSpecialPath(kind, absolutePath);
    }

    /**
     * @brief 特別なパスを取得
     * @param[in] kind パスの種類 
     * @return パス位置
     */
    FW_INLINE const str_t GetSpecialPath(const FwPath::FwFilePath kind) {
        return DoGetSpecialPath(kind);
    }

    

protected:
    /**
     * @brief 終了処理
     */
    virtual void DoShutdown() = 0;

    /**
     * @brief ファイルを開く
     */
    virtual FwFileStream * DoFileStreamOpen(const str_t filePath, const sint32_t options, const uint32_t priority) = 0;

    /**
     * @brief 基準となるパスをセット
     */
    virtual void DoSetSpecialPath(const FwPath::FwFilePath kind, const str_t absolutePath) = 0;

    /**
     * @brief 基準となるパスを取得
     */
    virtual const str_t DoGetSpecialPath(const FwPath::FwFilePath kind) = 0;

    

    /**
     * @brief コンストラクタ
     */
    FwFileManager() {
    }

    /**
     * @brief デストラクタ
     */
    virtual ~FwFileManager() {
    }
};

/**
 * @brief FwFileManagerを生成
 * @param[in] desc 詳細
 */
FW_DLL_FUNC FwFileManager * CreateFileManager(FwFileManagerDesc * desc);

END_NAMESPACE_FW

#endif  // FW_FILE_MANAGER_H_
