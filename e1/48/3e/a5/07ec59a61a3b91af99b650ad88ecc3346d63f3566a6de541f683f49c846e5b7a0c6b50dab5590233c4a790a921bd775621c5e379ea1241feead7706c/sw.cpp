void build(Solution &s)
{
    auto &p = s.addProject("LLNL", "0.5.4");
    p += Git("https://github.com/LLNL/zfp");

    auto &zfp = p.addTarget<StaticLibrary>("zfp");
    {
        auto &t = zfp;
        t += "include/.*"_rr;
        t += "src/.*"_rr;
        t += "array/.*"_rr;

        t -= "src/inline/.*"_rr;
        t -= "src/template/.*"_rr;

        t += "ZFP_SOURCE"_def;

        t.Public += "include"_idir;
        t.Protected += "src"_idir;
        t.Protected += "array"_idir;
    }

    auto &cfp = p.addTarget<StaticLibrary>("cfp");
    {
        auto &t = cfp;
        t.setRootDirectory("cfp");

        t -= "include/.*"_rr;
        t -= "src/.*"_rr;
        t += "src/cfparrays.cpp";

        t += "ZFP_SOURCE"_def;
        t.Public += "CFP_NAMESPACE=cfp"_def;
        t.Public += zfp;
    }
}
