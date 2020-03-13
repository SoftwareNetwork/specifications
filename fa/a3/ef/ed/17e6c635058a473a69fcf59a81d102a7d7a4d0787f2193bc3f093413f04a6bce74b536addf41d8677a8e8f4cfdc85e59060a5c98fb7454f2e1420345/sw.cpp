void build(Solution &s)
{
    auto &t = s.addLibrary("CLIUtils.CLI11", "1.9.0");
    t += Git("https://github.com/CLIUtils/CLI11", "v{v}");
}
