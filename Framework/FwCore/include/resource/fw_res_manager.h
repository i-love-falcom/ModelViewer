/**
 * @file fw_res_manager.h
 */
#include "core/fw_thread.h"
#include "misc/fw_noncopyable.h"
#include "resource/fw_res_package.h"

BEGIN_NAMESPACE_FW

class FwFileManager;
class FwResPackage;

using FwResPackageOnComplete    = std::function<void(const str_t, const str_t, FwResPackage *)>;
using FwResPackageOnError       = std::function<void(const str_t, const str_t, sint32_t)>;


/**
 * @struct FwResManagerDesc
 */
struct FwResManagerDesc {
    FwThreadAffinity    _threadAffinity;    ///< スレッドアフィニティ
    FwThreadPriority    _threadPriority;    ///< スレッド優先度
    FwFileManager*      _fileManager;       ///< ファイルマネージャ

    //! @todo Webから取得できるように


    FW_INLINE void Init() {
        _threadAffinity = DefaultFwThreadAffinity;
        _threadPriority = FwThreadPriority::kTheadPriorityNormal;
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
     * @param[in] onComplate    ロード完了時に呼び出されるコールバック
     * @param[in] onError       エラー発生時に呼び出されるコールバック
     * @return エラーコード
     */
    FW_INLINE sint32_t LoadPackage(const str_t relativePath, const str_t name, FwResPackageOnComplete onComplete, FwResPackageOnError onError) {
        return DoLoadPackage(relativePath, name, onComplete, onError);
    }
    
    /**
     * @brief パッケージをアンロードする
     * @param[in] handle    パッケージハンドル
     * @return エラーコード
     */
    FW_INLINE sint32_t UnloadPackage(FwResPackage * package) {
        return DoUnloadPackage(package);
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
     * @param[in] onComplate    ロード完了時に呼び出されるコールバック
     * @param[in] onError       エラー発生時に呼び出されるコールバック
     * @return エラーコード
     */
    virtual sint32_t DoLoadPackage(const str_t relativePath, const str_t name, FwResPackageOnComplete onComplete, FwResPackageOnError onError) = 0;

    /**
     * @brief パッケージをアンロードする
     * @param[in] handle    パッケージハンドル
     * @return エラーコード
     */
    virtual sint32_t DoUnloadPackage(FwResPackage * package) = 0;
};

/**
 * @brief FileSystemを生成
 * @param[in] desc 詳細
 */
FW_DLL_FUNC FwResManager * CreateResManager(FwResManagerDesc * desc);

END_NAMESPACE_FW
