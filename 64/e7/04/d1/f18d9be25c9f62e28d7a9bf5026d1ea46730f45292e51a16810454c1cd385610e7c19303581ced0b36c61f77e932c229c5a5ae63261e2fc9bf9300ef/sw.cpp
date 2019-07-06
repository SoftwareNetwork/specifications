void build(Solution &s)
{
    auto &bearssl = s.addTarget<StaticLibraryTarget>("bearssl", "master");
    bearssl += Git("https://www.bearssl.org/git/BearSSL");
}
