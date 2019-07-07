void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("thelink2012.any", "master");
    t += Git("https://github.com/thelink2012/any");
    t += "any.hpp";
}
