#ifndef _COMPILE_TIME_STRING_HPP_
#define _COMPILE_TIME_STRING_HPP_

#include<iostream>
#include<cstring>

#include "myTypeTraits.hpp"

namespace CompileTime
{
using CharT = char;

//**INDENT-OFF**
template<template<int, int> typename F,
           typename Goal, int Begin, int End,
           enable_if_t<
                ((Begin + Goal::Len()) < End) &&
                std::is_same<
                    Goal, ret_type_t< F<Begin, Goal::Len()> >
                            >::value
                , int > = 0 
        >
constexpr ssize_t FindFirstIdxHelper()
{
    return Begin;
}

template<template<int, int> typename F, 
            typename Goal, int Begin, int End,
            enable_if_t<
                ((Begin + Goal::Len()) >= End)
                , int  > = 0
        >
constexpr ssize_t FindFirstIdxHelper()
{
    return -1;
}

template<template<int, int> typename F, 
            typename Goal, int Begin, int End,
            enable_if_t<
                ((Begin + Goal::Len()) == End - 1) &&
                !std::is_same<Goal, ret_type_t<
                        F<Begin, Goal::Len()> >
                             >::value
                  , int> = 0 
        >
constexpr ssize_t FindFirstIdxHelper()
{
    return -1;
}

template<template<int, int> typename F, 
            typename Goal, int Begin, int End,
            enable_if_t<
                ((Begin + Goal::Len()) < End - 1) &&
                !std::is_same<Goal, ret_type_t<
                        F<Begin, Goal::Len()> >
                             >::value
                  , int> = 0 
        >
constexpr ssize_t FindFirstIdxHelper()
{
    return FindFirstIdxHelper<F, Goal, Begin + 1, End>();
}

//**INDENT-ON**

template<CharT ...Chars>
struct String;
 
template<CharT ...Chars, int ...N>
constexpr auto SubStrHelper(RangeSeq<N...> r) -> String<getEleN<N, CharT, Chars...>::value...>;

template<CharT ...Chars>
struct String {
    static constexpr size_t N = sizeof...(Chars) + 1;
    static constexpr const CharT value[N] = {Chars..., '\0'};

    static void print() { std::cout << value << std::endl; }
    friend std::ostream &operator<<(std::ostream &o, const String &s) { return o << s.value; }
    friend std::stringstream &operator<<(std::stringstream &o, const String &s) { o << s.value; return o; }

    constexpr CharT operator[](size_t n) const { return n >= N ? '\0' : value[n]; };

    static constexpr size_t Len() { return N - 1; }

    //常用运算接口
    template<int Begin, int Num>
    constexpr auto SubStr() const -> decltype(SubStrHelper<Chars...>(MakeRangeSeq<Begin, Num>())) { return {}; }

    //**INDENT-OFF**
    template<int Begin, int Num>
    struct SubStrFunctor {
        constexpr auto operator()() const -> decltype(SubStrHelper<Chars...>(MakeRangeSeq<Begin, Num>())) 
        { return {}; }
    };
    //**INDENT-ON**

    template<CharT ...Chars2, enable_if_t<sizeof...(Chars) >= sizeof...(Chars2)> * = nullptr>
    constexpr ssize_t FindFirstIdx(String<Chars2...> s) const
    {
        return FindFirstIdxHelper<SubStrFunctor, String<Chars2...>, 0, N> ();
    }
    template<CharT ...Chars2, enable_if_t<sizeof...(Chars) < sizeof...(Chars2)> * = nullptr>
    constexpr ssize_t FindFirstIdx(String<Chars2...> s) const
    {
        return -1;
    }
};

template<CharT ...Chars1, CharT ...Chars2>
constexpr String<Chars1..., Chars2...> operator+(const String<Chars1...> &str1, const String<Chars2...> &str2)
{
    return {};
}

//编译期字符串构造接口

template<CharT ...Chars>
constexpr const CharT String<Chars...>::value[N];

template<size_t M, const char *str, int ...N>
constexpr String<str[N]...> make_cts_helper(IntSeq<N...> s) { return {}; }

template<size_t N, const char *str>
constexpr auto make_cts() -> decltype(make_cts_helper<N, str>(IntSeq_t < (int)N - 1 > {}))
{
    return make_cts_helper<N, str>(IntSeq_t < (int)N - 1 > {});
}

//c++11接口的编译期字符串获取宏函数，只能放在全局变量声明的地方使用
#define MAKE_CTS(name, str) \
    constexpr const char __make_cts_helper_##name[] = str; \
    constexpr auto name = CompileTime::make_cts<sizeof(str), __make_cts_helper_##name>()

#if __cplusplus > 201400L
// 字符串字面量操作符模板，只能这么写。需要c++14或以上
template <typename T, T ...Chars>
constexpr auto operator""_cts()
{
    return String<Chars...> {};
}
//编译期字符串获取的宏函数
#define EXPAND_MACRO(X) X
#define CTS(str) str""_cts
#endif

template<typename T>
const char *PrintType()
{
    static char name[1024];
    strcpy(name, __PRETTY_FUNCTION__);
    name[sizeof(__PRETTY_FUNCTION__) - 2] = '\0';
    return name + sizeof("const char* CompileTime::PrintType() [with T = ") - 1;
}


}

#if 0
MAKE_CTS(cpp11_str1, "Hello ");
MAKE_CTS(cpp11_str2, "World!");
MAKE_CTS(cpp11_str4, __FILE__);

int main()
{
    using namespace CompileTime;
#if __cplusplus > 201400L
    // using CompileTime::operator""_cts;
    // constexpr auto cpp14_str1 = CTS("Hello ");
    // constexpr auto cpp14_str2 = CTS("World!");
    // constexpr auto cpp14_str3 = cpp14_str1 + cpp14_str2;
    // constexpr auto cpp14_str4 = CTS(__FILE__);
    // cpp14_str3.print();
    // cpp14_str4.print();
    // std::cout << cpp14_str3.Len() << std::endl;
    // std::cout << cpp14_str3[6] << std::endl;
    // constexpr auto tmp = cpp14_str3.SubStr<2, 8>();
    // tmp.print();
    // constexpr auto idx1 = cpp14_str3.FindFirstIdx(CTS("lo Wor"));
    // constexpr auto idx2 = cpp14_str4.FindFirstIdx(CTS("Time"));
    // constexpr auto idx3 = cpp14_str4.FindFirstIdx(CTS("Time2"));
    // std::cout << idx1 << ", " << idx2 << ", " << idx3 << std::endl;
#elif __cplusplus < 201400L
    constexpr auto cpp11_str3 = cpp11_str1 + cpp11_str2;
    cpp11_str3.print();
    cpp11_str4.print();
    std::cout << CompileTime::Strlen(cpp11_str3) << std::endl;
    std::cout << cpp11_str3[6] << std::endl;
#endif
    constexpr auto strFind = CTS("1{}");
    constexpr auto idx4 = FindFirstIdxHelper<
        decltype(strFind)::template SubStrFunctor, 
        decltype(CTS("{}")), 
        0, 4> ();
}

// // 定义具有链接性的字符串
// constexpr const char hello[] = "Hello";

// int main()
// {
//     // printf("%c", foo<sizeof("Hello "), hello>());  // 传递具有链接性的字符串作为模板参数
//     CompileTime::foo<sizeof("Hello "), hello>().print();
//     return 0;
// }
#endif

#endif
