void build(Solution &s)
{
    auto &p = s.addProject("ArtifexSoftware.thirdparty", "2.17.0");
    p += Git("https://github.com/ArtifexSoftware/thirdparty-lcms2", "lcms{M}.{m}{po}");

    auto &t = p.addTarget<Library>("lcms2");
    t += cpp17;
    t.Public += sw::Shared, "CMS_DLL"_def;
    t += sw::Shared, "CMS_DLL_BUILD"_def;
    t += "include/.*\\.h"_rr;
    t += "src/.*\\.[hc]"_rr;
    t.Public += "HAVE_LCMS2MT"_def;
    t.Public += sw::Shared, "HAVE_FUNC_ATTRIBUTE_VISIBILITY"_def;
}
