/**
 * @file fw_resource_manager.h
 */

#include "misc/fw_noncopyable.h"

BEGIN_NAMESPACE_FW

class FwFileManager;


/**
 * @struct FwResManagerDesc
 */
struct FwResManagerDesc {
    FwFileManager*  _fileManager;

    FW_INLINE void Init() {
        _fileManager = nullptr;
    }
};

/**
 * @class FwResManager
 */
class FwResManager : public NonCopyable<FwResManager> {
public:
    //! @todo リソースデータベース作成
    //! @todo リソース登録、解除作成

    /**
     * @brief 終了処理
     */
    FW_INLINE void Shutdown() {
        DoShutdown();
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
     * @brief 基準となるパスをセット
     */
    virtual void DoSetBasePath(const str_t path) = 0;

    /**
     * @brief 基準となるパスを取得
     */
    virtual const str_t DoGetBasePath() = 0;
};

/**
 * @brief FileSystemを生成
 * @param[in] desc 詳細
 */
FW_DLL_FUNC FwResManager * CreateResManager(FwResManagerDesc * desc);

END_NAMESPACE_FW
