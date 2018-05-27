/**
 * @file fw_res_manager.h
 */
#include "misc/fw_noncopyable.h"
#include "resource/fw_res_package.h"

BEGIN_NAMESPACE_FW

class FwFileManager;


/**
 * @struct FwResManagerDesc
 */
struct FwResManagerDesc {
    FwFileManager*  _fileManager;   ///< ファイルマネージャ

    FW_INLINE void Init() {
        _fileManager = nullptr;
    }
};

/**
 * @class FwResManager
 */
class FwResManager : public NonCopyable<FwResManager> {
public:
    /**
     * @brief 終了処理
     */
    FW_INLINE void Shutdown() {
        DoShutdown();
    }

    /**
     * @brief パッケージをロードする
     * @param[in] relativePath  相対パス
     * @param[in] name          パッケージ名
     * @param[out] handle       パッケージハンドル
     * @return エラーコード
     */
    FW_INLINE sint32_t LoadPackage(const str_t relativePath, const str_t name, FwResPackageHandle * handle) {
        return DoLoadPackage(relativePath, name, handle);
    }
    
    /**
     * @brief パッケージをアンロードする
     * @param[in] handle    パッケージハンドル
     * @return エラーコード
     */
    FW_INLINE sint32_t UnloadPackage(FwResPackageHandle & handle) {
        return DoUnloadPackage(handle);
    }

    
    //! @todo リソース登録、解除作成

protected:
    /**
     * @brief 終了処理
     */
    virtual void DoShutdown() = 0;

    /**
     * @brief パッケージをロードする
     * @param[in] relativePath  相対パス
     * @param[in] name          パッケージ名
     * @param[out] handle       パッケージハンドル
     * @return エラーコード
     */
    virtual sint32_t DoLoadPackage(const str_t relativePath, const str_t name, FwResPackageHandle * handle) = 0;

    /**
     * @brief パッケージをアンロードする
     * @param[in] handle    パッケージハンドル
     * @return エラーコード
     */
    virtual sint32_t DoUnloadPackage(FwResPackageHandle & handle) = 0;
};

/**
 * @brief FileSystemを生成
 * @param[in] desc 詳細
 */
FW_DLL_FUNC FwResManager * CreateResManager(FwResManagerDesc * desc);

END_NAMESPACE_FW
