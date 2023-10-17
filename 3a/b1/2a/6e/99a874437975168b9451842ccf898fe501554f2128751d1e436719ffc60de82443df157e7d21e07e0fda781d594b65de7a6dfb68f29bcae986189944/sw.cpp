void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("intel.ittnotify", "17.01.28");
    t += Git("https://github.com/intel/IntelSEAPI", "{M}.{m:02}.{p:02}");

    t.setRootDirectory("ittnotify");
}
