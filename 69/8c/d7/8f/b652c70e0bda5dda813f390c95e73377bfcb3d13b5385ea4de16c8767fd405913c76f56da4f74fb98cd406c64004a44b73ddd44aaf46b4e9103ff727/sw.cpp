void build(Solution &s)
{
    auto &p = s.addProject("qtproject.qt.labs.vstools.natvis", "3.0.1");
    p += Git("https://github.com/qt-labs/vstools", "v{v}");

    auto add_natvis = [&](int v) {
        path dir = "QtVsTools.Package";
        auto s = std::to_string(v);
        auto tgt = "qt" + s;
        auto natvis = tgt + ".natvis";
        auto xml = natvis + ".xml";
        auto &t = p.addLibrary(tgt);
        t += dir / xml;
        t.configureFile(dir / xml, natvis);
        t.patch(natvis, "##NAMESPACE##", "");
        t.Public += path{natvis};
    };
    add_natvis(5);
    add_natvis(6);
}
