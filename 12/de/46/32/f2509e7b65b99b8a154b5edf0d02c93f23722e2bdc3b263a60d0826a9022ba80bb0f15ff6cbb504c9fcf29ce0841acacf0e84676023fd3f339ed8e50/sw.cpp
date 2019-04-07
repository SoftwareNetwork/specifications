void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("therocode.meta_enum", "master");
    t += Git("https://github.com/therocode/meta_enum");
}
