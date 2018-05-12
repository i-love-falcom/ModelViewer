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

class FileStream;


/**
 * @struct FileManagerDesc
 */
struct FileManagerDesc {
    FwThreadAffinity    threadAffinity;
    FwThreadPriority    threadPriority;

    FW_INLINE void Init() {
        threadAffinity = DefaultFwThreadAffinity;
        threadPriority = kTheadPriorityNormal;
    }
};

/**
 * @class FileManager
 */
class FileManager : public NonCopyable<FileManager> {
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
    FW_INLINE FileStream * FileStreamOpen(const str_t relativePath, const str_t fileName, const sint32_t options, const uint32_t priority = kFilePriorityNormal) {
        char_t tmpPath[Path::kMaxPathLen + 1];
        return DoFileStreamOpen(Path::Combine(tmpPath, ARRAY_SIZEOF(tmpPath), GetBasePath(), relativePath, fileName), options, priority);
    }

    /**
     * @brief ファイルを開く
     * @param[in] fileName ファイル名
     * @param[in] options  オプション
     * @return ファイルストリームオブジェクト
     */
    FW_INLINE FileStream * FileStreamOpen(const str_t fileName, const sint32_t options) {
        char_t tmpPath[Path::kMaxPathLen + 1];
        return DoFileStreamOpen(Path::Combine(tmpPath, ARRAY_SIZEOF(tmpPath), GetBasePath(), fileName), options, kFilePriorityNormal);
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
    virtual FileStream * DoFileStreamOpen(const str_t filePath, const sint32_t options, const uint32_t priority) = 0;

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
    FileManager() {
    }

    /**
     * @brief デストラクタ
     */
    virtual ~FileManager() {
    }
};

/**
 * @brief FileSystemを生成
 * @param[in]   desc      詳細
 * @param[in]   allocator 内部使用するアロケータを指定。nullptrならDefaultAllocatorを使用
 */
FW_DLL_FUNC FileManager * CreateFileManager(FileManagerDesc * desc);

END_NAMESPACE_FW

#endif  // FW_FILE_MANAGER_H_
