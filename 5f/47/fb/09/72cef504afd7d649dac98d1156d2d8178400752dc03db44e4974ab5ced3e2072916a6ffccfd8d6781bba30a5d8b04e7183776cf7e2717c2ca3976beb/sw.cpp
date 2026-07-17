void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("ThePhD.sol2", "3.5.0");
    //t += Git("https://github.com/ThePhD/sol2", "v{v}");
    t += Git("https://github.com/egorpugin/sol2", "develop");

    t += "include/.*"_rr;
    t.Public += "sol2.natvis";

    // original sol2 does not work with lua 5.5 yet
    t.Public += "org.sw.demo.lua"_dep;
}
