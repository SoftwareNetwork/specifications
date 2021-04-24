void build(Solution &s)
{
    auto &p = s.addProject("ArtifexSoftware.thirdparty", "2.12");
    p += Git("https://github.com/ArtifexSoftware/thirdparty-lcms2", "{M}.{m}");

    auto &t = p.addTarget<Library>("lcms2");
    t.Public += sw::Shared, "CMS_DLL"_def;
    t += sw::Shared, "CMS_DLL_BUILD"_def;
    t += "include/.*\\.h"_rr;
    t += "src/.*\\.[hc]"_rr;
}
