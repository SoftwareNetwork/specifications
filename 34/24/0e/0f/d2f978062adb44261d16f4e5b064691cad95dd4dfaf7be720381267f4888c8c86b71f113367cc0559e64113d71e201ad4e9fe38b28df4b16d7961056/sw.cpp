void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("Neargye.magic_enum", "0.9.7");
    t += Git("https://github.com/Neargye/magic_enum", "v{v}");
}
