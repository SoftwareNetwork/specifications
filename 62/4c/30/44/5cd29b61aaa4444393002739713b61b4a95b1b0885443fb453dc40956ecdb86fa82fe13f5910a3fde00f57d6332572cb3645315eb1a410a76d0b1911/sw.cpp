void build(Solution &s)
{
    auto &t = s.addTarget<Library>("Cyan4973.xxHash", "0.7.0");
    t += Git("https://github.com/Cyan4973/xxHash", "v{v}");
    t += "xxhash.[hc]"_rr;
    t += "xxh3.h";

    if (t.getSettings().TargetOS.Type == OSType::Windows)
        t += "WIN32"_def;

    t += sw::Shared, "XXH_EXPORT"_def;
    t.Interface += sw::Shared, "XXH_IMPORT"_def;
}
