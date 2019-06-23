void build(Solution &s)
{
    auto &t = s.addTarget<Library>("microsoft.mimalloc", "master");
    t += Git("https://github.com/microsoft/mimalloc");

    t += "include/.*"_rr;
    t -= "src/.*"_rr;
    t += "src/static.c";

    t.Public += sw::Shared, "MI_SHARED_LIB"_def;
    t += sw::Shared, "MI_SHARED_LIB_EXPORT"_def;
}
