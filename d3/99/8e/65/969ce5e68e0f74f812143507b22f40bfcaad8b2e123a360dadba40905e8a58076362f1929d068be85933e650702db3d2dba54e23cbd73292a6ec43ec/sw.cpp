void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("apolukhin.magic_get", "develop");
    t += Git("https://github.com/apolukhin/magic_get");
}
