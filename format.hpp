#ifndef _TJH_FORMAT_HPP__
#define _TJH_FORMAT_HPP__

#include<sstream>
#include<string>
#include<iostream>
#include<iomanip>

#include"compileTimeString.hpp"

namespace tjh
{
namespace fmt
{

template<char ...c, typename ...Args>
std::stringstream fmtss(CompileTime::String<c...> fmt, Args &&... args);

template<char ...c, typename ...Args>
std::string fmts(CompileTime::String<c...> fmt, Args &&... args);

template<char ...c, typename ...Args>
size_t fmt(CompileTime::String<c...> fmt, Args &&... args);

template<typename T>
struct hex {
    hex(const T &t): t(t) {}
    friend std::ostream &operator<<(std::ostream &o, const hex &h)
    { return o << std::hex << h.t; }
  private:
    const T &t;
};
template<typename T>
hex<decay_t<T>> Hex(T &&t) { return {t}; }

template<typename T>
struct width {
    width(size_t w, const T &t): w(w), t(t) {}
    friend std::ostream &operator<<(std::ostream &o, const width &w)
    { return o << std::setw(w.w) << w.t; }
  private:
    size_t w;
    const T &t;
};
template<typename T>
width<decay_t<T>> Width(size_t w, T &&t) { return {w, t}; }

template<typename T>
struct left {
    left(const T &t): t(t) {}
    friend std::ostream &operator<<(std::ostream &o, const left &l)
    { return o << std::left << l.t; }
  private:
    const T &t;
};
template<typename T>
left<decay_t<T>> Left(T &&t) { return {t}; }

template<typename T>
struct right {
    right(const T &t): t(t) {}
    friend std::ostream &operator<<(std::ostream &o, const right &r)
    { return o << std::right << r.t; }
  private:
    const T &t;
};
template<typename T>
right<decay_t<T>> Right(T &&t) { return {t}; }

template<typename T>
struct fill {
    fill(char c, const T &t): c(c), t(t) {}
    friend std::ostream &operator<<(std::ostream &o, const fill &f)
    { return o << std::setfill(f.c) << f.t; }
  private:
    char c;
    const T &t;
};
template<typename T>
fill<decay_t<T>> Fill(char c, T &&t) { return {c, t}; }

namespace detail
{
template<char ...c>
using Cts = CompileTime::String<c...>;
template<char ...c>
constexpr size_t GetFmtSize(Cts<c...> str)
{
    using CompileTime::operator""_cts;
    constexpr auto idx1 = str.FindFirstIdx(CTS("%%"));
    constexpr auto idx2 = str.FindFirstIdx(CTS("%{}"));
    constexpr auto idx3 = str.FindFirstIdx(CTS("{}"));
    if constexpr(idx1 >= 0 && idx2 >= 0 && idx1 < idx2) {       //%% %{}
        constexpr int begin = idx1 + 2;
        constexpr int num = str.Len() - idx1 - 2;
        constexpr auto newStr = str.template SubStr<begin, num>();
        return GetFmtSize(newStr);
    } else if constexpr(idx1 >= 0 && idx2 < 0) {                //%%
        constexpr int begin = idx1 + 2;
        constexpr int num = str.Len() - idx1 - 2;
        constexpr auto newStr = str.template SubStr<begin, num>();
        return GetFmtSize(newStr);
    } else if constexpr(idx1 < 0 && idx2 >= 0) {                //%{}
        constexpr int begin = idx2 + 3;
        constexpr int num = str.Len() - idx2 - 3;
        constexpr auto newStr = str.template SubStr<begin, num>();
        return GetFmtSize(newStr);
    } else if constexpr(idx1 < 0 && idx2 < 0 && idx3 >= 0) {    //{}
        constexpr int begin = idx3 + 2;
        constexpr int num = str.Len() - idx3 - 2;
        constexpr auto newStr = str.template SubStr<begin, num>();
        return 1 + GetFmtSize(newStr);
    } else {
        return 0;
    }
}
template<ssize_t idx, char ...c>
constexpr ssize_t GetFmtIdx(Cts<c...> str)
{
    if constexpr(idx >= (ssize_t)GetFmtSize(str)) {
        return -1;
    } else if constexpr(idx == -1) {
        return 0;
    }
    using CompileTime::operator""_cts;
    constexpr auto idx1 = str.FindFirstIdx(CTS("%%"));
    constexpr auto idx2 = str.FindFirstIdx(CTS("%{}"));
    constexpr auto idx3 = str.FindFirstIdx(CTS("{}"));
    if constexpr(idx1 >= 0 && idx2 >= 0 && idx1 < idx2) {       //%% %{}
        constexpr int begin = idx1 + 2;
        constexpr int num = str.Len() - idx1 - 2;
        constexpr auto newStr = str.template SubStr<begin, num>();
        return begin + GetFmtIdx<idx>(newStr);
    } else if constexpr(idx1 >= 0 && idx2 < 0) {                //%%
        constexpr int begin = idx1 + 2;
        constexpr int num = str.Len() - idx1 - 2;
        constexpr auto newStr = str.template SubStr<begin, num>();
        return begin + GetFmtIdx<idx>(newStr);
    } else if constexpr(idx1 < 0 && idx2 >= 0) {                //%{}
        constexpr int begin = idx2 + 3;
        constexpr int num = str.Len() - idx2 - 3;
        constexpr auto newStr = str.template SubStr<begin, num>();
        return begin + GetFmtIdx<idx>(newStr);
    } else if constexpr(idx1 < 0 && idx2 < 0 && idx3 >= 0) {    //{}
        constexpr int begin = idx3 + 2;
        constexpr int num = str.Len() - idx3 - 2;
        constexpr auto newStr = str.template SubStr<begin, num>();
        return  idx3 + GetFmtIdx < idx - 1 > (newStr);
    } else {
        return 0;
    }
}
template<char ...c>
std::stringstream &fmtss(std::stringstream &o, CompileTime::String<c...>fmt) { return o; }
template<char ...c, typename A, typename ...Args>
std::stringstream &fmtss(std::stringstream &o, CompileTime::String<c...>fmt, A &&a0, Args &&... a)
{
    constexpr auto idx1 = GetFmtIdx<0>(fmt);
    if(idx1 < 0) {
        return o;
    }
    constexpr auto firstPart = fmt.template SubStr<0, idx1>();
    constexpr auto secondPart = fmt.template SubStr < idx1 + 2, fmt.Len() - idx1 - 2 > ();
    o << firstPart << a0;
    return fmtss(o, secondPart, std::forward<Args>(a)...);
}
}

/************ implement **********/
template<char ...c, typename ...Args>
std::stringstream fmtss(CompileTime::String<c...> fmt, Args &&... args)
{
    static_assert(detail::GetFmtSize(fmt) <= sizeof...(args));
    std::stringstream ss;
    return {std::move(detail::fmtss(ss, fmt, std::forward<Args>(args)...))};
}

template<char ...c, typename ...Args>
std::string fmts(CompileTime::String<c...> fmt, Args &&... args)
{
    static_assert(detail::GetFmtSize(fmt) <= sizeof...(args));
    std::stringstream ss;
    return {std::move(detail::fmtss(ss, fmt, std::forward<Args>(args)...).str())};
}

template<char ...c, typename ...Args>
size_t fmt(CompileTime::String<c...> fmt, Args &&... args)
{
    static_assert(detail::GetFmtSize(fmt) <= sizeof...(args));
    std::stringstream ss;
    auto &&str = detail::fmtss(ss, fmt, std::forward<Args>(args)...).str();
    std::cout << str;
    return str.length();
}

}
}

