void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("Cyan4973.xxHash", "0.8.0");
    t += Git("https://github.com/Cyan4973/xxHash", "v{v}");
    t += "xxhash.[hc]"_rr;
    t += "xxh3.h";

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t += "WIN32"_def;
        t.patch("xxh3.h", "__attribute__((noinline))", "");
        t.patch("xxh3.h", "#   pragma intrinsic(_umul128)", "//#   pragma intrinsic(_umul128)");
    }

    t += sw::Shared, "XXH_EXPORT"_def;
    t.Interface += sw::Shared, "XXH_IMPORT"_def;

    t.patch("xxhash.h", "#  if defined(__GNUC__)", "#  if defined(__GNUC__) || defined(__APPLE__)");
}
