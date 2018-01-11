void build(Solution &s)
{
    auto &t = s.add<LibraryTarget>("demo.zeux.pugixml", "1.8.1");
    t.Source = Git("https://github.com/zeux/pugixml", "v{v}");
    t.ApiName = "PUGIXML_API";
}