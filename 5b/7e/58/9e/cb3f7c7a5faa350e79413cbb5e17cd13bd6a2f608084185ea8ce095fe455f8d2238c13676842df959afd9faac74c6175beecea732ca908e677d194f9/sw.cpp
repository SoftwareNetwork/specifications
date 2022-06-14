void build(Solution &s)
{
    auto &rhash = s.addTarget<LibraryTarget>("aleksey14.rhash", "1.4.3");
    rhash += Git("https://github.com/rhash/RHash", "v{v}");

    rhash.ApiNames.insert("RHASH_API");
    rhash +=
        "librhash/.*\\.c"_rr,
        "librhash/.*\\.h"_rr;

    rhash -= "librhash/test.*"_rr;
    rhash.Public += "librhash"_id;

    rhash += Definition("RHASH_XVERSION=" + rhash.Variables["PACKAGE_VERSION_NUM"].toString());
}
