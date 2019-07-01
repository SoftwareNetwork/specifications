void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("scrypt", "1.2.1");
    t += RemoteFile("http://www.tarsnap.com/scrypt/scrypt-{v}.tgz");
    t += "lib/crypto/.*"_rr;
}
