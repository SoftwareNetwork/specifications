void build(Solution &s)
{
    auto &p = s.addProject("ArtifexSoftware.thirdparty", "mt");
    p += Git("https://github.com/ArtifexSoftware/thirdparty-lcms2", "", "", "3ddabfa3875364172a40db633642faf711a6e33b");

    auto &t = p.addTarget<Library>("lcms2");
    t.Public += sw::Shared, "CMS_DLL"_def;
    t += sw::Shared, "CMS_DLL_BUILD"_def;
    t += "include/.*\\.h"_rr;
    t += "src/.*\\.[hc]"_rr;
    t.Public += "HAVE_LCMS2MT"_def;
}
