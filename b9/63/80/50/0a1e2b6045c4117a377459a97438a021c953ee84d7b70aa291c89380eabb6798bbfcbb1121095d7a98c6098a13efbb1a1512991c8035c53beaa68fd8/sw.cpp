void build(Solution &s)
{
    auto &p = s.addProject("gnu.nettle", "3.5.1");
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
            "fat-x86_64.c",
            "gcmdata.c",
            "getopt.c",
            "sha-example.c",
            "shadata.c",
            "twofishdata.c";

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

        const std::map<int, std::vector<int>> args =
        {
            { 192,{192,7,6,GMP_NUMB_BITS} },
            { 224,{224,12,6,GMP_NUMB_BITS} },
            { 256,{256,14,6,GMP_NUMB_BITS} },
            { 384,{384,41,6,GMP_NUMB_BITS} },
            { 521,{521,56,6,GMP_NUMB_BITS} },
            { 25519,{255,14,6,GMP_NUMB_BITS} },
        };

        for (auto &[k, a] : args)
        {
            auto c = nettle.addCommand();
            c << cmd::prog(eccdata);
            for (auto &a2 : a)
                c << std::to_string(a2);
            c << cmd::std_out("ecc-" + std::to_string(k) + ".h");
        }

        // install
        if (!nettle.DryRun)
        {
            auto d = nettle.BinaryDir / "nettle";
            fs::create_directories(d);
            for (auto &[p, sf] : nettle[".*\\.h"_rr])
            {
                if (File(p, nettle.getFs()).isGenerated())
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
