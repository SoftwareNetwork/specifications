void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("yescrypt", "1.1.0");
    t += RemoteFile("https://www.openwall.com/yescrypt/yescrypt-{v}.tar.gz");
}
