#pragma sw require header org.sw.demo.qtproject.qt.base.tools.moc-*

void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("jmcnamara.libxlsxwriter", "0.8.5");
    t += Git("https://github.com/jmcnamara/libxlsxwriter", "", "RELEASE_{v}");

    t.Public += "USE_SYSTEM_MINIZIP"_def;
    t += "org.sw.demo.madler.minizip"_dep;
}
