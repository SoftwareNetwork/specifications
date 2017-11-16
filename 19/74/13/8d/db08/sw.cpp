void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &rhash = s.addTarget<LibraryTarget>("aleksey14.rhash", "1.3.4");
    rhash.Source = Git("https://github.com/rhash/RHash", "v{v}");

    rhash.ApiNames.insert("RHASH_API");
    rhash +=
        "librhash/.*\\.c"_rr,
        "librhash/.*\\.h"_rr,
        "win32/.*\\.h"_rr;

    rhash -=
        "librhash/test.*"_rr;

    rhash.Public +=
        "librhash"_id;

}