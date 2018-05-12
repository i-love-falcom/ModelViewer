/**
 * @file fw_memory.h
 * @author Kamai Masayoshi
 */
#ifndef FW_CONTAINER_ALLOCATOR_H_
#define FW_CONTAINER_ALLOCATOR_H_

#include "core/fw_allocator.h"


BEGIN_NAMESPACE_FW

template<class _Ty, uint64_t _Attribute = defaultFwMallocAttribute>
class allocator {
public:
    typedef _Ty                 value_type;
    typedef value_type *        pointer;
    typedef const value_type *  const_pointer;
    typedef void *              void_pointer;
    typedef const void *        const_void_pointer;
    typedef value_type &        reference;
    typedef const value_type &  const_reference;
    typedef size_t              size_type;
    typedef ptrdiff_t           difference_type;

    template<class _Other>
    struct rebind {
        typedef allocator<_Other, _Attribute> other;
    };

    pointer address(reference _Val) const throw() {
        return std::addressof(_Val);
    }

    const_pointer address(const_reference _Val) const throw() {
        return std::addressof(_Val);
    }

    pointer allocate(size_type _Count) {
        return reinterpret_cast<pointer>(FwMalloc(_Count * sizeof (_Ty), _Attribute));
    }

    pointer allocate(size_type _Count, const void *) {
        return (allocate(_Count));
    }

    void construct(pointer _Ptr) {
        ::new ((void *)_Ptr) _Ty();
    }

    void construct(pointer _Ptr, const_reference _Val) {
        ::new ((void *)_Ptr) _Ty(_Val);
    }

    void deallocate(pointer _Ptr, size_type) {
        _Ptr->~_Ty(); FwFree(_Ptr);
    }

    template<class _Other>
    void destroy(_Other * _Ptr) {
        _Ptr->~_Other();
    }

	size_t max_size() const throw() {
        return ((size_t)(-1) / sizeof (_Ty));
    }

    template<class _Other>
	allocator<_Ty, _Attribute> & operator =(const allocator<_Other, _Attribute>&) {
	    return (*this);
	}

    allocator() throw() {
    }
    
    allocator(const allocator<_Ty, _Attribute> &) throw() {
    }
	
    template<class _Other>
	allocator(const allocator<_Other, _Attribute> &) throw() {
	}
};


template<>
class allocator<void> {
public:
    typedef allocator<void>     other;
    typedef void                value_type;
    typedef void *              pointer;
    typedef const void *        const_pointer;
    typedef void *              void_pointer;
    typedef const void *        const_void_pointer;

    template<class _Other>
    struct rebind {
        typedef allocator<_Other> other;
    };

    template<class _Other>
    allocator<void>& operator =(const allocator<_Other>&) {
        return (*this);
    }

    allocator() throw() {
    }

    allocator(const allocator<void>&) throw() {
    }

    template<class _Other>
    allocator(const allocator<_Other>&) throw() {
    }
};

template<class _Ty,	class _Other, uint64_t _Attribute, uint64_t _OtherAttribute>
FW_INLINE bool operator ==(const allocator<_Ty, _Attribute>&, const allocator<_Other, _OtherAttribute>&) throw() {
    return (true);
}

template<class _Ty, class _Other, uint64_t _Attribute, uint64_t _OtherAttribute>
FW_INLINE bool operator !=(const allocator<_Ty, _Attribute>& _Left, const allocator<_Other, _OtherAttribute>& _Right) throw() {
    return (!(_Left == _Right));
}

END_NAMESPACE_FW

#endif  // FW_CONTAINER_ALLOCATOR_H_
