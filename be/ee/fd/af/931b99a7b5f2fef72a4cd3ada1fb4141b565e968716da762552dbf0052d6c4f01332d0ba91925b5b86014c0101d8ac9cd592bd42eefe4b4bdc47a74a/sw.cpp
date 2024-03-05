void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("h2o.picohttpparser", "master");
    t += Git("https://github.com/h2o/picohttpparser");
    t += "picohttpparser.*"_r;
}
