struct TclExecutable : ExecutableTarget
{
    using ExecutableTarget::ExecutableTarget;

    void setupCommand(builder::Command &c) const override
    {
        ExecutableTarget::setupCommand(c);

        c.environment["TCL_LIBRARY"] = to_printable_string(SourceDir / "library");
    }
};

void build(Solution &s)
{
    auto &tcl = s.addProject("tcl", "8.6.11");
    //tcl += Git("https://github.com/tcltk/tcl", "core-{M}-{m}-{p}");
    tcl += RemoteFile("https://prdownloads.sourceforge.net/tcl/tcl{v}-src.tar.gz");

    auto &stub = tcl.addTarget<StaticLibraryTarget>("stub");
    auto &lib = tcl.addTarget<LibraryTarget>("lib");
    auto &sh = tcl.addTarget<TclExecutable>("sh");

    // stub
    {
        stub +=
            "generic/tclOOStubLib.c",
            "generic/tclStubLib.c",
            "generic/tclTomMathStubLib.c";
        stub.Private += sw::Static, "STATIC_BUILD"_d;
        stub.Private += sw::Shared, "TCL_SHARED_BUILD"_d;
        auto d = stub.Public + sh;
        d->IncludeDirectoriesOnly = true;
        d = stub.Public + lib;
        d->IncludeDirectoriesOnly = true;
    }

    // lib
    {
        lib +=
            "compat/[^/]*\\.[h]"_rr,
            "generic/[^/]*\\.[hc]"_rr,
            "generic/[^/]*\\.decls"_rr,
            "libtommath/.*\\.[hc]"_rr,
            "unix/[^/]*\\.[hc]"_rr,
            "win/[^/]*\\.[hc]"_rr;
        lib -=
            ".*rc$"_rr,
            "generic/.*None.c"_rr,
            "generic/.*StubLib.c"_rr,
            "generic/regc_.*"_rr,
            "generic/rege_.*"_rr,
            "generic/regfronts.c",
            "libtommath/bn_deprecated.c",
            "libtommath/bn_mp_iseven.c",
            "libtommath/bn_mp_init_i.*"_rr,
            "libtommath/bn_mp_init_l.*"_rr,
            "libtommath/bn_mp_init_u.*"_rr,
            "libtommath/bn_mp_set_i.*"_rr,
            "libtommath/bn_mp_set_l.*"_rr,
            "libtommath/bn_mp_set_u.*"_rr,
            "libtommath/bn_mp_exptmod.c",
            "libtommath/bn_mp_get_mag_ull.c",
            "libtommath/bn_s_mp_exptmod.*"_rr,
            "libtommath/bn_.*prime.*"_rr,
            "unix/tclAppInit.c",
            "unix/tclLoadAix.c",
            "unix/tclLoadDyld.c",
            "unix/tclLoadNext.c",
            "unix/tclLoadOSF.c",
            "unix/tclLoadShl.c",
            "unix/tclXtNotify.c",
            "unix/tclXtTest.c",
            "win/nmakehlp.c",
            "win/tclAppInit.c",
            "win/tclWinDde.c",
            "win/tclWinReg.c";
        lib.Public +=
            "generic"_id,
            "libtommath"_id
            ;
        lib.Private += "BUILD_tcl"_d;
        lib.Private += "CFG_INSTALL_BINDIR=\"\""_d;
        lib.Private += "CFG_INSTALL_DOCDIR=\"\""_d;
        lib.Private += "CFG_INSTALL_INCDIR=\"\""_d;
        lib.Private += "CFG_INSTALL_LIBDIR=\"\""_d;
        lib.Private += "CFG_INSTALL_SCRDIR=\"\""_d;
        lib.Private += "CFG_RUNTIME_BINDIR=\"\""_d;
        lib.Private += "CFG_RUNTIME_DOCDIR=\"\""_d;
        lib.Private += "CFG_RUNTIME_INCDIR=\"\""_d;
        lib.Private += "CFG_RUNTIME_LIBDIR=\"\""_d;
        lib.Private += "CFG_RUNTIME_SCRDIR=\"\""_d;
        lib.Private += "HAVE_ZLIB"_d;
        lib.Private += "TCL_CFGVAL_ENCODING=\"\""_d;
        lib.Private += "TCL_TOMMATH"_d;
        if (lib.getBuildSettings().TargetOS.Type != OSType::Windows)
        {
            lib.Public += "TCL_LIBRARY=\"\""_d;
            lib.Public += "TCL_PACKAGE_PATH=\"\""_d;
        }
        if (lib.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            lib.Public += "UNICODE"_d;
            lib.Public += "_UNICODE"_d;

            for (auto p : {
                "if ((err != MP_OKAY) && MP_HAS(S_READ_ARC4RANDOM))",
                "if ((err != MP_OKAY) && MP_HAS(S_READ_GETRANDOM))",
                "if ((err != MP_OKAY) && MP_HAS(S_READ_URANDOM))",
                "if ((err != MP_OKAY) && MP_HAS(S_READ_LTM_RNG))",
            })
                lib.patch("libtommath/bn_s_mp_rand_platform.c", p, "//"s + p);
        }
        if (lib.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            lib +=
                "Advapi32.lib"_slib,
                "Netapi32.lib"_slib,
                "user32.lib"_slib,
                "Userenv.lib"_slib,
                "ws2_32.lib"_slib
                ;
        }
        else
        {
            lib += "HAVE_UNISTD_H"_def;
            lib += "dl"_slib;
        }
        lib.Private += sw::Shared, "TCL_SHARED_BUILD"_d;
        lib.Public += sw::Static, "STATIC_BUILD"_d;

        auto d = lib.Public + sh;
        d->IncludeDirectoriesOnly = true;
        lib.Public += stub;
        lib.Public += "org.sw.demo.madler.zlib"_dep;

        if (lib.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            lib -= "unix/.*"_rr;
            lib -= "compat/.*"_rr;
        }
        else
            lib -= "win/.*"_rr;

        lib.patch("generic/tclPort.h", "#include \"tcl.h\"", "#include \"tcl.h\"\n#include <stdint.h>");
    }

    // sh
    {
        sh +=
            "library/.*"_rr,
            "unix/.*\\.h"_rr,
            "unix/tclAppInit.c",
            "win/.*\\.h"_rr,
            "win/tclAppInit.c";
        sh -=
            ".*rc$"_rr;
        sh.Public +=
            "unix"_id,
            "win"_id;
        if (sh.getBuildSettings().TargetOS.is(OSType::Windows))
            sh -= "unix/.*"_rr;
        else
            sh -= "win/.*"_rr;
        sh.Public += lib;
    }
}
