void build(Solution &s)
{
    auto &t = s.addStaticLibrary("xiph.opusfile", "0.11.0");
    t += RemoteFile("https://archive.mozilla.org/pub/opus/opusfile-{M}.{m}.tar.gz");

    t.Public += "org.sw.demo.xiph.opus"_dep;
    t.Public += "org.sw.demo.xiph.libogg"_dep;
}
