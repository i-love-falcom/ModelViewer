/**
 * @file fw_resource.h
 */
#ifndef FW_RESOURCE_H_
#define FW_RESOURCE_H_

#include "resource/fw_resource_types.h"

BEGIN_NAMESPACE_FW

//! @todo 後でちゃんと定義する
using FwResId = uint64_t;
using FwResType = uint32_t;

/**
 * @class FwRes
 */
class FwRes {
public:
    enum {
        kMaxNameLen = 31,
    };

    /**
     * @brief 破棄
     */
    FW_INLINE virtual void Destroy() {
        //! @todo 破棄処理をFwResManagerに委任する
    };

    /**
     * @brief 名前を取得
     */
    FW_INLINE const str_t GetName() const {return const_cast<const str_t>(_name);}

    /**
     * @brief リソースIDを取得
     */
    FW_INLINE FwResId GetId() const {return _id;}

    /**
     * @brief リソースタイプ
     */
    FW_INLINE FwResType GetType() const {return _type;}

    /**
     * @brief 参照カウントを増加
     */
    FW_INLINE uint32_t AddRef(std::memory_order order = std::memory_order_seq_cst) {
        return _referenceCount.fetch_add(1, order) + 1;
    }

    /**
     * @brief 参照カウントを減少
     */
    FW_INLINE uint32_t Release(std::memory_order order = std::memory_order_seq_cst) {
        const uint32_t newValue = _referenceCount.fetch_sub(1, order) - 1;
        if (newValue == 0) {
            Destroy();
        }
    }
    
protected:
    uint32_t                _delayReleaseCount; //! 遅延解放カウント
    std::atomic_uint32_t    _referenceCount;    //! 参照カウント

    char_t      _name[kMaxNameLen];
    FwResId     _id;
    FwResType   _type;

};

/**
 * @class FwResPtr
 * @tparam T 保持するリソースの型
 */
template<typename T>
class FwResPtr {
public:
    /**
     * @brief リソースのポインタを取得
     */
    FW_INLINE T* Ptr() {return this->_ptr;}
    
    /**
     * @brief リソースのポインタを取得
     */
    FW_INLINE const T* Ptr() const {return this->_ptr;}
    
    /**
     * @brief 参照カウントを上げずにリソースを保持する
     */
    FW_INLINE void Attach(T* ptr) {
        this->Release();
        this->_ptr = ptr;
    }

    /**
     * @brief 代入
     */
    FW_INLINE const FwResPtr<T>& operator =(const FwResPtr<T>& value) {
        if (Ptr() == value.Ptr()) {
            return *this;
        }
        value.AddRef();
        this->Attach(value.Ptr());

        return *this;
    }

    /**
     * @brief ムーブ
     */
    FW_INLINE const FwResPtr<T>& operator =(FwResPtr<T>&& value) {
        if (Ptr() == value.Ptr()) {
            return *this;
        }
        value.AddRef();
        this->Attach(value.Ptr());
        value.Attach(nullptr);

        return *this;
    }
    
    /**
     * @name 比較演算子
     */
    //! @{
    FW_INLINE bool operator ==(const FwResPtr<T>& value) {
        return this->Ptr() == value.Ptr();
    }
    FW_INLINE bool operator !=(const FwResPtr<T>& value) {
        return this->Ptr() != value.Ptr();
    }
    //! @}

    /**
     * @name コンストラクタ
     */
    //! @{
    FW_INLINE FwResPtr()
    : FwResPtr(nullptr) {
    }
    FW_INLINE FwResPtr(T* ptr) {
        Attach(ptr);
    }
    FW_INLINE FwResPtr(FwResPtr<T>& value) {
        value.AddRef();
        this->Attach(value.Ptr());
    }
    FW_INLINE FwResPtr(FwResPtr<T>&& value) {
        value.AddRef();
        this->Attach(value.Ptr());
        value.Attach(nullptr);
    }
    //! @}


private:
    /**
     * @brief 参照カウントを増やす
     */
    FW_INLINE uint32_t AddRef() {
        if (_ptr != nullptr) {
            return _ptr->AddRef();
        }
        return 0;
    }

    /**
     * @brief 参照カウントを減らす
     */
    FW_INLINE uint32_t Release() {
        if (_ptr != nullptr) {
            return _ptr->Release();
        }
        return 0;
    }


    T*  _ptr;
};

template<typename T>
static FW_INLINE bool operator ==(const FwResPtr<T>& a, T* b) {
    return a.Ptr() == b;
}

template<typename T>
static FW_INLINE bool operator !=(const FwResPtr<T>& a, T* b) {
    return a.Ptr() != b;
}

template<typename T>
static FW_INLINE bool operator ==(T* a, const FwResPtr<T>& b) {
    return a == b.Ptr();
}

template<typename T>
static FW_INLINE bool operator !=(T* a, const FwResPtr<T>& b) {
    return a != b.Ptr();
}

END_NAMESPACE_FW

#endif	// FW_RESOURCE_H_
