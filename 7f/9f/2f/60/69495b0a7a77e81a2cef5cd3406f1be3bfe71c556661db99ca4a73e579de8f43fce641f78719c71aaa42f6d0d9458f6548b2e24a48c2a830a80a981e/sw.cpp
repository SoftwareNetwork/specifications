#pragma sw require header org.sw.demo.qtproject.qt.base.tools.moc-*

void build(Solution &s)
{
    auto &t = s.addTarget<Library>("dbzhang800.QtXlsxWriter", "master");
    t += Git("https://github.com/dbzhang800/QtXlsxWriter", "", "{v}");

    t += "src/xlsx/.*"_r;

    t += "QT_BUILD_XLSX_LIB"_def;
    t.Public += sw::Static, "XLSX_NO_LIB"_def;

    t.Public += "org.sw.demo.qtproject.qt.base.gui-*"_dep;
    t.Public += "org.sw.demo.qtproject.qt.base.xml-*"_dep;
    automoc("org.sw.demo.qtproject.qt.base.tools.moc-*"_dep, t);
}
