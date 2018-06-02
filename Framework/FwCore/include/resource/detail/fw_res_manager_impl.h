/**
 * @file fw_res_manager_impl.h
 */
#include "resource/fw_res_manager.h"

BEGIN_NAMESPACE_FW

class FwFileManager;
class FwResPackage;

/**
 * @class FwResManagerImpl
 */
class FwResManagerImpl : public FwResManager {
public:
    /**
     * @internal
     * @brief 初期化処理
     */
    void Init(FwResManagerDesc * desc);

    /**
     * @internal
     * @brief 終了処理
     */
    virtual void DoShutdown() FW_OVERRIDE;

    /**
     * @brief パッケージを生成
     * @param[in] desc      パッケージの記述
     * @param[in] name      パッケージ名
     * @param[out] package  生成したパッケージ
     */
    virtual sint32_t DoCreatePackage(const FwResPackageDesc & desc, const str_t name, const FwResPackage ** package) FW_OVERRIDE;

    /**
     * @internal
     * @brief パッケージをロードする
     * @param[in] relativePath  相対パス
     * @param[in] name          パッケージ名
     * @param[out] handle       パッケージハンドル
     * @return エラーコード
     */
    virtual sint32_t DoLoadPackage(const str_t relativePath, const str_t name, FwResPackageOnComplete onComplete, FwResPackageOnError onError) FW_OVERRIDE;

    /**
     * @internal
     * @brief パッケージをアンロードする
     * @param[in] handle    パッケージハンドル
     * @return エラーコード
     */
    virtual sint32_t DoUnloadPackage(FwResPackage * package) FW_OVERRIDE;

    /**
     * @brief 論理パスを物理パスへ変換する
     * @param[out] physicalPath  物理パス格納先バッファ
     * @param[in]  numOfElements 物理パス格納先バッファのエレメント数
     * @param[in] logicalPath    論理パス
     */
    virtual sint32_t DoConvertLogicalToPhysical(str_t physicalPath, const size_t numOfElements, const str_t logicalPath) FW_OVERRIDE;

    /**
     * @brief
     */
    FwResManagerImpl();

    /**
     * @brief デストラクタ
     */
    ~FwResManagerImpl();


    //! @todo リソースデータベース作成

private:
    /**
     * @brief ローカルファイルからパッケージをロードする
     */
    sint32_t LoadPackageFromFile(const str_t relativePath, const str_t name, FwResPackageHandle * handle);





    FwFileManager * _fileManager;
    //! @todo Webから取得できるように
};


END_NAMESPACE_FW
