// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___MEMORY_ALLOCATOR_H
#define _LIBCPP___MEMORY_ALLOCATOR_H

#include <__config>
#include <__memory/allocate_at_least.h>
#include <__memory/allocator_traits.h>
#include <__utility/forward.h>
#include <cstddef>
#include <new>
#include <stdexcept>
#include <type_traits>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Tp> class allocator;

#if _LIBCPP_STD_VER <= 17 || defined(_LIBCPP_ENABLE_CXX20_REMOVED_ALLOCATOR_VOID_SPECIALIZATION)
template <>
class _LIBCPP_TEMPLATE_VIS allocator<void>
{
#if _LIBCPP_STD_VER <= 17 || defined(_LIBCPP_ENABLE_CXX20_REMOVED_ALLOCATOR_MEMBERS)
public:
    _LIBCPP_DEPRECATED_IN_CXX17 typedef void*             pointer;
    _LIBCPP_DEPRECATED_IN_CXX17 typedef const void*       const_pointer;
    _LIBCPP_DEPRECATED_IN_CXX17 typedef void              value_type;

    template <class _Up> struct _LIBCPP_DEPRECATED_IN_CXX17 rebind {typedef allocator<_Up> other;};
#endif
};

template <>
class _LIBCPP_TEMPLATE_VIS allocator<const void>
{
#if _LIBCPP_STD_VER <= 17 || defined(_LIBCPP_ENABLE_CXX20_REMOVED_ALLOCATOR_MEMBERS)
public:
    _LIBCPP_DEPRECATED_IN_CXX17 typedef const void*       pointer;
    _LIBCPP_DEPRECATED_IN_CXX17 typedef const void*       const_pointer;
    _LIBCPP_DEPRECATED_IN_CXX17 typedef const void        value_type;

    template <class _Up> struct _LIBCPP_DEPRECATED_IN_CXX17 rebind {typedef allocator<_Up> other;};
#endif
};
#endif

// This class provides a non-trivial default constructor to the class that derives from it
// if the condition is satisfied.
//
// The second template parameter exists to allow giving a unique type to __non_trivial_if,
// which makes it possible to avoid breaking the ABI when making this a base class of an
// existing class. Without that, imagine we have classes D1 and D2, both of which used to
// have no base classes, but which now derive from __non_trivial_if. The layout of a class
// that inherits from both D1 and D2 will change because the two __non_trivial_if base
// classes are not allowed to share the same address.
//
// By making those __non_trivial_if base classes unique, we work around this problem and
// it is safe to start deriving from __non_trivial_if in existing classes.
template <bool _Cond, class _Unique>
struct __non_trivial_if { };

template <class _Unique>
struct __non_trivial_if<true, _Unique> {
    _LIBCPP_INLINE_VISIBILITY
    _LIBCPP_CONSTEXPR __non_trivial_if() _NOEXCEPT { }
};

// allocator
//
// Note: For ABI compatibility between C++20 and previous standards, we make
//       allocator<void> trivial in C++20.

template <class _Tp>
class _LIBCPP_TEMPLATE_VIS allocator
    : private __non_trivial_if<!is_void<_Tp>::value, allocator<_Tp> >
{
    static_assert(!is_volatile<_Tp>::value, "std::allocator does not support volatile types");
public:
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef _Tp         value_type;
    typedef true_type   propagate_on_container_move_assignment;
    typedef true_type   is_always_equal;

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
    allocator() _NOEXCEPT = default;

    template <class _Up>
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
    allocator(const allocator<_Up>&) _NOEXCEPT { }

    _LIBCPP_NODISCARD_AFTER_CXX17 _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
    _Tp* allocate(size_t __n) {
        if (__n > allocator_traits<allocator>::max_size(*this))
            __throw_bad_array_new_length();
        if (__libcpp_is_constant_evaluated()) {
            return static_cast<_Tp*>(::operator new(__n * sizeof(_Tp)));
        } else {
            return static_cast<_Tp*>(_VSTD::__libcpp_allocate(__n * sizeof(_Tp), _LIBCPP_ALIGNOF(_Tp)));
        }
    }

#if _LIBCPP_STD_VER > 20
    [[nodiscard]] _LIBCPP_HIDE_FROM_ABI constexpr
    allocation_result<_Tp*> allocate_at_least(size_t __n) {
        return {allocate(__n), __n};
    }
#endif

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
    void deallocate(_Tp* __p, size_t __n) _NOEXCEPT {
        if (__libcpp_is_constant_evaluated()) {
            ::operator delete((void*)__p);
        } else {
            _VSTD::__libcpp_deallocate((void*)__p, __n * sizeof(_Tp), _LIBCPP_ALIGNOF(_Tp));
        }
    }

    // C++20 Removed members
#if _LIBCPP_STD_VER <= 17 || defined(_LIBCPP_ENABLE_CXX20_REMOVED_ALLOCATOR_MEMBERS)
    _LIBCPP_DEPRECATED_IN_CXX17 typedef _Tp*       pointer;
    _LIBCPP_DEPRECATED_IN_CXX17 typedef const _Tp* const_pointer;
    _LIBCPP_DEPRECATED_IN_CXX17 typedef _Tp&       reference;
    _LIBCPP_DEPRECATED_IN_CXX17 typedef const _Tp& const_reference;

    template <class _Up>
    struct _LIBCPP_DEPRECATED_IN_CXX17 rebind {
        typedef allocator<_Up> other;
    };

    _LIBCPP_DEPRECATED_IN_CXX17 _LIBCPP_INLINE_VISIBILITY
    pointer address(reference __x) const _NOEXCEPT {
        return _VSTD::addressof(__x);
    }
    _LIBCPP_DEPRECATED_IN_CXX17 _LIBCPP_INLINE_VISIBILITY
    const_pointer address(const_reference __x) const _NOEXCEPT {
        return _VSTD::addressof(__x);
    }

    _LIBCPP_NODISCARD_AFTER_CXX17 _LIBCPP_INLINE_VISIBILITY _LIBCPP_DEPRECATED_IN_CXX17
    _Tp* allocate(size_t __n, const void*) {
        return allocate(__n);
    }

    _LIBCPP_DEPRECATED_IN_CXX17 _LIBCPP_INLINE_VISIBILITY size_type max_size() const _NOEXCEPT {
        return size_type(~0) / sizeof(_Tp);
    }

    template <class _Up, class... _Args>
    _LIBCPP_DEPRECATED_IN_CXX17 _LIBCPP_INLINE_VISIBILITY
    void construct(_Up* __p, _Args&&... __args) {
        ::new ((void*)__p) _Up(_VSTD::forward<_Args>(__args)...);
    }

    _LIBCPP_DEPRECATED_IN_CXX17 _LIBCPP_INLINE_VISIBILITY
    void destroy(pointer __p) {
        __p->~_Tp();
    }
#endif
};

