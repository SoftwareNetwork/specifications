void build(Solution &s)
{
    auto &t = s.addStaticLibrary("marzer.tomlplusplus", "3.0.1");
    t += Git("https://github.com/marzer/tomlplusplus", "v{v}");
    t += "include/.*"_rr;
    t.Public += "toml++.natvis";
}
