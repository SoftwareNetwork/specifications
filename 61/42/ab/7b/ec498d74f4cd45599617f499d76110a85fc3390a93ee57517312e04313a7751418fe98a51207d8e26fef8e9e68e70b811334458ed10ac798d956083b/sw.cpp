void build(Solution &s)
{
    auto &libtommath = s.addTarget<StaticLibraryTarget>("libtom.libtommath", "1.1.0");
    libtommath += Git("https://github.com/libtom/libtommath", "v{v}");

    libtommath +=
        ".*\\.h"_rr,
        "bn.*\\.c"_rr;
}
