void build(Solution &s)
{
    auto &p = s.addLibrary("CLIUtils.CLI11", "1.9.0");
    p += Git("https://github.com/CLIUtils/CLI11", "v{v}");
}
