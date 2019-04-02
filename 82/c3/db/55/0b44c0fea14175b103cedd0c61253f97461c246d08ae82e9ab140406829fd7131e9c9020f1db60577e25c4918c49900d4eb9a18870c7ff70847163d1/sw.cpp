void build(Solution &s)
{
    auto &p = s.addTarget<LibraryTarget>("tcbrindle.span", "master");
    p += Git("https://github.com/tcbrindle/span");
}
