void build(Solution &s)
{
    auto &p = s.addProject("ghostscript.thirdparty", "lcms2mt");
    p += Git("git://git.ghostscript.com/thirdparty-lcms2.git");

    auto &t = p.addTarget<Library>("lcms2");
    t.Public += sw::Shared, "CMS_DLL"_def;
    t += sw::Shared, "CMS_DLL_BUILD"_def;
    t += "include/.*\\.h"_rr;
    t += "src/.*\\.[hc]"_rr;
}
