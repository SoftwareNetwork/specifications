void build(Solution &s)
{
    auto &t = s.addLibrary("ToruNiina.toml11", "4.0.3");
    t += Git("https://github.com/ToruNiina/toml11", "v{v}");
    t += "include/.*"_rr;
    t.patch("include/toml11/fwd/comments_fwd.hpp", "std::ostream&& os", "std::ostream& os");
    //t += "TOML11_COMPILE_SOURCES"_def;
}
