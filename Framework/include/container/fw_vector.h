/**
 * @file fw_vector.h
 * @author Kamai Masayoshi
 */
#ifndef FW_VECTOR_H_
#define FW_VECTOR_H_

#include <vector>
#include <algorithm>

#include "container/fw_container_allocator.h"

BEGIN_NAMESPACE_FW

template<class _Ty, uint64_t _Attribute = DefaultMallocAttribute>
class vector : public std::vector<_Ty, NAMESPACE_FW allocator<_Ty, _Attribute> > {
public:
    /**
     * @brief 登録されていない要素を追加
     * @param[in] value 登録する要素
     * @retval true     正常に登録された
     * @retval false    既に同じものが登録されていた
     */
    bool insert_unique(const _Ty & value) {
        vector<_Ty, _Attribute>::iterator itr = std::find(begin(), end(), value);
        if (itr == end()) {
            push_back(value);
            return true;
        }
        return false;
    }

    /**
     * @brief ソート状態を崩さずに要素を追加
     * @param[in] value 登録する要素
     */
    void insert_sort(const _Ty & value) {
        vector<_Ty, _Attribute>::iterator itr = std::lower_bound(begin(), end(), value);
        insert(itr, value);
    }

    /**
     * @brief ソート状態を崩さずに登録されていない要素を追加
     * @param[in] value 登録する要素
     * @retval true     正常に登録された
     * @retval false    既に同じものが登録されていた
     */
    bool insert_sort_unique(const _Ty & value) {
        vector<_Ty, _Attribute>::iterator itr = std::lower_bound(begin(), end(), value);
        if (itr == end() || at(itr + 1) != value) {
            insert(itr, value);
            return true;
        }
        return false;
     }

    /**
     * @brief 要素の並びを変えずに削除
     * @param[in] value 削除する要素
     */
    void remove(const _Ty & value) {
        vector<_Ty, _Attribute>::iterator itr = std::find(begin(), end(), value);
        while (itr != end()) {
            erase(itr);
            itr = std::find(begin(), end(), value);
        }
    }

    /**
     * @brief 要素の並びを保障せず削除
     * @param[in] value 削除する要素
     */
    void remove_fast(const _Ty & value) {
        vector<_Ty, _Attribute>::iterator itr = std::find(begin(), end(), value);
        while (itr != end()) {
            at(itr - begin()) = at(size() - 1);
            pop_back();
            itr = std::find(begin(), end(), value);
        }
    }
};

END_NAMESPACE_FW

#endif  // FW_VECTOR_H_
