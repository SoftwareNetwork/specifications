void build(Solution &s)
{
    auto &bigint = s.addTarget<StaticLibraryTarget>("mattmccutchen.bigint", "master");
    bigint += Git("https://mattmccutchen.net/bigint/bigint.git", "", "master");

    bigint +=
        "BigInteger.[hc][hc]"_r,
        "BigIntegerAlgorithms.[hc][hc]"_r,
        "BigIntegerLibrary.hh",
        "BigIntegerUtils.[hc][hc]"_r,
        "BigUnsigned.[hc][hc]"_r,
        "BigUnsignedInABase.[hc][hc]"_r,
        "NumberlikeArray.hh";
}
