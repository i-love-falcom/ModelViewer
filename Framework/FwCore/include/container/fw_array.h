/**
 * @file fw_array.h
 * @author Kamai Masayoshi
 */
#ifndef FW_ARRAY_H_
#define FW_ARRAY_H_

#include "debug/fw_assert.h"


BEGIN_NAMESPACE_FW

template<typename _Ty, size_t _Num>
class array {
public:
    typedef array<_Ty, _Num>        _My;
    typedef _Ty                     value_type;
    typedef size_t                  size_type;
    typedef ptrdiff_t               defference_type;
    typedef value_type *            pointer;
    typedef const value_type *      const_pointer;
    typedef value_type &            reference;
    typedef const value_type &      const_reference;
    typedef pointer                 iterator;

    array() {
    }

    array(const _My & ref) {
        copy(*this, ref);
    }

    size_type size() const {
        return _Num;
    }

    size_type capacity() const {
        return _Num;
    }

    bool empty() const {
        return (_Num == 0);
    }

    void clear() {
        memset(buffer, 0, sizeof(buffer));
    }

    reference at(size_type idx) {
        FwAssert(check_range(idx));
        return buffer[idx];
    }

    const_reference at(size_type idx) const {
        FwAssert(check_range(idx));
        return buffer[idx];
    }

    iterator begin() {
        return &buffer[0];
    }

    iterator end() {
        return &buffer[size()];
    }

    reference front() {
        return at(0);
    }

    const_reference front() const {
        return at(0);
    }

    reference back() {
        return at(size() - 1);
    }

    const_reference back() const {
        return at(size() - 1);
    }

    pointer data() {
        return buffer;
    }

    const_pointer data() const {
        return buffer;
    }

    void fill(const _Ty & value) {
        for (size_type i = 0; i < size(); ++i) {
            buffer[i] = value;
        }
    }

    void swap(const _My & other) {
        _Ty tmp;
        for (size_type i = 0; i < size(); ++i) {
            tmp = buffer[i];
            buffer[i] = other.buffer[i];
            other.buffer[i] = tmp;
        }
    }

    _Ty ** operator &() {
        return &buffer;
    }

    reference operator [](size_type idx) {
        return at(idx);
    }

    const_reference operator [](size_type idx) const {
        return at(idx);
    }

    const array<_Ty, _Num> & operator =(const array<_Ty, _Num> & ref) {
        copy(*this, ref);
        return *this;
    }

private:
    bool check_range(size_type idx) {
        return (0 <= idx && idx < _Num);  
    }

    void copy(const _My & dst, const _My & src) {
        for (size_type i = 0; i < size(); ++i) {
            dst[i] = src[i];
        }
    }


    _Ty buffer[_Num == 0 ? 1 : _Num];
};


template<typename _Ty, size_t _Num, size_t _Align>
class block_array {
public:
    typedef block_array<_Ty, _Num, _Align>  _My;
    typedef _Ty                             value_type;
    typedef size_t                          size_type;
    typedef ptrdiff_t                       defference_type;
    typedef value_type *                    pointer;
    typedef const value_type *              const_pointer;
    typedef value_type &                    reference;
    typedef const value_type &              const_reference;

    block_array() {
    }

    block_array(const _My & ref) {
        copy(*this, ref);
    }

    size_type size() const {
        return _Num;
    }

    size_type max_size() const {
        return _Num;
    }

    bool empty() const {
        return (_Num == 0);
    }

    void clear() {
        memset(buffer, 0, sizeof(buffer));
    }

    reference at(size_type idx) {
        FwAssert(check_range(idx));
        return ref(idx);
    }

    const_reference at(size_type idx) const {
        FwAssert(check_range(idx));
        return ref(idx);
    }

    reference front() {
        return ref(0);
    }

    const_reference front() const {
        return ref(0);
    }

    reference back() {
        return ref(size() - 1);
    }

    const_reference back() const {
        return ref(size() - 1);
    }

    pointer data() {
        return buffer;
    }

    const_pointer data() const {
        return buffer;
    }

    void fill(const _Ty & value) {
        for (size_type i = 0; i < size(); ++i) {
            ref(i) = value;
        }
    }

    void swap(const _My & other) {
        _Ty tmp;
        for (size_type i = 0; i < size(); ++i) {
            tmp = ref(i);
            ref(i) = other.ref(i);
            other.ref(i) = tmp;
        }
    }

    _Ty ** operator &() {
        return &buffer;
    }

    reference operator [](size_type idx) {
        return at(idx);
    }

    const_reference operator [](size_type idx) const {
        return at(idx);
    }

    const array<_Ty, _Num> & operator =(const array<_Ty, _Num> & ref) {
        copy(*this, ref);
        return *this;
    }


private:
    bool check_range(size_type idx) {
        return (0 <= idx && idx < _Num);  
    }

    void copy(const _My & dst, const _My & src) {
        for (size_type i = 0; i < size(); ++i) {
            dst[i] = src[i];
        }
    }

    reference ref(size_type idx) {
         return buffer[((sizeof(_Ty) + (_Align - 1)) & ~(_Align - 1)) * idx];
    }

    const_reference ref(size_type idx) const {
        return buffer[((sizeof(_Ty) + (_Align - 1)) & ~(_Align - 1)) * idx];
    }

    unsigned char buffer[((sizeof(_Ty) + (_Align - 1)) & ~(_Align - 1)) * (_Num == 0 ? 1 : _Num)];
};

END_NAMESPACE_FW

#endif  // FW_ARRAY_H_
