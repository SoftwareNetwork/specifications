void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("aantron.better_enums", "0.11.3");
    t += Git("https://github.com/aantron/better-enums");
}
