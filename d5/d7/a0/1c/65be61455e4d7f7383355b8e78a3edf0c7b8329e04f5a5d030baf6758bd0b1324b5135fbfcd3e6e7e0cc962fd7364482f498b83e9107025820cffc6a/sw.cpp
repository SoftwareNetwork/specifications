void build(Solution &s)
{
    auto &mpc = s.addTarget<LibraryTarget>("gnu.mpc", "master");
    mpc += Git("https://gforge.inria.fr/anonscm/git/mpc/mpc.git", "", "{v}");

    mpc.setRootDirectory("src");
    mpc += ".*"_rr;
    mpc -= "logging.c";
    mpc.Public += "org.sw.demo.gnu.mpfr"_dep;
    mpc.writeFileOnce("config.h");
}
