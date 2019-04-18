void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("foonathan.type_safe", "0.2.1");
    t += Git("https://github.com/foonathan/type_safe", "v{v}");
}
