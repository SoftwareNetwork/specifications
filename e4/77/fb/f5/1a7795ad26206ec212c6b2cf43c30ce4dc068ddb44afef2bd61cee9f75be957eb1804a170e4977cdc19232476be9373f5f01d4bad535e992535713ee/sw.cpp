void build(Solution &s)
{
    auto &p = s.addLibrary("CLIUtils.CLI11", "1.8.0");
    p += Git("https://github.com/CLIUtils/CLI11", "v{v}");
}
