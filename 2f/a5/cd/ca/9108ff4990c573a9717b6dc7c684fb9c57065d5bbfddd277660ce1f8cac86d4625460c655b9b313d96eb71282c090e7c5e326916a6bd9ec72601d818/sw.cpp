void build(Solution &s)
{
    auto &p = s.addProject("mm2", "master");
    p += Git("https://github.com/mm2/Little-CMS");

    auto &t = p.addTarget<Library>("LittleCMS");
    t.Public += sw::Shared, "CMS_DLL"_def;
    t += sw::Shared, "CMS_DLL_BUILD"_def;
    t += "include/.*\\.h"_rr;
    t += "src/.*\\.[hc]"_rr;
}
