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

template<class _Ty, uint64_t _Attribute = defaultFwMallocAttribute>
class vector : public std::vector<_Ty, NAMESPACE_FW allocator<_Ty, _Attribute> > {
public:
    typedef vector<_Ty, _Attribute>                                     _My;
    typedef std::vector<_Ty, NAMESPACE_FW allocator<_Ty, _Attribute> >  _MyBase;
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
     * @brief 末尾に要素を追加してその参照を取得
     * @return 末尾要素への参照
     */
    reference append() {
        resize(size() + 1);
        return back();
    }

    /**
     * @brief 末尾に要素を追加してその参照を取得
     * @return 末尾要素への参照
     */
    const_reference append() const {
        resize(size() + 1);
        return back();
    }

    /**
     * @brief 登録されていない要素を追加
     * @param[in] value 登録する要素
     * @retval true     正常に登録された
     * @retval false    既に同じものが登録されていた
     */
    bool insert_unique(const _Ty & value) {
        const_iterator itr = std::find(begin(), end(), value);
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
     * @param[in] value 登録する要素
     * @retval true     正常に登録された
     * @retval false    既に同じものが登録されていた
     */
    bool insert_sort_unique(const _Ty & value) {
        const_iterator itr = std::lower_bound(begin(), end(), value);
        if (itr == end() || at(itr + 1) != value) {
            insert(itr, value);
            return true;
        }
        return false;
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
     * @param[in] value 削除する要素
     */
    void remove(const _Ty & value) {
        const_iterator itr = std::find(begin(), end(), value);
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
        const_iterator itr = std::find(begin(), end(), value);
        while (itr != end()) {
            at(itr - begin()) = at(size() - 1);
            pop_back();
            itr = std::find(begin(), end(), value);
        }
    }
};

END_NAMESPACE_FW

#endif  // FW_VECTOR_H_
