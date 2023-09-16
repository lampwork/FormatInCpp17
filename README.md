# FormatInCpp17
a toy which can be used in cpp17; std::format alike

# cpp version must >= cpp17

# examples
```c++
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
```
