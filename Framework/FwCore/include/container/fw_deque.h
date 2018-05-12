﻿/**
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
    typedef deque<_Ty, _Attribute>                                      _My;
    typedef std::deque<_Ty, NAMESPACE_FW allocator<_Ty, _Attribute> >   _MyBase;
    typedef allocator<_Ty, _Attribute>                                  _Alloc;

    typedef _Alloc                                      allocator_type;
    typedef _MyBase::reference                          reference;
    typedef _MyBase::const_reference                    const_reference;
    typedef typename _MyBase::iterator                  iterator;
    typedef typename _MyBase::const_iterator            const_iterator;
    typedef typename _MyBase::size_type                 size_type;
    typedef typename _MyBase::difference_type           difference_type;
    typedef typename _MyBase::value_type                value_type;
    typedef typename _MyBase::pointer                   pointer;
    typedef typename _MyBase::reverse_iterator          reverse_iterator;
    typedef typename _MyBase::const_reverse_iterator    const_reverse_iterator;

    /**
     * @brief 登録されていない要素を追加
     */
    void insert_unique(const _Ty & value) {
        const_iterator itr = std::find(begin(), end(), value);
        if (itr == end()) {
            push_back(value);
        }
    }

    /**
     * @brief ソート状態を崩さずに要素を追加
     */
    void insert_sort(const _Ty & value) {
        const_iterator itr = std::lower_bound(begin(), end(), value);
        insert(itr, value);
    }

    /**
     * @brief ソート状態を崩さずに要素を追加
     */
    template<class _Compare>
    void insert_sort(const _Ty & value, _Compare comp) {
        const_iterator itr = std::lower_bound(begin(), end(), value, comp);
        insert(itr, value);
    }

    /**
     * @brief ソート状態を崩さずに登録されていない要素を追加
     */
    void insert_sort_unique(const _Ty & value) {
        const_iterator itr = std::lower_bound(begin(), end(), value);
        if (itr == end() || at(itr + 1) != value) {
            insert(itr, value);
        }
     }

    /**
     * @brief ソート状態を崩さずに登録されていない要素を追加
     */
    template<class _Compare>
    void insert_sort_unique(const _Ty & value, _Compare comp) {
        const_iterator itr = std::lower_bound(begin(), end(), value, comp);
        if (itr == end() || at(itr + 1) != value) {
            insert(itr, value);
        }
     }

    /**
     * @brief 要素の並びを変えずに削除
     */
    void remove(const _Ty & value) {
        const_iterator itr = std::find(begin(), end(), value);
        while (itr != end()) {
            erase(itr);
            itr = std::find(begin(), end(), value);
        }
    }
};

END_NAMESPACE_FW

#endif  // FW_DEQUE_H_