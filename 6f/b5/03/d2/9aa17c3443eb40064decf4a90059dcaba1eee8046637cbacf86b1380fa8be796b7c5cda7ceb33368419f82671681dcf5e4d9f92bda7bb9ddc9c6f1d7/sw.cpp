void build(Solution &s)
{
    auto &t = s.addTarget<Library>("daniele77.cli", "1.0.0");
    t += Git("https://github.com/daniele77/cli", "v{v}", "{v}");
    t -= "cli/.*"_rr;
}
