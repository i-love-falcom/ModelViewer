/**
 * @file fw_resource_manager.cpp
 */
#include "precompiled.h"
#include "resource/fw_resource_manager.h"

BEGIN_NAMESPACE_FW
BEGIN_NAMESPACE_NONAME

/**
 * @class FwResManagerImpl
 */
class FwResManagerImpl : public FwResManager {
public:
    // 初期化処理
    void Init(FwResManagerDesc * desc) {

    }

    // 終了処理
    virtual void DoShutdown() FW_OVERRIDE {
        
    }

    // 基準となるパスをセット
    virtual void DoSetBasePath(const str_t path) FW_OVERRIDE {

    }

    // 基準となるパスを取得
    virtual const str_t DoGetBasePath() FW_OVERRIDE {
        return nullptr;
    }

    FwResManagerImpl() {

    }

    ~FwResManagerImpl() {

    }


private:
};

END_NAMESPACE_NONAME


// 生成
FwResManager * CreateResManager(FwResManagerDesc * desc) {
    FwResManagerImpl * manager = FwNew<FwResManagerImpl>();
    manager->Init(desc);

    return manager;
}
END_NAMESPACE_FW
