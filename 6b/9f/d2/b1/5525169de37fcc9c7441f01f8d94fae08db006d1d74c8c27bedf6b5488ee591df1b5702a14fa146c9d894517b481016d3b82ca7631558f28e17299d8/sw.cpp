void build(Solution &s)
{
    auto &t = s.addStaticLibrary("ArtifexSoftware.extract", "10.5.1");
    t += Git("https://github.com/ArtifexSoftware/extract", "ghostpdl-{M}.{m:02}.{p}");

    t -= "src/.*-exe.c"_rr;
    t -= "src/.*-test.c"_rr;

    t.Public += "org.sw.demo.madler.zlib"_dep;
}