template <class _Tp>
class _LIBCPP_TEMPLATE_VIS allocator<const _Tp>
    : private __non_trivial_if<!is_void<_Tp>::value, allocator<const _Tp> >
{
    static_assert(!is_volatile<_Tp>::value, "std::allocator does not support volatile types");
public:
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef const _Tp   value_type;
    typedef true_type   propagate_on_container_move_assignment;
    typedef true_type   is_always_equal;

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
    allocator() _NOEXCEPT = default;

    template <class _Up>
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
    allocator(const allocator<_Up>&) _NOEXCEPT { }

    _LIBCPP_NODISCARD_AFTER_CXX17 _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
    const _Tp* allocate(size_t __n) {
        if (__n > allocator_traits<allocator>::max_size(*this))
            __throw_bad_array_new_length();
        if (__libcpp_is_constant_evaluated()) {
            return static_cast<const _Tp*>(::operator new(__n * sizeof(_Tp)));
        } else {
            return static_cast<const _Tp*>(_VSTD::__libcpp_allocate(__n * sizeof(_Tp), _LIBCPP_ALIGNOF(_Tp)));
        }
    }

#if _LIBCPP_STD_VER > 20
    [[nodiscard]] _LIBCPP_HIDE_FROM_ABI constexpr
    allocation_result<const _Tp*> allocate_at_least(size_t __n) {
        return {allocate(__n), __n};
    }
#endif

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
    void deallocate(const _Tp* __p, size_t __n) {
        if (__libcpp_is_constant_evaluated()) {
            ::operator delete(const_cast<_Tp*>(__p));
        } else {
            _VSTD::__libcpp_deallocate((void*) const_cast<_Tp *>(__p), __n * sizeof(_Tp), _LIBCPP_ALIGNOF(_Tp));
        }
    }

    // C++20 Removed members
#if _LIBCPP_STD_VER <= 17 || defined(_LIBCPP_ENABLE_CXX20_REMOVED_ALLOCATOR_MEMBERS)
    _LIBCPP_DEPRECATED_IN_CXX17 typedef const _Tp* pointer;
    _LIBCPP_DEPRECATED_IN_CXX17 typedef const _Tp* const_pointer;
    _LIBCPP_DEPRECATED_IN_CXX17 typedef const _Tp& reference;
    _LIBCPP_DEPRECATED_IN_CXX17 typedef const _Tp& const_reference;

    template <class _Up>
    struct _LIBCPP_DEPRECATED_IN_CXX17 rebind {
        typedef allocator<_Up> other;
    };

    _LIBCPP_DEPRECATED_IN_CXX17 _LIBCPP_INLINE_VISIBILITY
    const_pointer address(const_reference __x) const _NOEXCEPT {
        return _VSTD::addressof(__x);
    }

    _LIBCPP_NODISCARD_AFTER_CXX17 _LIBCPP_INLINE_VISIBILITY _LIBCPP_DEPRECATED_IN_CXX17
    const _Tp* allocate(size_t __n, const void*) {
        return allocate(__n);
    }

    _LIBCPP_DEPRECATED_IN_CXX17 _LIBCPP_INLINE_VISIBILITY size_type max_size() const _NOEXCEPT {
        return size_type(~0) / sizeof(_Tp);
    }

    template <class _Up, class... _Args>
    _LIBCPP_DEPRECATED_IN_CXX17 _LIBCPP_INLINE_VISIBILITY
    void construct(_Up* __p, _Args&&... __args) {
        ::new ((void*)__p) _Up(_VSTD::forward<_Args>(__args)...);
    }

    _LIBCPP_DEPRECATED_IN_CXX17 _LIBCPP_INLINE_VISIBILITY
    void destroy(pointer __p) {
        __p->~_Tp();
    }
#endif
};

template <class _Tp, class _Up>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
bool operator==(const allocator<_Tp>&, const allocator<_Up>&) _NOEXCEPT {return true;}

template <class _Tp, class _Up>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
bool operator!=(const allocator<_Tp>&, const allocator<_Up>&) _NOEXCEPT {return false;}

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___MEMORY_ALLOCATOR_H
