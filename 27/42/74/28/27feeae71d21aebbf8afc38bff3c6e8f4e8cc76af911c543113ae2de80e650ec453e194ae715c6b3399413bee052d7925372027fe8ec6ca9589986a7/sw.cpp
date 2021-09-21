void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("amazon.awslabs.s2n", "1.1.0");
    t += Git("https://github.com/awslabs/s2n", "v{v}");

    t += "S2N_API"_api;

    t += "api/.*"_rr;
    t += "crypto/.*"_rr;
    t += "error/.*"_rr;
    t += "stuffer/.*"_rr;
    t += "tls/.*"_rr;
    t += "utils/.*"_rr;
    t += "pq-crypto/.*"_rr;

    t -= "pq-crypto/sike_r1/.*"_rr;
    t += "pq-crypto/sike_r1/fp_generic_r1.c";
    t += "pq-crypto/sike_r1/P503_r1.c";
    t += "pq-crypto/sike_r1/sike_r1_kem.c";
    t += "pq-crypto/sike_r1/fips202_r1.c";

    /*t -= "pq-crypto/sike_r2/.*"_rr;
    t += "pq-crypto/sike_r2/fips202.c";
    t += "pq-crypto/sike_r2/P434.c";*/

    t.Public += "api"_idir;
    t += "."_idir;

    t.patch("api/s2n.h", "#    define S2N_API", "//#     define S2N_API");
    t.patch("utils/s2n_asn1_time.c", "__tm_gmtoff", "tm_gmtoff");

    t.Public += "org.sw.demo.openssl.crypto"_dep;
}
