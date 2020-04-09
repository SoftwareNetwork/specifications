void build(Solution &s)
{
    auto &t = s.addLibrary("qtproject.qt.labs.vstools.natvis", "dev");
    t += Git("https://github.com/qt-labs/vstools");

    t += "src/qtvstools/qt5.natvis.xml";
    t.configureFile("src/qtvstools/qt5.natvis.xml", "qt5.natvis");
    t.Public += "qt5.natvis";
}
