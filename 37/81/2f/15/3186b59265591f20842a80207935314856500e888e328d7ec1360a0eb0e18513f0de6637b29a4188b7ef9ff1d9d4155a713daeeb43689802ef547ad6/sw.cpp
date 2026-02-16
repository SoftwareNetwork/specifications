static std::vector<path> &perl_dirs1() {
    static std::vector<path> paths {
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

        // openssl
        "ext/DynaLoader",
        "dist/XSLoader",
        "cpan/Pod-Usage/lib",
        "cpan/podlators/lib",
        "cpan/Pod-Simple/lib",
        "cpan/Pod-Escapes/lib",
        "dist/if",
        "cpan/Encode",
        "cpan/Encode/lib",
        "dist/Storable/lib",
        "ext/Fcntl",
        "dist/FindBin/lib",
        "cpan/Win32",
        "ext/File",
        "ext/File-Glob",
        "cpan/IPC-Cmd/lib",
        "cpan/Params-Check/lib",
        "cpan/Locale-Maketext-Simple/lib",
        "cpan/Module-Load/lib",
        "cpan/Module-Load-Conditional/lib",
        "dist/IO",
        "dist/IO/lib",
        "cpan/Module-Metadata/lib",
        "ext/POSIX",
        "ext/POSIX/lib",
    };
    return paths;
}
static void perl_dirs(auto &&f) {
    for (auto &&p : perl_dirs1()) {
        f(p);
    }
}

struct PerlExecutable : ExecutableTarget
{
    using ExecutableTarget::ExecutableTarget;

    path libsdir;
    std::vector<path> extra_paths;

