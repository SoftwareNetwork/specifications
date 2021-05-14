void build(Solution &s)
{
    auto &mpfr = s.addTarget<LibraryTarget>("gnu.mpfr", "4.1.0");
    mpfr += RemoteFile("https://www.mpfr.org/mpfr-current/mpfr-{v}.tar.xz");

    mpfr.setRootDirectory("src");
    mpfr += ".*"_rr;

    mpfr -=
        "jyn_asympt.c",
        "round_raw_generic.c";

    mpfr.Private += "MPFR_TUNE_CASE=\"\""_d;
    mpfr.Public += "org.sw.demo.gnu.gmp.lib"_dep;

    if (mpfr.getBuildSettings().TargetOS.is(ArchType::x86))
        mpfr += "x86"_idir;
    else if (mpfr.getBuildSettings().TargetOS.is(ArchType::x86_64))
        mpfr += "x86_64/core2"_idir;

    if (mpfr.Variables["WORDS_BIGENDIAN"] == 1)
        mpfr += "HAVE_BIG_ENDIAN"_def;
    else
        mpfr += "HAVE_LITTLE_ENDIAN"_def;
}
