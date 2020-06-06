void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("ThePhD.sol2", "3.2.1");
    t += Git("https://github.com/ThePhD/sol2", "v{v}");
}
