void build(Solution &s)
{
    auto &apu = s.addTarget<LibraryTarget>("apache.apr.util", "1.6.1");
    apu += Git("https://github.com/apache/apr-util", "{v}");

    apu +=
        "include/.*"_rr,

        //"buckets/.*"_rr,
        "crypto/.*"_rr,
        "encoding/.*"_rr,
        "hooks/.*"_rr,
        //"memcache/.*"_rr,
        //"ldap/.*"_rr,
        "misc/.*"_rr,
        //"redis/.*"_rr,
        "strmatch/.*"_rr,
        "uri/.*"_rr,
        "xlate/.*"_rr,
        "xml/.*"_rr
        ;
    //apu -= "buckets/apr_buckets_mmap.c";

    apu.Public += "include"_id;
    apu += "include/private"_id;

    apu += sw::Shared, "APU_DECLARE_EXPORT"_d;
    apu.Public += sw::Static, "APU_DECLARE_STATIC"_d;

    // .hw - w for windows
    //apu.configureFile("include/apr_ldap.hw", "apr_ldap.h");
    apu.configureFile("include/apu.hw", "apu.h");
    apu.configureFile("include/private/apu_config.hw", "apu_config.h");

    apu.Public += "org.sw.demo.apache.apr.iconv"_dep;
    apu.Public += "org.sw.demo.expat"_dep;
}
