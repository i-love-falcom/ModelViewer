/**
 * @file fw_file_manager.h
 * @author Kamai Masayoshi
 */
#ifndef FW_FILE_MANAGER_H_
#define FW_FILE_MANAGER_H_

#include "file/fw_file_types.h"
#include "file/fw_path.h"
#include "misc/fw_noncopyable.h"

BEGIN_NAMESPACE_FW

class FileReadStream;
class FileWriteStream;

/**
 * @struct FileManagerDesc
 */
struct FileManagerDesc {
    sint32_t streamPoolSize;
    uint32_t asyncThreadAffinity;
    uint32_t asyncThreadPriority_t;
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
     */
    FW_INLINE FileReadStream * FileReadOpen(const str_t relativePath, const str_t fileName, const sint32_t options, const uint32_t priority = kFilePriorityNormal) {
        char_t tmpPath[Path::kMaxPathLen + 1];
        return DoFileReadOpen(Path::Combine(tmpPath, ARRAY_SIZEOF(tmpPath), GetBasePath(), relativePath, fileName), options, priority);
    }

    /**
     * @brief ファイルを非同期で開く
     */
    FW_INLINE FileWriteStream * FileWriteOpen(const str_t relativePath, const str_t fileName, const sint32_t options, const uint32_t priority = kFilePriorityNormal) {
        char_t tmpPath[Path::kMaxPathLen + 1];
        return DoFileWriteOpen(Path::Combine(tmpPath, ARRAY_SIZEOF(tmpPath), GetBasePath(), relativePath, fileName), options, priority);
    }

    /**
     * @brief ファイルを開く
     */
    FW_INLINE FileReadStream * FileReadOpen(const str_t fileName, const sint32_t options) {
        return FileReadOpen(nullptr, fileName, options, kFilePriorityNormal);
    }

    /**
     * @brief ファイルを開く
     */
    FW_INLINE FileWriteStream * FileWriteOpen(const str_t fileName, const sint32_t options) {
        return FileWriteOpen(nullptr, fileName, options, kFilePriorityNormal);
    }

    /**
     * @brief 基準となるパスをセット
     */
    FW_INLINE void SetBasePath(const str_t path) {
        DoSetBasePath(path);
    }

    /**
     * @brief 基準となるパスを取得
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
    virtual FileReadStream * DoFileReadOpen(const str_t filePath, const sint32_t options, const uint32_t priority) = 0;

    /**
     * @brief ファイルを開く
     */
    virtual FileWriteStream * DoFileWriteOpen(const str_t filePath, const sint32_t options, const uint32_t priority) = 0;

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
