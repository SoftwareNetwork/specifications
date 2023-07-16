void build(Solution &s)
{
    auto &rhash = s.addTarget<LibraryTarget>("aleksey14.rhash", "1.4.4");
    rhash += Git("https://github.com/rhash/RHash", "v{v}");

    rhash.ApiNames.insert("RHASH_API");
    rhash +=
        "librhash/.*\\.c"_rr,
        "librhash/.*\\.h"_rr;

    rhash -= "librhash/test.*"_rr;
    rhash.Public += "librhash"_id;

    rhash += Definition("RHASH_XVERSION=" + rhash.Variables["PACKAGE_VERSION_NUM"].toString());

    rhash.patch("librhash/byte_order.h",
        "defined(_ARM_) || defined(__arm__)",
        "defined(_ARM_) ||  defined(__arm__) || defined(_M_ARM64) || defined(_M_ARM64EC)"
    );
}
