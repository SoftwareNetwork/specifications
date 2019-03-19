void build(Solution &s)
{
    auto &bearssl = s.addTarget<StaticLibraryTarget>("bearssl", "0.6.0");
    bearssl += Git("https://www.bearssl.org/git/BearSSL", "v{M}.{m}");

    bearssl +=
        "inc/.*"_rr,
        "src/.*"_rr;

    bearssl +=
        "src"_id;

    bearssl.Public +=
        "inc"_id;
}
