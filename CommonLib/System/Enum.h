#pragma once

#include <iostream>
#include <utility>
#include <optional>

class Enum {
protected:
    // Helper that processes the entire sequence at once
    template <typename T, int MinRange, int... Is>
    static constexpr std::optional<T> LowestEnumValueOfImpl(std::integer_sequence<int, Is...>) {
        std::optional<T> result = std::nullopt;

        // A short-circuiting fold expression.
        // It expands to: (Check(-128) || Check(-127) || Check(-126) ...)
        // The moment one returns true, evaluation stops.
        (
            (EnumValueIsValid<T, static_cast<T>(MinRange + Is)>() ? (result = static_cast<T>(MinRange + Is), true) : false)
            || ...
        );

        return result;
    }

    //TODO: Untested
    template <typename T, int MaxRange, int... Is>
    static constexpr std::optional<T> LargestEnumValueOfImpl(std::integer_sequence<int, Is...>) {
        std::optional<T> result = std::nullopt;

        // A short-circuiting fold expression.
        // It expands to: (Check(128) || Check(127) || Check(126) ...)
        // The moment one returns true, evaluation stops.
        (
            (EnumValueIsValid<T, static_cast<T>(MaxRange - Is)>() ? (result = static_cast<T>(MaxRange - Is), true) : false)
            || ...
        );

        return result;
    }

public:
    // Internal trick using compiler-specific function name inspection
    template <typename T, T Value>
    static constexpr bool EnumValueIsValid() {
#if defined(__clang__) || defined(__GNUC__)
        // Compilers embed the actual enum name if valid, or a raw cast like (Priority)2 if invalid
        constexpr auto name = std::string_view{ __PRETTY_FUNCTION__ };
        return name.find(')') == std::string_view::npos && name.find(':') != std::string_view::npos;
#elif defined(_MSC_VER)
        constexpr auto name = std::string_view{ __FUNCSIG__ };
        return name.find_last_of("0123456789") == std::string_view::npos;
#else
        return false; // Fallback for unsupported compilers
#endif
    }

    //Also figure out if theres a way to handle different underlying type instead of 'int'
    //enum E1 : int
    //enum E2 : uint
    //enum E3 : float
    template <typename T, int MinRange = -128, int MaxRange = 127>
    static constexpr std::optional<T> LowestEnumValueOf() {
        constexpr auto RangeSize = MaxRange - MinRange + 1;
        static_assert(RangeSize > 0, "MaxRange must be greater than or equal to MinRange");

        return LowestEnumValueOfImpl<T, MinRange>(std::make_integer_sequence<int, RangeSize>{});
    }

    //TODO: Untested
    template <typename T, int MinRange = -128, int MaxRange = 127>
    static constexpr std::optional<T> LargestEnumValueOf() {
        constexpr auto RangeSize = MaxRange - MinRange + 1;
        static_assert(RangeSize > 0, "MaxRange must be greater than or equal to MinRange");

        return LargestEnumValueOfImpl<T, MaxRange>(std::make_integer_sequence<int, RangeSize>{});
    }
};