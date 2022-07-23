void build(Solution &s)
{
    auto &t = s.addStaticLibrary("ArtifexSoftware.extract", "master");
    t += Git("https://github.com/ArtifexSoftware/extract");

    t -= "src/.*exe.*"_rr;
    t -= "src/.*test.*"_rr;

    t += "org.sw.demo.madler.zlib"_dep;
}
