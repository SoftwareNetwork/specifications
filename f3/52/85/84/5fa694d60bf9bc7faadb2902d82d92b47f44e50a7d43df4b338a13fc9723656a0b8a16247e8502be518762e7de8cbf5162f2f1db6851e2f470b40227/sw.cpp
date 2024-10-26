static void perl_dirs(auto &&f) {
    std::vector<path> paths {
        "cpan/AutoLoader/lib",
        "dist/base/lib",
        "dist/Carp/lib",
        "dist/PathTools",
        "dist/PathTools/lib",
        "cpan/ExtUtils-Install/lib",
        "cpan/ExtUtils-MakeMaker/lib",
        "cpan/ExtUtils-Manifest/lib",
        "cpan/File-Path/lib",
        "ext/re",
        "dist/Term-ReadLine/lib",
        "dist/Exporter/lib",
        "ext/File-Find/lib",
        "cpan/Text-Tabs/lib",
        "dist/constant/lib",
        "cpan/version/lib",
        "cpan/Getopt-Long/lib",
        "cpan/Text-ParseWords/lib",
        "cpan/ExtUtils-PL2Bat/lib",
        "lib",

        // win32
        "dist/ExtUtils-ParseXS/lib",
        "cpan/parent/lib",
        "cpan/ExtUtils-Constant/lib",

        // VMS
        "ext/VMS-Filespec/lib",
    };
    for (auto &&p : paths) {
        f(p);
    }
}

struct PerlExecutable : ExecutableTarget
{
    using ExecutableTarget::ExecutableTarget;

    void setupCommand(builder::Command &c) const override
    {
        std::vector<path> paths;
        perl_dirs([&](auto &&p) {paths.push_back(SourceDir / p);});
        String s;
        for (auto &&p : paths)
            s += p.string() + ";";
        s.resize(s.size() - 1);
        c.environment["PERL5LIB"] = s;

        ExecutableTarget::setupCommand(c);
    }

    void setupCommandForRun(builder::Command &c) const override
    {
        setupCommand(c);
        //c.create_new_console = true;
        //c.setInteractive(true);
    }
};

