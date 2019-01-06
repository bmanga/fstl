#pragma once

#ifndef FSTL_TYPE_TRAITS_H
#define FSTL_TYPE_TRAITS_H

namespace fstl {

namespace type_traits_detail {
template <class T> struct add_reference { using lvalue = T &; using rvalue = T &&; };
template <class T> struct add_reference<T &> { using lvalue = T &; using rvalue = T &&; };
template <class T> struct add_reference<T &&> { using lvalue = T &; using rvalue = T &&; };

template<class T>
typename add_reference<T>::rvalue declval() noexcept;
}

struct false_type { static constexpr bool value = false; };
struct true_type  { static constexpr bool value = true; };

template <class ...> using void_t = void;

template <class T, class = void>
struct is_default_constructible : false_type {};

template <class T> struct is_default_constructible<T, void_t<decltype(T())>>
  : true_type {};

template <class T, class = void>
struct is_copy_constructible : false_type {};

template <class T> struct is_copy_constructible<T, void_t<decltype(T(type_traits_detail::declval<const T&>()))>>
  : true_type {};


}

#endif //FSTL_TYPE_TRAITS_H
