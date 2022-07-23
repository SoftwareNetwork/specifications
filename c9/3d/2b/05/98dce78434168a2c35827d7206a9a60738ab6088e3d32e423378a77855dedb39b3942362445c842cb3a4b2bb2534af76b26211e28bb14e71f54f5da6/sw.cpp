void build(Solution &s)
{
    auto &t = s.addStaticLibrary("ArtifexSoftware.extract", "c10c442dfea1cb49f63ee9f97f05402d5678f271d");
    t += Git("https://github.com/ArtifexSoftware/extract", "", "", "10c442dfea1cb49f63ee9f97f05402d5678f271d");

    t -= "src/.*exe.*"_rr;
    t -= "src/.*test.*"_rr;

    t += "org.sw.demo.madler.zlib"_dep;
}
