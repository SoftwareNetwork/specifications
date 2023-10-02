void build(Solution &s)
{
    auto &pugixml = s.addTarget<LibraryTarget>("zeux.pugixml", "1.14.0");
    pugixml += Git("https://github.com/zeux/pugixml", "v{M}.{m}{po}");

    pugixml.ApiName = "PUGIXML_API";
    pugixml += "src/.*"_rr;
    pugixml.Public += "scripts/natvis/pugixml_compact.natvis";
}
