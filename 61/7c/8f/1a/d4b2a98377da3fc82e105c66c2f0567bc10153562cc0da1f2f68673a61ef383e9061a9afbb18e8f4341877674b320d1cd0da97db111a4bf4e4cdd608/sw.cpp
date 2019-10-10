void build(Solution &s)
{
    auto &jxrlib = s.addTarget<StaticLibraryTarget>("4creators.jxrlib", "2019.10.9");
    jxrlib += Git("https://github.com/4creators/jxrlib", "v{v}");

    jxrlib +=
        "common/include/.*"_rr,
        "image/decode/.*"_rr,
        "image/encode/.*"_rr,
        "image/sys/.*"_rr,
        "image/x86/.*"_rr,
        "jxrgluelib/.*\\.[hc]"_rr;

    jxrlib.Public +=
        "common/include"_idir,
        "jxrgluelib"_idir,
        "image/sys"_idir;
    jxrlib.Public += "."_idir;

    if (jxrlib.getBuildSettings().TargetOS.Type == OSType::Windows)
        jxrlib.Public += "WIN32"_def;
}
