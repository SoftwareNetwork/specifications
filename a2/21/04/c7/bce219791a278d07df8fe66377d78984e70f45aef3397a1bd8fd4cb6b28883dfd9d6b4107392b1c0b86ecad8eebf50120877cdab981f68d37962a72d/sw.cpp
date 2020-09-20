void build(Solution &s)
{
    auto &t = s.addLibrary("ToruNiina.toml11", "3.6.0");
    t += Git("https://github.com/ToruNiina/toml11", "v{v}");

    t += ".*\\.hpp"_r;
    t += "toml/.*"_rr;
}