#define FMTSS(__fmt, ...) tjh::fmt::fmtss(CTS(__fmt), __VA_ARGS__)
#define FMTS(__fmt, ...) tjh::fmt::fmts(CTS(__fmt), __VA_ARGS__)
#define FMT(__fmt, ...) tjh::fmt::fmt(CTS(__fmt), __VA_ARGS__)

#endif

struct Type {
    friend std::ostream &operator<<(std::ostream &o, const Type &t)
    { return o << "this is type operator<<()"; }
};

int main()
{
    using CompileTime::operator""_cts;

    //we see %{} as normal \{\}，not a placeholder
    //print: %{, this is a int: }, 255, this is type operator<<()
    std::cout << tjh::fmt::fmtss(CTS("%{{}}, {}, {}"), ", this is a int: ", 255.0, Type{}).str() << std::endl;

    //print: 1st: -1, 2nd:this is type operator<<()
    std::cout << FMTS("1st: {}, 2nd:{};", -1, Type{}) << std::endl;

    //print: 1st: 0x00001, 2nd:1.1
    std::cout << FMTS("1st: 0x{}, 2nd:{};",
                       tjh::fmt::Fill('0', tjh::fmt::Width(5, tjh::fmt::Hex(1))),
                       1.1) << std::endl;
}
