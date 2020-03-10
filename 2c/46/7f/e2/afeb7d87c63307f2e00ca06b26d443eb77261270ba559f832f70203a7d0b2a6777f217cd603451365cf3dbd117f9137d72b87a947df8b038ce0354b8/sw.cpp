void build(Solution &s)
{
    auto &p = s.addProject("torproject.tor", "0.4.2.6");
    p += Git("https://git.torproject.org/tor.git", "tor-{v}");

    /*auto &ext = p.addLibrary("ext");
    {
        auto &t = ext;
    }*/

    auto &lib = p.addLibrary("lib");
    {
        auto &t = lib;
        t += "src/win32/.*"_rr;
        t.Public += "src/win32"_idir;
        t.Public += "src"_idir;
        t.setSourceDirectory("src/lib");
        t += ".*"_rr;

        t += "ENABLE_OPENSSL"_def;

        t += "org.sw.demo.openssl.crypto"_dep;
        t += "org.sw.demo.madler.zlib"_dep;
        t += "org.sw.demo.libevent"_dep;
        t += "org.sw.demo.coruus.keccak_tiny.unrolled-singlefile"_dep;
    }
}
