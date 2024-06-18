void build(Solution &s)
{
    auto &t = s.addLibrary("ToruNiina.toml11", "4.0.0");
    t += Git("https://github.com/ToruNiina/toml11", "v{v}");
    t += "include/.*"_rr;
    //t += "TOML11_COMPILE_SOURCES"_def;
}
