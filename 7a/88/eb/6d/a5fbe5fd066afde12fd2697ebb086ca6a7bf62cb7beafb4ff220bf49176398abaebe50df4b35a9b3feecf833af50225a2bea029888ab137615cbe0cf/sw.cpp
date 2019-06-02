void build(Solution &s)
{
    auto &libtomcrypt = s.addTarget<StaticLibraryTarget>("libtom.libtomcrypt", "1.18.2");
    libtomcrypt += Git("https://github.com/libtom/libtomcrypt", "v{v}");

    libtomcrypt += "LTC_SOURCE"_def;
    libtomcrypt.Public += "src/headers"_idir;
}
