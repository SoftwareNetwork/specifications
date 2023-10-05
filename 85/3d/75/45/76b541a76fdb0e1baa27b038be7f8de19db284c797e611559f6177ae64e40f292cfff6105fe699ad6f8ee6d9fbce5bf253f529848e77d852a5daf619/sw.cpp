void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("boostorg.pfr", "develop");
    t += Git("https://github.com/boostorg/pfr");
}
