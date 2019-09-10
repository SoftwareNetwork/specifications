struct TclExecutable : ExecutableTarget
{
    void setupCommand(builder::Command &c) const override
    {
        ExecutableTarget::setupCommand(c);

        c.environment["TCL_LIBRARY"] = (SourceDir / "library").u8string();
    }
};

void build(Solution &s)
{
    auto &tcl = s.addProject("tcl", "8.6.9");
    tcl += RemoteFile("https://prdownloads.sourceforge.net/tcl/tcl{v}-src.tar.gz");

    auto &stub = tcl.addTarget<StaticLibraryTarget>("stub");
    auto &lib = tcl.addTarget<LibraryTarget>("lib");
    auto &sh = tcl.addTarget<TclExecutable>("sh");

    //
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

    //
    lib +=
        "compat/[^/]*\\.[h]"_rr,
        "generic/[^/]*\\.[hc]"_rr,
        "generic/[^/]*\\.decls"_rr,
        "libtommath/[^/]*\\.[c]"_rr,
        "libtommath/tommath_.*"_r,
        "unix/[^/]*\\.[hc]"_rr,
        "win/[^/]*\\.[hc]"_rr;
    lib -=
        ".*rc$"_rr,
        "generic/.*None.c"_rr,
        "generic/.*StubLib.c"_rr,
        "generic/regc_.*"_rr,
        "generic/rege_.*"_rr,
        "generic/regfronts.c",
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
        "libtommath"_id,
        "generic"_id
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
    }
    if (lib.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        lib.Public +=
            "Advapi32.lib"_slib,
            "Netapi32.lib"_slib,
            "user32.lib"_slib,
            "Userenv.lib"_slib,
            "ws2_32.lib"_slib
            ;
    }
    lib.Private += sw::Shared, "TCL_SHARED_BUILD"_d;
    lib.Public += sw::Static, "STATIC_BUILD"_d;

    d = lib.Public + sh;
    d->IncludeDirectoriesOnly = true;
    lib.Public += stub;
    lib.Public += "org.sw.demo.madler.zlib-1"_dep;

    if (lib.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        auto m2 = lib.BinaryDir / "tclMain1.c";
        lib.configureFile("generic/tclMain.c", m2);
        lib += m2;
        lib[m2].args.push_back("-DTCL_ASCII_MAIN=1");
        lib -= "unix/.*"_rr;
        lib -= "compat/.*"_rr;
    }
    else
        lib -= "win/.*"_rr;

    //
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
