/**
 * @file fw_deque.h
 * @author Kamai Masayoshi
 */
#ifndef FW_DEQUE_H_
#define FW_DEQUE_H_

#include <deque>
#include <algorithm>

#include "container/fw_container_allocator.h"

BEGIN_NAMESPACE_FW

template<class _Ty, uint64_t _Attribute = defaultFwMallocAttribute >
class deque : public std::deque<_Ty, NAMESPACE_FW allocator<_Ty, _Attribute> > {
public:
    /**
     * @brief 登録されていない要素を追加
     */
    void insert_unique(const _Ty & value) {
        deque<_Ty>::iterator itr = std::find(begin(), end(), value);
        if (itr == end()) {
            push_back(value);
        }
    }

    /**
     * @brief ソート状態を崩さずに要素を追加
     */
    void insert_sort(const _Ty & value) {
        deque<_Ty>::iterator itr = std::lower_bound(begin(), end(), value);
        insert(itr, value);
    }

    /**
     * @brief ソート状態を崩さずに登録されていない要素を追加
     */
    void insert_sort_unique(const _Ty & value) {
        deque<_Ty>::iterator itr = std::lower_bound(begin(), end(), value);
        if (itr == end() || at(itr + 1) != value) {
            insert(itr, value);
        }
     }

    /**
     * @brief 要素の並びを変えずに削除
     */
    void remove(const _Ty & value) {
        deque<_Ty>::iterator itr = std::find(begin(), end(), value);
        while (itr != end()) {
            erase(itr);
            itr = std::find(begin(), end(), value);
        }
    }
};

END_NAMESPACE_FW

#endif  // FW_DEQUE_H_
