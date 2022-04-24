void build(Solution &s)
{
    auto &t = s.addStaticLibrary("marzer.tomlplusplus", "3.1.0");
    t += Git("https://github.com/marzer/tomlplusplus", "v{v}");
    t += "include/.*"_rr;
    t.Public += "toml++.natvis";
}
