void build(Solution &s)
{
    auto &p = s.addProject("gnu.nettle", "3.9.0");
    p += RemoteFile("https://ftp.gnu.org/gnu/nettle/nettle-{M}.{m}{po}.tar.gz");

    auto &eccdata = p.addTarget<ExecutableTarget>("eccdata");
    {
        eccdata.setChecks("nettle", true);

        eccdata +=
            "eccdata.c",
            "mini-gmp.c",
            "mini-gmp.h";

        eccdata -=
            "mini-gmp.c";
    }

    auto &nettle = p.addTarget<StaticLibraryTarget>("nettle");
    {
        nettle.setChecks("nettle", true);

        nettle +=
            "[^/]*\\.c"_rr,
            "[^/]*\\.h"_rr,
            "version.h.in";

        nettle -=
            "aesdata.c",
            "desdata.c",
            "eccdata.c",
            "fat-arm.c",
            "fat-arm64.c",
            "fat-x86_64.c",
            "fat-ppc.c",
            "fat-s390x.c",
            "getopt.c",
            "sha-example.c",
            "shadata.c",
            "twofishdata.c"
            ;

        nettle.Private += "UNUSED="_d;

        if (nettle.getCompilerType() == CompilerType::MSVC)
        {
            nettle.Private += "alloca=_alloca"_d;
        }

        nettle += "NETTLE_USE_MINI_GMP=1"_v;
        nettle += sw::Variable{"MAJOR_VERSION=" + std::to_string(nettle.getPackage().getVersion().getMajor())};
        nettle += sw::Variable{"MINOR_VERSION=" + std::to_string(nettle.getPackage().getVersion().getMinor())};
        const auto GMP_NUMB_BITS = nettle.Variables["SIZEOF_UNSIGNED_LONG"] * 8;
        nettle.Variables["GMP_NUMB_BITS"] = std::to_string(GMP_NUMB_BITS);
        nettle.configureFile("version.h.in", "version.h");
        nettle.configureFile("version.h.in", "nettle/version.h");
        nettle.writeFileOnce(nettle.BinaryPrivateDir / "config.h", "");
        nettle.writeFileOnce("nettle-stdint.h", "#include <stdint.h>");
        nettle.replaceInFileOnce(nettle.SourceDir / "pss-mgf1.h",
            "#include \"sha2.h\"",
            "#include \"sha2.h\"\n#include \"sha3.h\"");
        nettle.replaceInFileOnce(nettle.SourceDir / "streebog.c",
            "uint64_t Z[8] = {};",
            "uint64_t Z[8] = {0};");

        const std::map<String, std::vector<int>> args =
        {
            { "secp192r1",{8,6,GMP_NUMB_BITS} },
            { "secp224r1",{16,7,GMP_NUMB_BITS} },
            { "secp256r1",{11,6,GMP_NUMB_BITS} },
            { "secp384r1",{32,6,GMP_NUMB_BITS} },
            { "secp521r1",{44,6,GMP_NUMB_BITS} },
            { "curve25519",{11,6,GMP_NUMB_BITS} },
            { "curve448",{38,6,GMP_NUMB_BITS} },
            { "gost_gc256b",{11,6,GMP_NUMB_BITS} },
            { "gost_gc512a",{43,6,GMP_NUMB_BITS} },
        };

        for (auto &&[k, a] : args)
        {
            auto c = nettle.addCommand();
            c << cmd::prog(eccdata);
            c << k;
            for (auto &a2 : a)
                c << std::to_string(a2);
            c << cmd::std_out("ecc-" + boost::replace_all_copy(k, "_", "-") + ".h");
        }

        // install
        if (!nettle.DryRun)
        {
            auto d = nettle.BinaryDir / "nettle";
            fs::create_directories(d);
            for (auto &[p, sf] : nettle[".*\\.h"_rr])
            {
#if SW_CPP_DRIVER_API_VERSION > 1
                if (nettle.is_source_file(p))
#else
                if (File(p, nettle.getFs()).isGenerated())
#endif
                    continue;

                auto f = p;
                nettle.check_absolute(f);
                fs::copy_file(f, d / f.filename(), fs::copy_options::update_existing);
            }
        }
    }
}

void check(Checker &c)
{
    {
        auto &s = c.addSet("nettle");
        s.checkFunctionExists("getline");
        s.checkFunctionExists("secure_getenv");
        s.checkIncludeExists("dlfcn.h");
        s.checkIncludeExists("time.h");
        s.checkTypeSize("long");
        s.checkTypeSize("size_t");
        s.checkTypeSize("uid_t");
        s.checkTypeSize("unsigned long");
        s.checkTypeSize("void *");
        s.checkLibraryFunctionExists("dl", "dlopen");
        s.checkLibraryFunctionExists("gmp", "__gmpz_powm_sec");
        s.checkSourceCompiles("HAVE_TIME_WITH_SYS_TIME", R"sw_xxx(
        #include <time.h>
        #include <sys/time.h>
        int main() {return 0;}
        )sw_xxx");
    }
}
