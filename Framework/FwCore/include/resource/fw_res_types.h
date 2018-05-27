/**
 * @file fw_resource_types.h
 */
#ifndef FW_RESOURCE_TYPES_H_
#define FW_RESOURCE_TYPES_H_

BEGIN_NAMESPACE_FW

//----------------------------------------------------
// FwRes
//----------------------------------------------------
//! リソース名の長さ
static const uint32_t MAX_RES_NAME_LEN = 255;

using FwResId = uint64_t;

/**
 * @enum FwResType
 */
enum class FwResType : uint32_t {
    kRawData,       ///< Raw data
    
};


//----------------------------------------------------
// FwResPackage
//----------------------------------------------------
//! リソースパッケージ名の長さ
static const uint32_t MAX_RES_PACKAGE_NAME_LEN = 255;

using FwResPackageId = uint64_t; ///< パッケージID

/**
 * @enum FwResPackageType
 */
enum class FwResPackageType : uint32_t {
    kRawData,   ///< Raw data
};

END_NAMESPACE_FW

#endif	// FW_RESOURCE_TYPES_H_
