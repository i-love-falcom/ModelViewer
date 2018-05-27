﻿/**
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