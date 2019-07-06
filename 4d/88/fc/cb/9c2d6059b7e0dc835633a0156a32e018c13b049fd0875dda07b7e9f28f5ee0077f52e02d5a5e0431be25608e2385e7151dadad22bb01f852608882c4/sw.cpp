void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("cbeck88.visit_struct", "master");
    t += Git("https://github.com/cbeck88/visit_struct");
}