void build(Solution &s)
{
    auto &p = s.addProject("perl", "5.41.5");
    p += Git("https://github.com/Perl/perl5", "v{v}");
    //p += RemoteFile("https://github.com/Perl/perl5/archive/refs/tags/v{v}.tar.gz");

    Files base =
    {
        "av.c",
        "builtin.c",
        "caretx.c",
        "class.c",
        "deb.c",
        "doio.c",
        "doop.c",
        "dquote.c",
        "dump.c",
        "globals.c",
        "gv.c",
        "mro_core.c",
        "hv.c",
        "locale.c",
        "keywords.c",
        "mathoms.c",
        "mg.c",
        "numeric.c",
        "op.c",
        "pad.c",
        "peep.c",
        "perl.c",
        "perly.c",
        "pp.c",
        "pp_ctl.c",
        "pp_hot.c",
        "pp_pack.c",
        "pp_sort.c",
        "pp_sys.c",
        "reentr.c",
        "regcomp.c",
        "regcomp_invlist.c",
        "regcomp_study.c",
        "regcomp_trie.c",
        "regexec.c",
        "run.c",
        "scope.c",
        "sv.c",
        "taint.c",
        "time64.c",
        "toke.c",
        "universal.c",
        "utf8.c",
        "util.c",
        "perlio.c",
        "win32/win32.c",
        "win32/win32sck.c",
        "win32/win32thread.c",
        "win32/fcrypt.c",
    };

    auto &gu = p.addTarget<Executable>("generate_uudmap");
    gu += "generate_uudmap.c";
    gu += "mg_raw.h";

    auto &mp = p.addTarget<Executable>("miniperl");
    {
        const String cfg_add = R"(
#ifndef _config_h_footer_
#define _config_h_footer_
#undef Off_t
#undef LSEEKSIZE
#undef Off_t_size
#undef PTRSIZE
#undef SSize_t
#undef HAS_ATOLL
#undef HAS_STRTOLL
#undef HAS_STRTOULL
#undef Size_t_size
#undef IVTYPE
#undef UVTYPE
#undef IVSIZE
#undef UVSIZE
#undef NV_PRESERVES_UV
#undef NV_PRESERVES_UV_BITS
#undef IVdf
#undef UVuf
#undef UVof
#undef UVxf
#undef UVXf
#undef USE_64_BIT_INT
#undef USE_LONG_DOUBLE
#undef USE_CPLUSPLUS
#undef FILE_ptr
#undef FILE_cnt
#undef FILE_base
#undef FILE_bufsiz
#define FILE_ptr(fp) PERLIO_FILE_ptr(fp)
#define FILE_cnt(fp) PERLIO_FILE_cnt(fp)
#define FILE_base(fp) PERLIO_FILE_base(fp)
#define FILE_bufsiz(fp) (PERLIO_FILE_cnt(fp) + PERLIO_FILE_ptr(fp) - PERLIO_FILE_base(fp))
#define I_STDBOOL
#define Off_t __int64
#define LSEEKSIZE 8
#define Off_t_size 8
#define PTRSIZE 8
#define SSize_t __int64
#define HAS_ATOLL
#define HAS_STRTOLL
#define HAS_STRTOULL
#define Size_t_size 8
#define IVTYPE __int64
#define UVTYPE unsigned __int64
#define IVSIZE 8
#define UVSIZE 8
#undef NV_PRESERVES_UV
#define NV_PRESERVES_UV_BITS 53
#define IVdf "I64d"
#define UVuf "I64u"
#define UVof "I64o"
#define UVxf "I64x"
#define UVXf "I64X"
#define USE_64_BIT_INT
#undef USE_CPLUSPLUS
#endif
)";

        mp -= ".*\\.[hc]"_r;
        mp -= ".*\\.inc"_r;
        mp -= ".*\\.tab"_r;
        mp -= ".*\\.act"_r;
        mp -= "win32/.*"_rr;
        mp += base, "miniperlmain.c";

        mp += "."_idir;

        mp += "PERLDLL"_def;
        mp += "PERL_CORE"_def;
        mp += "PERL_EXTERNAL_GLOB"_def;
        mp += "PERL_IS_MINIPERL"_def;

        if (mp.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            mp += "win32"_idir;
            mp += "win32/include"_idir;

            mp.pushBackToFileOnce("win32/config_H.vc", cfg_add);
            mp.configureFile("win32/config_H.vc", "config.h");

            mp += "WIN32"_def;
            mp += "WIN64"_def;

            mp += "ws2_32.lib"_slib;
            mp += "user32.lib"_slib;
            mp += "Advapi32.lib"_slib;
            mp += "Comctl32.lib"_slib;

            /*mp += "_CRT_SECURE_NO_DEPRECATE"_def;
            mp += "_CRT_NONSTDC_NO_DEPRECATE"_def;
            mp += "_WINSOCK_DEPRECATED_NO_WARNINGS"_def;

            mp += "_CONSOLE"_def;
            mp += "NO_STRICT"_def;
            mp += "CONSERVATIVE"_def;*/
        }
        else
        {
            mp.pushBackToFileOnce("win32/config_H.gc", cfg_add);
            mp.configureFile("win32/config_H.gc", "config.h");
        }

        {
            auto c = mp.addCommand();
            c << cmd::prog(gu)
                << cmd::out("uudmap.h")
                << cmd::out("bitcount.h")
                << cmd::out("mg_data.h")
                ;
        }
    }

    auto &lib = p.addTarget<SharedLibrary>("lib"); // for now, sw mixes static and shared builds
    {
        (lib.Public + mp)->IncludeDirectoriesOnly = true;

        lib -= "lib/.*"_rr;
        lib -= "dist/.*"_rr;
        lib -= "ext/.*"_rr;
        lib -= "cpan/.*"_rr;

        lib -= "pod/.*"_rr;
        lib -= "regen/.*"_rr;

        lib -= "Cross/.*"_rr;
        lib -= "Porting/.*"_rr;

        lib -= ".*"_r;
        lib -= "win32/.*"_rr;
        lib += base, "win32/perllib.c";
        if (auto nsf = lib["win32/perllib.c"].as<NativeSourceFile*>())
            nsf->BuildAs = NativeSourceFile::CPP;

        lib.Protected += "."_idir;

        {
            auto c = lib.addCommand();
            c << cmd::prog(gu)
                << cmd::out("uudmap.h")
                << cmd::out("bitcount.h")
                << cmd::out("mg_data.h")
                ;
        }
        {
            auto config_sh = lib.addCommand();
            config_sh << cmd::prog(mp)
                << cmd::wdir(lib.SourceDir / "win32")
                << "-I" << lib.SourceDir / "lib"
                << cmd::in("win32/config_sh.PL")
                << "archname=MSWin32-x64-multi-thread"
                << "WIN64=define"
                << "d_mymalloc=undef"
                << "use64bitint=define"
                << "usethreads=define"
                << "useithreads=define"
                << "usemultiplicity=define"
                << "uselargefiles=define"
                << "usecplusplus="
                << "uselongdouble=undef"
                << "usesitecustomize=undef" // !
                << "default_inc_excludes_dot=define"
                << "cc=cl"
                << "ld=link"
                << "static_ext=Win32CORE"

                /*<< "ccflags=-nologo -GF -W3 -O1 -MD -Zi -DNDEBUG -GL -fp:precise -DWIN32 -D_CONSOLE -DNO_STRICT -DWIN64 -DCONSERVATIVE -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_WINSOCK_DEPRECATED_NO_WARNINGS  -DPERL_TEXTMODE_SCRIPTS -DPERL_IMPLICIT_CONTEXT -DPERL_IMPLICIT_SYS"
                << "libs=oldnames.lib kernel32.lib user32.lib gdi32.lib winspool.lib  comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib  netapi32.lib uuid.lib ws2_32.lib mpr.lib winmm.lib  version.lib odbc32.lib odbccp32.lib comctl32.lib msvcrt.lib vcruntime.lib ucrt.lib"
                << "incpath=c:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Tools\\MSVC\\14.16.27023\\include"
                << "libpth=c:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Tools\\MSVC\\14.16.27023\\lib\\x64;"
                << "libc=ucrt.lib"
                << "make=nmake"
                << "LINK_FLAGS=-nologo -nodefaultlib -debug -opt:ref,icf -ltcg -libpath:\\\"c:\\perl\\lib\\CORE\\\" -machine:AMD64"
                << "optimize=-O1 -MD -Zi -DNDEBUG -GL -fp:precise"*/

                << cmd::in("win32/config.vc")
                << cmd::std_out(lib.SourceDir / "config.sh");

            /*
            "INST_TOP=c:\\perl"
            "INST_VER="
            "INST_ARCH="
            "cf_email="
            "libperl=perl529.lib"
            */

            auto make_patchnum = lib.addCommand();
            make_patchnum << cmd::prog(mp)
                << "-I" << lib.SourceDir / "lib"
                << cmd::in("make_patchnum.pl")
                << cmd::end()
                //<< cmd::out("git_version.h")
                << cmd::out(lib.SourceDir / "lib" / "Config_git.pl")
                ;
            //make_patchnum.c->record_inputs_mtime = true;

            auto configpm = lib.addCommand();
            configpm << cmd::prog(mp)
                << "-I" << lib.SourceDir / "lib"
                << cmd::in("configpm")
                << cmd::end()
                << cmd::in(lib.SourceDir / "config.sh")
                << cmd::in(lib.SourceDir / "lib" / "Config_git.pl")
                << cmd::out(lib.SourceDir / "lib" / "Config.pod")
                << cmd::out(lib.SourceDir / "lib" / "Config.pm")
                ;

            auto buildcustomize = lib.addCommand();
            buildcustomize << cmd::prog(mp)
                << "-I" << lib.SourceDir / "lib"
                << "-f"
                << cmd::in("write_buildcustomize.pl")
                << cmd::end()
                << cmd::out(lib.SourceDir / "lib" / "buildcustomize.pl")
                ;

            /*auto config_h = lib.addCommand();
            config_h << cmd::prog(mp)
            << cmd::wdir(lib.SourceDir / "win32")
            << "-I" << lib.SourceDir / "lib"
            << cmd::in("win32/config_h.PL")
            //<< "CONFIG_H=config.h.in"
            << cmd::end()
            //<< cmd::out(lib.SourceDir / "win32" / "config.h.in")
            << cmd::out(lib.SourceDir / "win32" / "config.h")
            << cmd::in("lib/buildcustomize.pl")
            ;*/

            /*SW_MAKE_EXECUTE_BUILTIN_COMMAND_AND_ADD(copy_cmd, lib, "sw_copy_file");
            copy_cmd->args.push_back((mp.BinaryDir / "config.h").u8string());
            copy_cmd->args.push_back((lib.SourceDir / "win32" / "config.h").u8string());
            copy_cmd->addInput((mp.BinaryDir / "config.h"));
            copy_cmd->addOutput((lib.SourceDir / "win32" / "config.h"));
            copy_cmd->maybe_unused = builder::Command::MU_ALWAYS;
            lib += lib.SourceDir / "win32" / "config.h";*/

            auto perllibst = lib.addCommand();
            perllibst << cmd::prog(mp)
                << cmd::wdir(lib.BinaryDir);
            perl_dirs([&](auto &&p) {perllibst << "-I" << lib.SourceDir / p;});
            perllibst
                << cmd::in(lib.SourceDir / "win32" / "create_perllibst_h.pl")
                << cmd::end()
                << cmd::out("perllibst.h")
                << cmd::in(lib.SourceDir / "lib" / "Config.pm")
                ;

            /*auto perldll = lib.addCommand();
            perldll << cmd::prog(mp)
            << cmd::wdir(lib.SourceDir / "win32")
            << "-I" << lib.SourceDir / "lib"
            << cmd::in("makedef.pl")
            << "PLATFORM=win32"
            << "-DPERL_TEXTMODE_SCRIPTS"
            << "-DPERL_IMPLICIT_SYS"
            << "-DPERL_IMPLICIT_CONTEXT"
            << "CCTYPE=MSVC141"
            << "TARG_DIR=../"
            << cmd::std_out("perldll.def")
            << cmd::end()
            << cmd::in(lib.SourceDir / "lib/Config.pm")
            << cmd::in(lib.SourceDir / "win32" / "config.h")
            ;*/

            //PLATFORM=win32 -O1 -MD -Zi -DNDEBUG -GL -fp:precise -DWIN32 -D_CONSOLE -DNO_STRICT -DWIN64 -DCONSERVATIVE -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE -D_WINSOCK_DEPRECATED_NO_WARNINGS  -DPERL_TEXTMODE_SCRIPTS -DPERL_IMPLICIT_CONTEXT -DPERL_IMPLICIT_SYS  CCTYPE=MSVC141 TARG_DIR=..\ > perldll.def

            auto dyna = lib.addCommand();
            dyna << cmd::prog(mp)
                //<< cmd::wdir(lib.SourceDir / "win32")
                ;
            perl_dirs([&](auto &&p) {dyna << "-I" << lib.SourceDir / p;});
            dyna
                << cmd::in("dist/ExtUtils-ParseXS/lib/ExtUtils/xsubpp")
                << "-noprototypes"
                << "-typemap"
                << cmd::in("lib/ExtUtils/typemap")
                << cmd::in("ext/DynaLoader/dl_win32.xs")
                << cmd::std_out("DynaLoader.c")
                << cmd::end()
                << cmd::in(lib.SourceDir / "lib" / "Config.pm")
                //<< cmd::in(lib.SourceDir / "win32" / "config.h")
                ;
        }

        lib += sw::Shared, "PERLDLL"_def;
        lib += "PERL_CORE"_def;

        lib += "PERL_TEXTMODE_SCRIPTS"_def;
        lib += "PERL_IMPLICIT_CONTEXT"_def;
        lib += "PERL_IMPLICIT_SYS"_def;

        lib.Protected += "win32"_idir;
        lib.Protected += "win32/include"_idir;

        lib.Public += sw::Shared, "WIN32"_def;
        lib.Public += sw::Shared, "WIN64"_def;

        lib += "ws2_32.lib"_slib;
        lib += "user32.lib"_slib;
        lib += "Advapi32.lib"_slib;
        lib += "Comctl32.lib"_slib;

        lib += "ext/Win32CORE/Win32CORE.c";
        lib += "DynaLoader.c";
        lib += "ext/DynaLoader"_idir;

        //lib ^= lib.SourceDir / "git_version.h";
        /*lib.writeFileOnce("git_version.h", R"(
        #define PERL_PATCHNUM "v5.31.7"
        #define PERL_GIT_UNPUSHED_COMMITS
        )");*/
    }

    auto &perl = p.addTarget<PerlExecutable>("perl");
    {
        perl += "win32/runperl.c";
        perl.Public += lib;

        perl -= "lib/.*"_rr;
        perl -= "dist/.*"_rr;
        perl -= "ext/.*"_rr;
        perl -= "cpan/.*"_rr;
    }
}
