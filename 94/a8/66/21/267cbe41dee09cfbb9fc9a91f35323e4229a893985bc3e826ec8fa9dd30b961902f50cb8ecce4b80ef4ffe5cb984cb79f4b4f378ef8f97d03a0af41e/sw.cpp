void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("p_ranav.argparse", "2.9.0");
    t += Git("https://github.com/p-ranav/argparse", "v{M}.{m}");
}
