void build(Solution &s)
{
    auto &t = s.addStaticLibrary("cofyc.argparse", "1.1.0");
    t += Git("https://github.com/cofyc/argparse");
    t += "argparse.*"_r;
}
