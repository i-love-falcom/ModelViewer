/**
 * @file fw_file_manager.h
 * @author Kamai Masayoshi
 */
#ifndef FW_FILE_MANAGER_H_
#define FW_FILE_MANAGER_H_

#include "threading/fw_thread.h"
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
        _threadPriority = FwThreadPriorityNormal;
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
    FW_INLINE FwFileStream * FileStreamOpen(const str_t relativePath, const str_t fileName, const sint32_t options, const FwFilePriority priority = FwFilePriorityNormal) {
        char_t tmpPath[FwPath::kMaxPathLen + 1];
        return DoFileStreamOpen(FwPath::Combine(tmpPath, FW_ARRAY_SIZEOF(tmpPath), GetBasePath(), relativePath, fileName), options, priority);
    }

    /**
     * @brief ファイルを開く
     * @param[in] fileName ファイル名
     * @param[in] options  オプション
     * @return ファイルストリームオブジェクト
     */
    FW_INLINE FwFileStream * FileStreamOpen(const str_t fileName, const sint32_t options) {
        char_t tmpPath[FwPath::kMaxPathLen + 1];
        return DoFileStreamOpen(FwPath::Combine(tmpPath, FW_ARRAY_SIZEOF(tmpPath), GetBasePath(), fileName), options, FwFilePriorityNormal);
    }

    /**
     * @brief 基準となるパスをセット
     * @param[in] path 基準パス位置
     */
    FW_INLINE void SetBasePath(const str_t path) {
        DoSetBasePath(path);
    }

    /**
     * @brief 基準となるパスを取得
     * @return 基準パス位置
     */
    FW_INLINE const str_t GetBasePath() {
        return DoGetBasePath();
    }


protected:
    /**
     * @brief 終了処理
     */
    virtual void DoShutdown() = 0;

    /**
     * @brief ファイルを開く
     */
    virtual FwFileStream * DoFileStreamOpen(const str_t filePath, const sint32_t options, const FwFilePriority priority) = 0;

    /**
     * @brief 基準となるパスをセット
     */
    virtual void DoSetBasePath(const str_t path) = 0;

    /**
     * @brief 基準となるパスを取得
     */
    virtual const str_t DoGetBasePath() = 0;


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
