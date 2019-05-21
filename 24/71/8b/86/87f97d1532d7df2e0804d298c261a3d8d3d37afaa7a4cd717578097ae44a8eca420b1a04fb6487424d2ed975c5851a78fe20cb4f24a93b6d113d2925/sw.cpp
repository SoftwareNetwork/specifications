void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("akrzemi1.Optional", "master");
    t += Git("https://github.com/akrzemi1/Optional");
    t += "optional.hpp";
}
