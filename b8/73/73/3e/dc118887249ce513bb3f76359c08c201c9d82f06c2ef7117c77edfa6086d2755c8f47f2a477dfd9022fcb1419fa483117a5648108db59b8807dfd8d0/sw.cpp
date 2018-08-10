void build(Solution &s)
{
    auto &lzo = s.addTarget<LibraryTarget>("oberhumer.lzo.lzo", "2.10.0");
    lzo += RemoteFile("https://www.oberhumer.com/opensource/lzo/download/lzo-2.10.tar.gz");
    lzo.ApiName = "__LZO_EXPORT1";
}
