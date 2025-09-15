void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("baldurk.renderdoc", "1.40.0");
    t += Git("https://github.com/baldurk/renderdoc", "v{M}.{m}");

    t += "renderdoc/api/.*"_rr;
    t.Public += "renderdoc/api/app"_idir;
}
