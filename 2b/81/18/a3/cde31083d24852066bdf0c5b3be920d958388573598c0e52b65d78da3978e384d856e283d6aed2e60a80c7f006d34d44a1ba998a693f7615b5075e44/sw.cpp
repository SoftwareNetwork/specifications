void build(Solution &s)
{
    auto &p = s.addProject("iamantony", "1.5.0");
    p += Git("https://github.com/iamantony/qtcsv", "v{M}.{m}{po}");

    auto &t = p.addTarget<Library>("qtcsv");
    t += "."_id;
    t.Public += "include"_id;
    t.Public += "org.sw.demo.qtproject.qt.base.core"_dep;
    t.Public += sw::Static, "QTCSV_STATIC_LIB"_def;
    t += "QTCSV_LIBRARY"_def;
}