    void setupCommand(builder::Command &c) const override
    {
        c.use_response_files = false;

        std::vector<path> paths;
        paths.push_back(libsdir / "lib");
        perl_dirs([&](auto &&p) {
            paths.push_back(SourceDir / p);
            // we add binary dir manually when making modules
        });
        String s;
        for (auto &&p : paths)
            s += p.string() + ";";
        for (auto &&p : extra_paths)
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
    auto &p = s.addProject("perl", "5.43.7");
    p += Git("https://github.com/Perl/perl5", "v{v}");
    auto &packages = p.addDirectory("packages");
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

    using lib_build_type = SharedLibrary;

    auto &perl = p.addTarget<PerlExecutable>("perl");
    auto &lib = p.addTarget<lib_build_type>("lib"); // for now, sw mixes static and shared builds

    auto copy_file = [](auto &&from, auto &&to) {
        //std::error_code ec;
        fs::copy_file(from, to, fs::copy_options::update_existing);
    };
    auto fix_perl_path_old2 = [&](auto &c) {
        c << "-I" << lib.BinaryDir;
        c << "-I" << lib.BinaryDir / "lib";
    };
    auto fix_perl_path_old = [&](auto &c) {
        perl_dirs([&](auto &&p) {
            c << "-I" << lib.SourceDir / p;
            c << "-I" << lib.BinaryDir / p;
        });
    };
    auto fix_perl_path = [&](auto &c) {
        c << cmd::in(lib.BinaryDir / "lib/buildcustomize.pl");
    };
    auto xsubpp = [&](auto &t, auto &&fn, auto &&typemaps, const Files &depfiles = {}) {
        path out = fn;
        if (!out.is_absolute()) {
            out = t.BinaryDir / fn;
        }
        out += ".c";
        auto c = t.addCommand();
        c << cmd::prog(mp);
        path p{fn};
        //c << cmd::wdir(p.is_absolute() ? p.parent_path() : t.SourceDir / p.parent_path());
        fix_perl_path_old2(c);
        fix_perl_path_old(c);
        c
            // add lib dep, use lib.getFile()?
            << cmd::in(lib.SourceDir / "dist/ExtUtils-ParseXS/lib/ExtUtils/xsubpp")
            << "-noprototypes"
            ;
        for (auto &&tm : typemaps) {
            c << "-typemap" << tm;
        }
        c
            << cmd::in(fn)
            << cmd::std_out(out)
            ;
        c << cmd::end();
        for (auto &&f : depfiles) {
            c << cmd::in(f);
        }
        //lib -= out;
        t += out; // how this works with sw upload?
        return out;
    };

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

        lib.Protected += "win32"_idir;
        lib.Protected += "win32/include"_idir;

        lib += "PERL_CALLCONV"_api;

        lib.Public += "WIN32"_def;
        lib.Public += "WIN64"_def;

        lib += "ws2_32.lib"_slib;
        lib += "user32.lib"_slib;
        lib += "Advapi32.lib"_slib;
        lib += "Comctl32.lib"_slib;

        lib += "ext/Win32CORE/Win32CORE.c";

        lib.Public += sw::Static, "PERL_STATIC_SYMS"_def;
        lib += sw::Shared, "PERLDLL"_def;
        lib += "PERL_CORE"_def;

        // some conf
        lib.Public += "PERL_TEXTMODE_SCRIPTS"_def;
        lib.Public += "PERL_IMPLICIT_SYS"_def;
        // comment out?
        //lib.Public += "PERL_IMPLICIT_CONTEXT"_def; // but PERL_IMPLICIT_CONTEXT was removed from core perl. It does not do anything. Undeffing it for compilation
        lib.Public += "MULTIPLICITY"_def; // needed for PERL_IMPLICIT_CONTEXT
        // we need both
        lib.Public += "USE_ITHREADS"_def; // same as USE_THREADS? looks like no
        lib.Public += "USE_THREADS"_def;
        //

        // for dynamic xsubpp modules
        lib.Public += "USE_DYNAMIC_LOADING"_def;
        // for noreturn dllexport functions
        lib.patch("win32/win32.h", "#      define PERL_CALLCONV_NO_RET", "//#       define PERL_CALLCONV_NO_RET");
        // for linking xsubpp modules to perl.dll
        lib.patch("INTERN.h", "defined(WIN32) && defined(__MINGW32__)", "(defined(WIN32) || defined(__MINGW32__))");
        // for xsubpp modules exports
        lib.patch("XSUB.h", "defined(__CYGWIN__)", "(defined( __CYGWIN__) || defined(WIN32))");
        //
        lib.patch("cpan/IPC-Cmd/lib/IPC/Cmd.pm", "WNOHANG", "WNOHANG()");
        lib.patch("cpan/IPC-Cmd/lib/IPC/Cmd.pm", "_exit 1", "_exit(1)");

        {
            auto c = lib.addCommand();
            c << cmd::prog(gu)
                << cmd::out("uudmap.h")
                << cmd::out("bitcount.h")
                << cmd::out("mg_data.h")
                ;
        }
        // generate things
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

                << cmd::in("win32/config.vc");
                ;
            auto config_sh_idir = lib.BinaryDir;
            config_sh << cmd::std_out(config_sh_idir / "config.sh");

            /*
            "INST_TOP=c:\\perl"
            "INST_VER="
            "INST_ARCH="
            "cf_email="
            "libperl=perl529.lib"
            */

            auto make_patchnum_pl = lib.BinaryDir / "make_patchnum.pl";
            auto config_git_pl = lib.BinaryDir / "Config_git.pl";
            //lib.addCommand(SW_VISIBLE_BUILTIN_FUNCTION(copy_file)) << cmd::in(lib.Source / "make_patchnum.pl") << cmd::out(make_patchnum_pl);
            if (!lib.DryRun) {
                fs::create_directories(lib.BinaryDir / "lib");
                copy_file(lib.SourceDir / "make_patchnum.pl", make_patchnum_pl);

                // out to .in.h and copy after?
                lib.patch(make_patchnum_pl, "git_version.h", normalize_path((lib.BinaryDir / "git_version.h").lexically_relative(lib.SourceDir)).string());
                // was lib/Config_git.pl
                lib.patch(make_patchnum_pl, "lib/Config_git.pl", normalize_path((config_git_pl).lexically_relative(lib.SourceDir)).string());
            }

            auto make_patchnum = lib.addCommand();
            make_patchnum << cmd::prog(mp)
                << "-I" << lib.SourceDir / "lib"
                << cmd::in(make_patchnum_pl)
                << cmd::end()
                << cmd::out("git_version.h")
                << cmd::out(config_git_pl)
                ;

            if (!lib.DryRun) {
                copy_file(lib.SourceDir / "myconfig.SH", lib.BinaryDir / "myconfig.SH");
                fs::create_directories(lib.BinaryDir / "Porting");
                copy_file(lib.SourceDir / "Porting/Glossary", lib.BinaryDir / "Porting/Glossary");
            }

            auto config_pm = lib.BinaryDir / "lib" / "Config.pm";

            auto configpm = lib.addCommand();
            configpm << cmd::prog(mp)
                << cmd::wdir(lib.BinaryDir)
                << "-I" << lib.SourceDir
                << "-I" << lib.SourceDir / "lib"
                << cmd::in("configpm")
                << cmd::end()
                << cmd::in(config_sh_idir / "config.sh")
                << cmd::in(config_git_pl)
                << cmd::out(lib.BinaryDir / "lib" / "Config.pod")
                << cmd::out(config_pm)
                ;

            if (!lib.DryRun) {
                copy_file(lib.SourceDir / "write_buildcustomize.pl", lib.BinaryDir / "write_buildcustomize.pl");
                lib.patch(lib.BinaryDir / "write_buildcustomize.pl", "my $file = 'lib/buildcustomize.pl';", std::format("my $file = '{}/lib/buildcustomize.pl';", normalize_path(lib.BinaryDir).string()));
            }
            auto buildcustomize = lib.addCommand();
            buildcustomize << cmd::prog(mp)
                << "-I" << lib.SourceDir / "lib"
                << "-f"
                << cmd::in(lib.BinaryDir / "write_buildcustomize.pl")
                << cmd::end()
                << cmd::out("lib/buildcustomize.pl")
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
            fix_perl_path_old2(perllibst);
            fix_perl_path_old(perllibst);
            perllibst
                //<< cmd::in(lib.BinaryDir / "lib/buildcustomize.pl")
                << cmd::in(lib.SourceDir / "win32" / "create_perllibst_h.pl")
                << cmd::end()
                << cmd::out("perllibst.h")
                << cmd::in(config_pm)
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

            {
            auto dyna = lib.addCommand();
            dyna << cmd::prog(mp)
                //<< cmd::wdir(lib.SourceDir / "win32")
                ;
            fix_perl_path_old2(dyna);
            fix_perl_path_old(dyna);
            dyna
                << cmd::in("dist/ExtUtils-ParseXS/lib/ExtUtils/xsubpp")
                << "-noprototypes"
                << "-typemap"
                << cmd::in("lib/ExtUtils/typemap")
                << cmd::in("ext/DynaLoader/dl_win32.xs")
                << cmd::std_out("DynaLoader.c")
                << cmd::end()
                << cmd::in(config_pm)
                //<< cmd::in(lib.SourceDir / "win32" / "config.h")
                ;
            lib += "DynaLoader.c";
            lib += "ext/DynaLoader"_idir;
            }

            auto copy_and_patch = [&](auto &&t, auto &&fn, auto &&from, auto &&to) {
                path in = fn;
                path out = fn;
                if (!in.is_absolute()) {
                    in = t.SourceDir / fn;
                }
                if (!out.is_absolute()) {
                    out = t.BinaryDir / fn;
                }
                t -= in;
                if (t.DryRun)
                    return out;
                fs::create_directories(out.parent_path());
                copy_file(in, out);
                auto patch_fn = out;
                patch_fn += ".patch";
                //if (!fs::exists(patch_fn)) {
                    t.patch(out, from, to);
                    //write_file(patch_fn, "");
                //}
                return out;
            };
            auto fix_local_require = [&](auto &&file, auto &&outfn) {
                return copy_and_patch(lib, file, "./"s + path{outfn}.filename().string(), ""s + normalize_path(outfn).string());
            };
            auto fix_local_output = [&](auto &&file, auto &&outfn) {
                //return copy_and_patch(lib, file, "\">"s + path{outfn}.filename().string(), "\">"s + out.string());
            };

            perl.extra_paths.push_back(lib.SourceDir / "lib");
            perl.extra_paths.push_back(lib.BinaryDir);
            perl.extra_paths.push_back(lib.BinaryDir / "lib");
            //
            perl.extra_paths.push_back(lib.BinaryDir / "ext");
            perl.extra_paths.push_back(lib.BinaryDir / "dist");
            perl.extra_paths.push_back(lib.BinaryDir / "cpan");
            // some modules depends on previous ones
            // we did not keep tracking of it yet
            auto PL_to_file = [&](auto &&fn) {
                auto in = fn;
                auto f = path{in}.string();
                while (!f.empty() && f.back() != '.') f.pop_back();
                f.pop_back();
                for (auto i = f.size() - 1; i != -1; --i) {
                    if (f[i] == '_') {
                        f[i] = '.';
                        break;
                    }
                }
                auto outcopy = fn;
                path out = f;
                //copy_and_patch(lib, outcopy, "\">"s + path{out}.filename().string(), "\">"s + normalize_path(lib.BinaryDir / out).string());
                copy_and_patch(lib, outcopy, ""s + path{out}.filename().string(), ""s + normalize_path(lib.BinaryDir / out).string());
                if (lib.DryRun)
                    return out;
                auto c = lib.addCommand()
                    << cmd::prog(mp)
                    << cmd::wdir((lib.SourceDir / in).parent_path())
                    << "-I" << lib.SourceDir / "lib"
                    << "-I" << lib.BinaryDir
                    << "-I" << lib.BinaryDir / "lib"
                    ;
                fix_perl_path_old2(c);
                fix_perl_path_old(c);
                c
                    << cmd::in(lib.BinaryDir / outcopy)
                    << cmd::out(lib.BinaryDir / out)
                    << cmd::end()
                    << cmd::in(config_pm)
                    ;
                //fix_and_add_output_file(c, "dist/Devel-PPPort/RealPPPort_xs.PL", "dist/Devel-PPPort/RealPPPort.xs");
                perl.extra_paths.push_back((lib.BinaryDir / out).parent_path());
                return lib.BinaryDir / out;
            };
            auto make_module_simple = [&](auto &&disposition, auto &&name) {
                PL_to_file(std::format("{0}/{1}/{1}_pm.PL", disposition, name));
            };
            auto make_module_simple1 = [&](const path &name) {
                make_module_simple(name.parent_path().string(), name.filename().string());
            };
            make_module_simple1("dist/XSLoader");
            lib.patch(lib.SourceDir / "ext/DynaLoader/DynaLoader_pm.PL", "croak(\"Can't locate", "$file = dl_findfile_sw($modfname) unless $file; croak( \"Can't locate");
            lib.patch(lib.SourceDir / "ext/DynaLoader/DynaLoader_pm.PL", "sub dl_findfile {", std::format(R"(
sub dl_findfile_sw {{
    my $output = @_[0];
    my $output = qx({} list org.sw.demo.perl.packages.*.$output 2>&1);
    my @output = split /\n/, $output;
    my @output = split / /, $output;
    $output = "$output[0]-$output[1]";
    return dl_findfile(map("-L$_",@dirs,@INC), $output);
}}

sub dl_findfile {{
            )", normalize_string_copy(sw::getSwExecutableName().string())));
            make_module_simple1("ext/DynaLoader");
            make_module_simple1("dist/lib");

            // for modules
            {
                auto RealPPPort_xs = PL_to_file("dist/Devel-PPPort/RealPPPort_xs.PL");
                auto pport_pm = PL_to_file("dist/Devel-PPPort/PPPort_pm.PL");
                {
                    auto ppport_pl = copy_and_patch(lib, "dist/Devel-PPPort/ppport_h.PL", "ppport.h", normalize_path(lib.BinaryDir / "ppport.h").string());

                    auto ppport = lib.addCommand();
                    ppport << cmd::prog(mp)
                        << "-I" << lib.SourceDir / "lib"
                        << "-I" << pport_pm.parent_path()
                        << cmd::in(ppport_pl)
                        << cmd::out("ppport.h")
                        << cmd::end()
                        << cmd::in(pport_pm)
                        ;
                }

                auto &pp = packages.addTarget<lib_build_type>("dist.Devel.PPPort");
                {
                    auto &t = pp;
                    t += lib;
                    //t += "PERL_EXT_RE_BUILD"_def;
                    xsubpp(t, RealPPPort_xs, std::vector<path>{
                        lib.SourceDir / "lib/ExtUtils/typemap",
                        lib.SourceDir / "dist/Devel-PPPort/typemap",
                    });
                    //t.addCommand(SW_VISIBLE_BUILTIN_FUNCTION(copy_file)) << cmd::in(out) << cmd::out(t.BinaryDir / out.filename() += ".c");
                    t += "dist/Devel-PPPort/.*\\.c"_r;
                }
            }
        }
    }

    {
        perl.libsdir = lib.SourceDir;

        perl += "win32/runperl.c";
        perl.Public += lib;

        perl -= "lib/.*"_rr;
        perl -= "dist/.*"_rr;
        perl -= "ext/.*"_rr;
        perl -= "cpan/.*"_rr;
    }

    auto &enc = packages.addTarget<lib_build_type>("cpan.Encode");
    {
        auto &t = enc;
        t += lib;
        t += "cpan/Encode/.*\\.c"_r;
        t += "cpan/Encode/Encode"_idir;
        t.writeFileOnce("def_t.fnm", R"(
ucm/ascii.ucm
ucm/8859-1.ucm
ucm/cp1252.ucm
ucm/null.ucm
ucm/ctrl.ucm
)");

        auto c = t.addCommand();
        c
            << cmd::prog(mp)
            << cmd::wdir("cpan/Encode")
            << "-I" << lib.SourceDir / "lib"
            << "-I" << lib.BinaryDir
            << "-I" << lib.BinaryDir / "lib"
            ;
        fix_perl_path_old2(c);
        fix_perl_path_old(c);
        c
            << "bin/enc2xs"
            << "-\"Q\""
            << "-\"O\""
            << "-o"
            << cmd::out(normalize_path(t.BinaryDir / "def_t.c"))
            << "-f" << normalize_path(t.BinaryDir / "def_t.fnm")
            ;

        xsubpp(t, "cpan/Encode/Encode.xs", std::vector<path>{
            lib.SourceDir / "lib/ExtUtils/typemap",
        });
    }

    auto &cwd = packages.addTarget<lib_build_type>("dist.PathTools.Cwd");
    {
        auto &t = cwd;
        t += lib;
        t += "DOUBLE_SLASHES_SPECIAL=0"_def;

        xsubpp(t, "dist/PathTools/Cwd.xs", std::vector<path>{
            lib.SourceDir / "lib/ExtUtils/typemap",
        });
    }

    auto &storable = packages.addTarget<lib_build_type>("dist.Storable");
    {
        auto &t = storable;
        t += lib;
        t += "VERSION=\"3.39\""_def;
        t += "XS_VERSION=\"3.39\""_def;

        xsubpp(t, "dist/Storable/Storable.xs", std::vector<path>{
            lib.SourceDir / "lib/ExtUtils/typemap",
        });
    }

    auto &fcntl = packages.addTarget<lib_build_type>("ext.Fcntl");
    {
        auto &t = fcntl;
        t += lib;
        t += "VERSION=\"1.20\""_def;
        t += "XS_VERSION=\"1.20\""_def;
        t += IncludeDirectory{lib.BinaryDir / "ext/Fcntl"};

        if (!lib.DryRun) {
            fs::create_directories(lib.BinaryDir / "ext/Fcntl");
            copy_file(lib.SourceDir / "ext/Fcntl/Fcntl.pm", lib.BinaryDir / "ext/Fcntl/Fcntl.pm");
            copy_file(lib.SourceDir / "ext/Fcntl/Fcntl.xs", lib.BinaryDir / "ext/Fcntl/Fcntl.xs");
        }
        auto c = lib.addCommand();
        c
            << cmd::prog(mp)
            << cmd::wdir(lib.BinaryDir / "ext/Fcntl")
            << "-I" << lib.SourceDir / "lib"
            << "-I" << lib.BinaryDir
            << "-I" << lib.BinaryDir / "lib"
            ;
        fix_perl_path_old2(c);
        fix_perl_path_old(c);
        c
            << cmd::in("ext/Fcntl/Makefile.PL")
            << cmd::end()
            << cmd::out("ext/Fcntl/const-xs.inc")
            << cmd::out("ext/Fcntl/const-c.inc")
            ;
        //lib -= "ext/Fcntl/const-xs.inc";
        //lib -= "ext/Fcntl/const-c.inc";

        xsubpp(t, lib.BinaryDir / "ext/Fcntl/Fcntl.xs", std::vector<path>{
            lib.SourceDir / "lib/ExtUtils/typemap",
        }
        //, Files{lib.BinaryDir / "ext/Fcntl/const-xs.inc", lib.BinaryDir / "ext/Fcntl/const-c.inc"}
        );
    }

    auto &win32 = packages.addTarget<lib_build_type>("cpan.Win32");
    {
        auto &t = win32;
        t += lib;
        t += IncludeDirectory{lib.SourceDir / "cpan/Win32"};
        t += IncludeDirectory{lib.BinaryDir / "cpan/Win32"};
        t += "advapi32.lib"_slib;
        t += "User32.lib"_slib;
        t += "Winhttp.lib"_slib;
        t += "Version.lib"_slib;
        t += "ole32.lib"_slib;
        t += "Netapi32.lib"_slib;
        t += "Userenv.lib"_slib;
        t += "Shell32.lib"_slib;

        if (!lib.DryRun) {
            //fs::create_directories(lib.BinaryDir / "cpan/Win32");
            //copy_file(lib.SourceDir / "cpan/Win32/longpath.inc", lib.BinaryDir / "cpan/Win32/longpath.inc");
        }

        xsubpp(t, "cpan/Win32/Win32.xs", std::vector<path>{
            lib.SourceDir / "lib/ExtUtils/typemap",
        });
    }

    auto &file_glob = packages.addTarget<lib_build_type>("ext.File.Glob");
    {
        auto &t = file_glob;
        t += lib;
        t += "ext/File-Glob/bsd_glob.c";
        t += IncludeDirectory{lib.SourceDir / "ext/File-Glob"};
        t += IncludeDirectory{lib.BinaryDir / "ext/File-Glob"};

        if (!lib.DryRun) {
            fs::create_directories(lib.BinaryDir / "ext/File-Glob");
            copy_file(lib.SourceDir / "ext/File-Glob/Glob.pm", lib.BinaryDir / "ext/File-Glob/Glob.pm");
            copy_file(lib.SourceDir / "ext/File-Glob/Glob.xs", lib.BinaryDir / "ext/File-Glob/Glob.xs");

            fs::create_directories(lib.BinaryDir / "ext/File");
            copy_file(lib.SourceDir / "ext/File-Glob/Glob.pm", lib.BinaryDir / "ext/File/Glob.pm");
        }

        auto c = lib.addCommand();
        c
            << cmd::prog(mp)
            << cmd::wdir(lib.BinaryDir / "ext/File-Glob")
            << "-I" << lib.SourceDir / "lib"
            << "-I" << lib.BinaryDir
            << "-I" << lib.BinaryDir / "lib"
            << "-I" << lib.SourceDir / "ext/File-Glob"
            << "-I" << lib.BinaryDir / "ext/File-Glob"
            ;
        fix_perl_path_old2(c);
        fix_perl_path_old(c);
        c
            << cmd::in("ext/File-Glob/Makefile.PL")
            << cmd::end()
            << cmd::out("ext/File-Glob/const-xs.inc")
            << cmd::out("ext/File-Glob/const-c.inc")
            ;

        xsubpp(t, lib.BinaryDir / "ext/File-Glob/Glob.xs", std::vector<path>{
            lib.SourceDir / "lib/ExtUtils/typemap",
        });
    }

    auto process_module = [&](auto &t, const path &dir) {

        t += IncludeDirectory{lib.SourceDir / dir};
        t += IncludeDirectory{lib.BinaryDir / dir};

        auto s = dir.filename().string();
        auto p = s.rfind('-');
        auto has_dash = p == -1;
        auto fn = has_dash ? s : s.substr(p + 1);
        auto prefix = s.substr(0, p);

        if (!lib.DryRun) {
            fs::create_directories(lib.BinaryDir / dir);
            auto pm = lib.SourceDir / dir / (fn + ".pm"s);
            auto has_pm = fs::exists(pm);
            if (has_pm) {
                copy_file(pm, lib.BinaryDir / dir / (fn + ".pm"s));
            }
            copy_file(lib.SourceDir / dir / (fn + ".xs"s), lib.BinaryDir / dir / (fn + ".xs"s));

            if (has_dash) {
                fs::create_directories(lib.BinaryDir / prefix);
                if (has_pm) {
                    copy_file(pm, lib.BinaryDir / prefix / (fn + ".pm"s));
                }
            }
        }

        if (!lib.DryRun && fs::exists(lib.SourceDir / dir / "Makefile.PL") && read_file(lib.SourceDir / dir / "Makefile.PL").contains("WriteConstants"sv)) {
            if (fs::exists(lib.SourceDir / dir / "lib")) {
                fs::copy(lib.SourceDir / dir / "lib", lib.BinaryDir / dir / "lib", fs::copy_options::recursive | fs::copy_options::update_existing);
            }
            auto c = lib.addCommand();
            c
                << cmd::prog(mp)
                << cmd::wdir(lib.BinaryDir / dir)
                << "-I" << lib.SourceDir / "lib"
                << "-I" << lib.BinaryDir
                << "-I" << lib.BinaryDir / "lib"
                << "-I" << lib.SourceDir / dir
                << "-I" << lib.SourceDir / dir / "lib"
                << "-I" << lib.BinaryDir / dir
                ;
            fix_perl_path_old2(c);
            fix_perl_path_old(c);
            c
                << cmd::in(dir / "Makefile.PL")
                << cmd::end()
                << cmd::out(dir / "const-xs.inc")
                << cmd::out(dir / "const-c.inc")
                ;
        }

        std::vector<path> typemaps{
            lib.SourceDir / "lib/ExtUtils/typemap",
        };
        if (!lib.DryRun && fs::exists(lib.SourceDir / dir / "typemap")) {
            typemaps.push_back(lib.SourceDir / dir / "typemap");
        }
        xsubpp(t, lib.BinaryDir / dir / (fn + ".xs"s), typemaps);
    };

    auto &dist_io = packages.addTarget<lib_build_type>("dist.IO");
    {
        auto &t = dist_io;
        t += lib;
        t += "dist/IO/poll.c";
        process_module(t, "dist/IO");
    }

    auto &ext_posix = packages.addTarget<lib_build_type>("ext.POSIX");
    {
        auto &t = ext_posix;
        t += lib;
        process_module(t, "ext/POSIX");
    }
}
