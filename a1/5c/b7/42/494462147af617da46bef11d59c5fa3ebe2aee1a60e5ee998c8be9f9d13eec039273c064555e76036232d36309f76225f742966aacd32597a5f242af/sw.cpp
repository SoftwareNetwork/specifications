void build(Solution &s)
{
    auto &t = s.addLibrary("qtproject.qt.labs.vstools.natvis", "3.0.1");
    t += Git("https://github.com/qt-labs/vstools", "v{v}");

    auto add_natvis = [&](int v) {
        path dir = "QtVsTools.Package";
        auto s = std::to_string(v);
        auto natvis = "qt" + s + ".natvis";
        auto xml = natvis + ".xml";
        t += dir / xml;
        t.configureFile(dir / xml, natvis);
        t.patch(natvis, "##NAMESPACE##", "");
        t.Public += path{natvis};
    };
    add_natvis(5);
    add_natvis(6);
}
