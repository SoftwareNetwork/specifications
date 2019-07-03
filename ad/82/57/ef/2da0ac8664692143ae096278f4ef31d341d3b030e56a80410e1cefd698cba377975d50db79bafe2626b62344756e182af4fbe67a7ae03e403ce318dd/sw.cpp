void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("kgabis.parson", "master");
    t += Git("https://github.com/kgabis/parson");
    t += "parson\\..*"_rr;
}
