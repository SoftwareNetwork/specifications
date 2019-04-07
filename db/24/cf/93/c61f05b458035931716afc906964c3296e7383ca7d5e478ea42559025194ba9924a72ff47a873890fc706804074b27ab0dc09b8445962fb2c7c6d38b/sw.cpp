void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("google.cctz", "2.3");
    t += Git("https://github.com/google/cctz", "v{M}.{m}");

    t += "include/.*"_rr;
    t += "src/.*"_rr;
    t -= ".*test.*"_rr;
    t -= ".*bench.*"_rr;
}
