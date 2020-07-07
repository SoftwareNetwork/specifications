void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("apolukhin.magic_get", "1.0.1");
    t += Git("https://github.com/apolukhin/magic_get");
}
