void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("p_ranav.argparse", "2.0.1");
    t += Git("https://github.com/p-ranav/argparse", "v{v}");
}
