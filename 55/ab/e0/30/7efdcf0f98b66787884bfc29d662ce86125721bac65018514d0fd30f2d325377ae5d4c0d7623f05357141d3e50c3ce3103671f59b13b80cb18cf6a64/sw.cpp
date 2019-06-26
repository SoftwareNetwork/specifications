void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("BlackMATov.flat", "master");
    t += Git("https://github.com/BlackMATov/flat.hpp");
}
