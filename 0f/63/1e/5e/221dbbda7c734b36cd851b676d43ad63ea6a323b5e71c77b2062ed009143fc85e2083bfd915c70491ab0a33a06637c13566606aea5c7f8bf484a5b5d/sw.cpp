void build(Solution &s)
{
    auto &t = s.addLibrary("CLIUtils.CLI11", "2.1.1");
    t += Git("https://github.com/CLIUtils/CLI11", "v{v}");
}
