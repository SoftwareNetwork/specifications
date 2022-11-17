void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("apolukhin.magic_get", "2.0.0");
    t += Git("https://github.com/apolukhin/magic_get");
}
