/**
* @file fw_res_package.h
*/
#ifndef FW_RES_PACKAGE_H_
#define FW_RES_PACKAGE_H_

#include "resource/fw_res_types.h"

BEGIN_NAMESPACE_FW

class FwResManager;


/**
 * @class FwResPackage
 */
class FwResPackage {
public:
    /**
     * @brief 破棄
     */
    FW_INLINE virtual void Destroy() {
        //! @todo 破棄処理をFwResManagerに委任する
    };

    /**
     * @brief 名前を取得
     */
    FW_INLINE const str_t GetName() const { return const_cast<const str_t>(_name); }

    /**
     * @brief パッケージIDを取得
     */
    FW_INLINE FwResPackageId GetId() const { return _id; }

    /**
     * @brief パッケージタイプ
     */
    FW_INLINE FwResPackageType GetType() const { return _type; }

 
protected:
    char_t              _name[MAX_RES_PACKAGE_NAME_LEN + 1];
    FwResPackageId      _id;
    FwResPackageType    _type;

private:
    FwResManager *  _manager;
};

/**
 * @class FwResPackageHandle
 */
class FwResPackageHandle {
public:


private:
    friend FwResManager;
};

END_NAMESPACE_FW

#endif	// FW_RES_PACKAGE_H_
