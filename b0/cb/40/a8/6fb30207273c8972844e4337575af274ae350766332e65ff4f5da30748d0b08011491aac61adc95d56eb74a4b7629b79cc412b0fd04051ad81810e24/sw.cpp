void build(Solution &s)
{
    auto &t = s.addLibrary("pantor.inja", "3.3.0");
    t += Git("https://github.com/pantor/inja", "v{v}");
    t.Public += "org.sw.demo.nlohmann.json"_dep;
}
