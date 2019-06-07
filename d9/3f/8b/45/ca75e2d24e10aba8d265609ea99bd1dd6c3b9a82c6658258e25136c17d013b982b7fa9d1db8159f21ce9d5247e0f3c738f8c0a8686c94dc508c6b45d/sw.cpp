void build(Solution &s)
{
    // Uncomment to make a project. Also replace s.addTarget(). with p.addTarget() below.
    auto &p = s.addProject("mm2", "master");
    p += Git("https://github.com/mm2/Little-CMS");

    auto &t = p.addTarget<Library>("Little_CMS");
    t.Public += sw::Shared, "CMS_DLL"_def;
    t += sw::Shared, "CMS_DLL_BUILD"_def;
    t -= "src/.*\\.def"_rr;
}
