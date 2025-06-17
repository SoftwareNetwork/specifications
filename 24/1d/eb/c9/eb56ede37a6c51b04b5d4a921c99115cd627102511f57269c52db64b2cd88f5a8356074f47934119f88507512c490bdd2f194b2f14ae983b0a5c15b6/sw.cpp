void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("amazon.awslabs.s2n", "1.5.21");
    t += Git("https://github.com/awslabs/s2n", "v{v}");

    t += "S2N_API"_api;

    t += "api/.*"_rr;
    t += "crypto/.*"_rr;
    t += "error/.*"_rr;
    t += "stuffer/.*"_rr;
    t += "tls/.*"_rr;
    t += "utils/.*"_rr;
    //t += "pq-crypto/.*"_rr;

    t.Public += "api"_idir;
    t += "."_idir;
    if (!t.DryRun && !fs::exists(t.BinaryDir / "s2n")) {
        fs::create_directories(t.BinaryDir);
        fs::create_directory_symlink(t.SourceDir / "api", t.BinaryDir / "s2n");
    }

    t += "utils/s2n_prelude.h"_fi;

    t.patch("api/s2n.h", "#    define S2N_API", "//#     define S2N_API");
    //t.patch("utils/s2n_asn1_time.c", "__tm_gmtoff", "tm_gmtoff");

    if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
    {
        t += "pthread"_slib;
    }

    t.Public += "org.sw.demo.openssl.crypto"_dep;
}
