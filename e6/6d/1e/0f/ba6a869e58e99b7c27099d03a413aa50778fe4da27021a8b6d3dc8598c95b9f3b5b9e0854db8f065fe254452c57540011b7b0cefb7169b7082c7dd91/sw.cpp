void build(Solution &s)
{
    auto &mpc = s.addTarget<LibraryTarget>("gnu.mpc", "1.1.0");
    mpc += RemoteFile("https://ftp.gnu.org/gnu/mpc/mpc-{v}.tar.gz");

    mpc.setRootDirectory("src");
    mpc += ".*"_rr;
    mpc -= "logging.c";
    mpc.Public += "org.sw.demo.gnu.mpfr"_dep;
    mpc.writeFileOnce(mpc.BinaryPrivateDir / "config.h");
}
