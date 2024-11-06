void build(Solution &s)
{
    auto &mlspp = s.addStaticLibrary("cisco.mlspp", "master");
    mlspp += Git("https://github.com/cisco/mlspp");

    auto add_lib = [&](auto &&name, bool lib = true) -> decltype(auto) {
        auto &t = mlspp.addStaticLibrary(name);
        t.setSourceDirectory("lib/"s + name);
        t += cpp17;
        //t += "include/.*"_rr;
        //t += "lib/.*"_rr;
        return t;
    };

    auto &tls_syntax = add_lib("tls_syntax");
    tls_syntax.writeFileOnce("namespace.h", "#pragma once\n#define MLS_NAMESPACE mlspp");
    auto &bytes = add_lib("bytes");
    bytes.Public += tls_syntax;
    auto &hpke = add_lib("hpke");
    hpke.Public += bytes;
    hpke.Public += "org.sw.demo.openssl.crypto"_dep;
    hpke.Public += "org.sw.demo.nlohmann.json.natvis"_dep;

    mlspp += cpp17;
    mlspp.Public += hpke;

    auto &mls_ds = add_lib("mls_ds");
    mls_ds.Public += mlspp;
    auto &mls_vectors = add_lib("mls_vectors");
    mls_vectors.Public += mlspp;

    //add_lib("bytes").writeFileOnce("namespace.h", "#pragma once\n#define MLS_NAMESPACE");
    //add_lib("bytes").configureFile("cmake/namespace.h.in", "namespace.h");
}
