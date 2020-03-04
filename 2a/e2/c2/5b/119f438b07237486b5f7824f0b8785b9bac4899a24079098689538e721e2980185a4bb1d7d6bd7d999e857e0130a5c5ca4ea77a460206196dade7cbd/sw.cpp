void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("gnu.shishi", "1.0.2");
    t += RemoteFile("https://ftp.gnu.org/gnu/shishi/shishi-{M}.{m}.{p}.tar.gz");

    t +=
        "lib/[^/]*\\.c"_rr,
        "lib/[^/]*\\.h"_rr;

    t.writeFileOnce(t.BinaryPrivateDir / "sys/socket.h", "");
    t.writeFileOnce(t.BinaryPrivateDir / "sys/select.h", "");
    t.writeFileOnce(t.BinaryPrivateDir / "sys/time.h", "");
    t.writeFileOnce(t.BinaryPrivateDir / "unistd.h", "");
}
