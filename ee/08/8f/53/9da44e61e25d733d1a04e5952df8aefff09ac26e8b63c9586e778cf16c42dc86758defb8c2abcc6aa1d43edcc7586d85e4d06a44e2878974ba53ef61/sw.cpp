void build(Solution &s)
{
    // Uncomment to make a project. Also replace s.addTarget(). with p.addTarget() below.
    auto &p = s.addProject("ArtifexSoftware", "0.16.0");
    p += Git("https://github.com/ArtifexSoftware/jbig2dec", "{M}.{m}");

    auto &t = p.addStaticLibrary("jbig2dec");
    t += "config_.*"_r;
    t += "os_types.h";
    t += "jbig2[\\._].*"_r;
    t.Public += "org.sw.demo.glennrp.png"_dep;
}
