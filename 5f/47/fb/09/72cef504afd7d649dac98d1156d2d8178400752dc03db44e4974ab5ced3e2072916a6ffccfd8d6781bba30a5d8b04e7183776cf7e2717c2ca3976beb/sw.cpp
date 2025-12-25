void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("ThePhD.sol2", "3.5.0");
    t += Git("https://github.com/ThePhD/sol2", "v{v}");

    t += "include/.*"_rr;
    t.Public += "sol2.natvis";

    t.Public += "org.sw.demo.lua-5.4"_dep;
}
