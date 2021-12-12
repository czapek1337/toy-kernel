#pragma once

namespace type_traits {

template <typename T, T __value>
struct IntegralConstant {
    using ValueType = T;

    static constexpr T value = __value;
};

template <bool __value>
using BoolConstant = IntegralConstant<bool, __value>;

using TrueConstant = BoolConstant<true>;
using FalseConstant = BoolConstant<false>;

template <typename>
struct IsPointerHelper : public FalseConstant {};

template <typename T>
struct IsPointerHelper<T *> : public TrueConstant {};

template <typename T>
struct IsPointer : public IsPointerHelper<T> {};

} // namespace type_traits

template <typename T, typename U>
concept IsSame = __is_same(T, U);

template <typename T>
concept IsPointer = type_traits::IsPointer<T>::value;
