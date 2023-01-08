void build(Solution &s)
{
    auto &cryptopp = s.addTarget<LibraryTarget>("weidai11.cryptopp", "8.3.0");
    cryptopp += Git("https://github.com/weidai11/cryptopp", "CRYPTOPP_{M}_{m}_{p}");

    cryptopp +=
        "[^/]*\\.asm"_rr,
        "[^/]*\\.cpp"_rr,
        "[^/]*\\.[hs]"_rr;

    cryptopp -=
        "bench.*\\.cpp"_rr,
        "fipsalgt.cpp",
        ".*test.*"_rr,
        "validat.*\\.cpp"_rr
        ;

    cryptopp.Interface += sw::Shared, "CRYPTOPP_IMPORTS"_d;
    cryptopp += sw::Shared, "CRYPTOPP_EXPORTS"_d;

    // for asm
    if (cryptopp.getBuildSettings().TargetOS.is(ArchType::x86_64))
        cryptopp += "_M_X64"_def;
    else if (cryptopp.getBuildSettings().TargetOS.is(ArchType::x86))
        cryptopp += "_M_X86"_def;
    if (cryptopp.getBuildSettings().TargetOS.is(OSType::Windows))
        cryptopp += "Advapi32.lib"_slib;

    cryptopp.patch("iterhash.cpp", "#if defined(__GNUC__) || defined(__clang__)", "#if defined(__GNUC__)  || defined(__clang__) || defined(_WIN32)");
}
