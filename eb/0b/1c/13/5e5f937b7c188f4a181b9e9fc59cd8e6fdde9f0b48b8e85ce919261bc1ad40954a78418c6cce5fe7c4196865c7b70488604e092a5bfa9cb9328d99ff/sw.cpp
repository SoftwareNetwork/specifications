void build(Solution &s)
{
    auto &json = s.addTarget<LibraryTarget>("nlohmann.json", "3.11.3");
    json += Git("https://github.com/nlohmann/json", "v{v}");
    json += "include/.*"_rr;
    json += "nlohmann_json.natvis"; // we should keep it because of sw compat
    //json.Public += "nlohmann_json.natvis";

    auto &natvis = json.addTarget<LibraryTarget>("natvis");
    {
        auto &t = natvis;
        t.patch("tools/generate_natvis/nlohmann_json.natvis.j2", "m_type ==", "m_data.m_type  ==");
        t.patch("tools/generate_natvis/nlohmann_json.natvis.j2", "m_value.", "m_data.m_value .");
        t.addCommand()
            << cmd::prog("org.sw.demo.python.exe-3.10"_dep)
            << cmd::dep("org.sw.demo.python.pypi.jinja2"_dep)
            << cmd::dep("org.sw.demo.python.pypi.markupsafe"_dep)
            << cmd::wdir("tools/generate_natvis")
            << cmd::in("tools/generate_natvis/generate_natvis.py")
            << "--version"
            << t.getPackage().getVersion().toString()
            << t.BinaryDir
            << cmd::end()
            << cmd::in("tools/generate_natvis/nlohmann_json.natvis.j2")
            << cmd::out(t.BinaryDir / "nlohmann_json.natvis")
            ;
        t.Public += json;
        t.Public += t.BinaryDir / "nlohmann_json.natvis";
    }
}
