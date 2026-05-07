void build(Solution &s)
{
    auto &botan = s.addTarget<LibraryTarget>("randombit.botan", "3.12.0");
    botan += Git("https://github.com/randombit/botan");

    botan += cpp20;

    botan += "BOTAN_DLL"_api;

    botan -= "src/build-data/.*"_rr;

    botan.addCommand()
        << cmd::prog("org.sw.demo.python.exe"_dep)
        << cmd::in("configure.py")
        << "--with-build-dir="s + botan.BinaryDir.string()
        << "--cc=msvc" // clang clangcl emcc gcc icc msvc xcode xlc
        << "--with-boost"
        << "--with-bzip2"
        << "--with-sqlite3"
        << "--with-zlib"
        << "--with-lzma"
        << cmd::end()
        << cmd::out("build/build.h")
        ;

    botan += IncludeDirectory{botan.BinaryDir / "build"};
    botan += IncludeDirectory{botan.BinaryDir / "build/include/external"};
    botan += IncludeDirectory{botan.BinaryDir / "build/include/internal"};
    botan.Public += IncludeDirectory{botan.BinaryDir / "build/include/public"};

    botan.setRootDirectory("src/lib");
    botan += ".*"_rr;

    botan -= ".*info.txt"_rr;

    botan -= ".*aes_power8.*"_rr;
    botan -= ".*armv8.*"_rr;
    //botan -= ".*bmi2.*"_rr;
    botan -= ".*rng/esdm_rng.*"_rr;
    botan -= ".*tls_null.*"_rr;
    botan -= ".*argon2_simd64.*"_rr;
    botan -= ".*aes_vaes.*"_rr;
    botan -= ".*sha2_64_x86.*"_rr;
    botan -= ".*sm3_x86.*"_rr;
    botan -= ".*sm4_x86.*"_rr;
    botan -= ".*twofish_avx512.*"_rr;
    botan -= ".*serpent_avx512.*"_rr;
    botan -= ".*keccak_perm_bmi2.*"_rr;
    botan -= ".*prov/tpm.*"_rr;
    botan -= ".*rng/jitter_rng.*"_rr;
    // utils/cpuid/
    botan -= ".*cpuid_arm32.*"_rr;
    botan -= ".*cpuid_loongarch64.*"_rr;
    botan -= ".*cpuid_ppc.*"_rr;
    botan -= ".*cpuid_aarch64.*"_rr;
    botan -= ".*cpuid_riscv64.*"_rr;
    botan -= ".*cpuid_wasm.*"_rr;

    if (botan.getBuildSettings().TargetOS.Type != OSType::Macos)
    {
        botan -= ".*commoncrypto.*"_rr;
        botan -= ".*macos.*"_rr;
    }

    if (botan.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        botan -= ".*fd_unix.cpp"_rr;
        //botan -= ".*dev_random.cpp"_rr;
        botan -= ".*getentropy.*"_rr;
        botan -= ".*tpm.cpp"_rr;
        //botan -= ".*avx512.cpp"_rr;
        //botan -= ".*proc_walk.cpp"_rr;

        botan += "_ENABLE_EXTENDED_ALIGNED_STORAGE"_def;
    }
    botan += "BOTAN_IS_BEING_BUILT"_def;
    //botan += "BOTAN_FUTURE_INTERNAL_HEADER(x)="_def;

    botan.Public += "org.sw.demo.boost.asio"_dep;
    botan.Public += "org.sw.demo.madler.zlib"_dep;
    botan.Public += "org.sw.demo.xz_utils.lzma"_dep;
    botan.Public += "org.sw.demo.openssl.crypto"_dep;
    botan.Public += "org.sw.demo.sqlite3"_dep;
    botan.Public += "org.sw.demo.bzip2"_dep;

}
