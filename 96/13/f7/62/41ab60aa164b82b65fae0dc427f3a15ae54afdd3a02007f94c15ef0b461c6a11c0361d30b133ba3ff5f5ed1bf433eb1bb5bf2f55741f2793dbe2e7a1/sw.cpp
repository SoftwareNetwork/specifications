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

        // win32 (?)
        "dist/ExtUtils-ParseXS/lib",
        "cpan/parent/lib",
        "cpan/ExtUtils-Constant/lib",

        // VMS (what is VMS?)
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

        // perlmain.c
        "ext/ExtUtils-Miniperl/lib",

        // nasm
        "cpan/IO-Compress/lib",
        "cpan/Scalar-List-Utils/lib",
        "cpan/Compress-Raw-Zlib/lib",
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
        //c.use_response_files = false;
        static auto path_delim = getBuildSettings().TargetOS.Type == OSType::Windows
                                //|| getBuildSettings().TargetOS.Type == OSType::Mingw
                                ? ";" : ":"
                                ;

        std::vector<path> paths;
        if (!libsdir.empty()) {
            paths.push_back(libsdir / "lib");
        }
        perl_dirs([&](auto &&p) {
            paths.push_back(SourceDir / p);
            // we add binary dir manually when making modules
        });
        String s;
        for (auto &&p : paths)
            s += p.string() + path_delim;
        for (auto &&p : extra_paths)
            s += p.string() + path_delim;
        s.resize(s.size() - 1);
        c.environment["PERL5LIB"] = s;

        c.environment["PERL_SRC"] = normalize_path(libsdir).string();
        c.environment["PERL_CORE"] = normalize_path(libsdir).string(); // needed for some modules
        // or this?                                  vvvvvvvvvvvvvvvvv
        //c.environment["PERL_SRC"] = normalize_path(SourceDir / "lib").string();

        //c.environment["PERL_LIB"] = normalize_path(SourceDir / "lib").string();
        //c.environment["PERL_INC"] = s;
        //c.environment["PERL_ARCHLIB"] = s;

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
    auto &p = s.addProject("perl", "5.43.8");
    p += Git("https://github.com/Perl/perl5", "v{v}");
    auto &packages = p.addDirectory("packages");
    //p += RemoteFile("https://github.com/Perl/perl5/archive/refs/tags/v{v}.tar.gz");

    auto base = [&](auto &t)
    {
        t +=
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
            "perlio.c"
            ;
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows) {
            t +=
            "win32/win32.c",
            "win32/win32sck.c",
            "win32/win32thread.c",
            "win32/fcrypt.c"
            ;
        } else if (!t.getBuildSettings().TargetOS.isApple()) {
            t += "crypt"_slib;
        }

        // for noreturn dllexport functions
        t.patch("win32/win32.h", "#      define PERL_CALLCONV_NO_RET", "//#define PERL_CALLCONV_NO_RET");
        // for linking xsubpp modules to perl.dll
        t.patch("INTERN.h", "defined(WIN32) && defined(__MINGW32__)", "(defined(WIN32) || defined(__MINGW32__))");
        t.patch("INTERN.h", "#      define EXT\n", "#      define EXT SW_EXPORT\n");
        //t.patch("INTERN.h", "#      define dEXT\n", "#      define dEXT SW_EXPORT\n");
        t.patch("INTERN.h", "#      define EXTCONST EXTERN_C const\n", "#      define EXTCONST EXTERN_C SW_EXPORT const\n");
        t.patch("INTERN.h", "#      define EXTCONST const\n", "#      define EXTCONST SW_EXPORT const\n");
        //t.patch("INTERN.h", "#      define dEXTCONST const\n", "#      define dEXTCONST SW_EXPORT const\n"); // not needed?
        // for xsubpp modules exports
        // (needed for *nix + SharedLibrary perl.lib)
        //t.patch("XSUB.h", "__declspec(dllexport)", "SW_PERL_API"); //????
        t.patch("XSUB.h", "#  define XS_EXTERNAL(name) extern \"C\" XSPROTO(name)", "#  define XS_EXTERNAL(name) extern \"C\" SW_EXPORT XSPROTO(name)");
        t.patch("XSUB.h", "#  define XS_EXTERNAL(name) XSPROTO(name)", "#  define XS_EXTERNAL(name) SW_EXPORT XSPROTO(name)");
        t.patch("XSUB.h", "defined(__CYGWIN__)", "(defined( __CYGWIN__) || defined(WIN32))");
    };

    auto &gu = p.addTarget<Executable>("generate_uudmap");
    gu += "generate_uudmap.c";
    gu += "mg_raw.h";

    using lib_build_type = SharedLibrary;

    auto &perl = p.addTarget<PerlExecutable>("perl");
     // for now, sw mixes static and shared builds
    auto &lib =
        //perl.getBuildSettings().TargetOS.Type == OSType::Windows
        //?
        (Library &)p.addTarget<SharedLibrary>("lib")
        //: (Library &)p.addTarget<StaticLibrary>("lib")
        ;
    auto &mp = p.addTarget<PerlExecutable>("miniperl");
    {
        mp.libsdir = lib.SourceDir;

        mp -= "config_h.SH";
        mp -= "Porting/config.sh";
        mp -= ".*\\.[hc]"_r;
        mp -= ".*\\.inc"_r;
        mp -= ".*\\.tab"_r;
        mp -= ".*\\.act"_r;
        mp -= "win32/.*"_rr;
        base(mp);
        mp += "miniperlmain.c";

        mp += "."_idir;

        if (mp.getBuildSettings().TargetOS.Type != OSType::Windows) {
            mp += "SW_PERL_API"_api;
        }
        mp += "PERLDLL"_def;
        mp += "PERL_CORE"_def;
        //mp += "PERL_EXTERNAL_GLOB"_def;
        //mp += "PERL_IS_MINIPERL"_def;

        for (auto &&f : {
            "perl.c",
            //"opmini.c",
            //"perlmini.c",
            //"universalmini.c",
            })
        for (auto &&d : {"-DPERL_IS_MINIPERL","-DPERL_EXTERNAL_GLOB",})
            mp[f].args.push_back(d);

        if (mp.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            mp += "win32"_idir;
            mp += "win32/include"_idir;

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
        } else {
            //if (!mp.getBuildSettings().TargetOS.isApple()) {
            if (mp.getCompilerType() == CompilerType::GNU) {
                mp += "quadmath"_slib;
            }
            if (!mp.DryRun) {
                auto f = read_file(mp.SourceDir / "config_h.SH");
                auto a = "!GROK!THIS!"s;
                auto p = f.find(a);
                if (p == -1) {
                    throw std::runtime_error{"cannot find perl anchor "s + a};
                }
                auto b = f.find('\n', p) + 1;
                auto e = f.find(a, b);
                auto file = f.substr(b, e-b);
                auto lines = read_lines(mp.SourceDir / "Porting/config.sh");
                std::map<std::string, std::string> m1;
                for (auto &&line : lines) {
                    if (line[0] == '#' || line[0] == ':' || line[0] == ' ') {
                        continue;
                    }
                    auto equ = line.find('=');
                    auto k = line.substr(0,equ);
                    auto off = equ+1;
                    if (line[off] == '\'') ++off;
                    if (line.back() == '\'') line.pop_back();
                    auto v = line.substr(off);
                    m1.emplace(k, v);
                    //for (auto &&repl : {" "s, "\t"s, "\""s}) {
                    //    if (!(v.empty() && repl == " "s)) {
                    //        v += repl;
                    //    }
                    //    boost::replace_all(file, "$"s + k + repl, v);
                    //}
                    //boost::replace_all(file, "$"s + k + "\t", v + "\t");
                    //boost::replace_all(file, "$"s + k + "\"", v + "\"");
                }
                m1["usequadmath"] = "undef";
                m1["i_quadmath"] = "undef";
                if (mp.getCompilerType() == CompilerType::GNU) {
                    m1["usequadmath"] = "define";
                    m1["i_quadmath"] = "define";
                } else {
                    m1["nvtype"] = "long double";
                    m1["uselongdouble"] = "define";
                }
                if (mp.getBuildSettings().TargetOS.isApple()) {
                    m1["nvtype"] = "long double";
                    m1["uselongdouble"] = "define";

                    m1["d_crypt"] = "undef";
                    m1["i_crypt"] = "undef";
                    m1["i_shadow"] = "undef";
                    m1["d_off64_t"] = "undef";
                    m1["d_getspnam"] = "undef";
                    m1["d_getspnam_r"] = "undef";
                    m1["d_union_semun"] = "define";
                    m1["d_strerror_l"] = "undef";
                    m1["strerror_r_proto"] = "REENTRANT_PROTO_I_IBI"; // IBW?
                    m1["d_setresuid"] = "undef";
                    m1["d_readdir64_r"] = "undef";
                    m1["d_prctl_set_name"] = "undef";
                    //m1["d_strxfrm"] = "undef";
                    m1["d_strxfrm_l"] = "undef";
                    m1["d_memrchr"] = "undef";
                    m1["d_setregid"] = "undef";
                    m1["d_setresgid"] = "undef";
                    m1["d_dup3"] = "undef";
                    m1["d_pipe2"] = "undef";
                    m1["d_gethostbyaddr_r"] = "undef";
                    m1["d_gethostbyname_r"] = "undef";
                    m1["d_getprotobyname_r"] = "undef";
                    m1["d_getprotobynumber_r"] = "undef";
                    m1["d_getprotoent_r"] = "undef";
                    m1["d_getservbyname_r"] = "undef";
                    m1["d_getservbyport_r"] = "undef";
                    m1["d_getservent_r"] = "undef";
                    m1["d_getpwent_r"] = "undef";
                    m1["d_getgrent_r"] = "undef";
                    m1["d_getnetbyaddr_r"] = "undef";
                    m1["d_getnetbyname_r"] = "undef";
                    m1["d_gethostent_r"] = "undef";
                    m1["d_getnetent_r"] = "undef";
                    m1["d_syscall"] = "undef";
                    m1["d_clearenv"] = "undef";
                    m1["d_clearerr"] = "undef";
                    m1["d_eaccess"] = "undef";
                    m1["d_cuserid"] = "undef";
                }
                std::vector<std::pair<std::string, std::string>> m2;
                for (auto &&[k,v] : m1) {
                    m2.emplace_back(k, v);
                }
                std::ranges::sort(m2, [](auto &&v1, auto &&v2) {return v1.first.size() > v2.first.size(); });
                for (auto &&[k,v] : m2) {
                    boost::replace_all(file, "$"s + k, v);
                }
                mp.writeFileOnce("config.h", file);
            }
            //mp.configureFile("Porting/config_H", "config.h");
        }

        mp += "pub.egorpugin.primitives.response_file_handler"_dep;
        mp.patch("miniperlmain.c", "int exitstatus, i;", "process_response_file(&argc, &argv); int  exitstatus, i;");
        mp.patch("miniperlmain.c", "static PerlInterpreter *my_perl;", "static  PerlInterpreter *my_perl;\n#include <primitives/response_file_handler.h>");

        mp.addCommand()
            << cmd::prog(gu)
            << cmd::out("uudmap.h")
            << cmd::out("bitcount.h")
            << cmd::out("mg_data.h")
            ;
    }

    auto config_pm = lib.BinaryDir / "lib" / "Config.pm";

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
        c << cmd::wdir(out.parent_path());
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
        //c << cmd::in(config_pm);
        //lib -= out;
        t += out; // how this works with sw upload?
        return out;
    };
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
    struct patch_data {
        std::string from;
        std::string to;
    };
    struct pl_patch_options {
        std::vector<patch_data> patches;
    };
    auto PL_to_file = [&](auto &&fn, pl_patch_options popts = {}) {
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
        auto patch_from = path{out}.filename().string();
        auto patch_to = normalize_path(lib.BinaryDir / out).string();
        if (popts.patches.empty()) {
            popts.patches.emplace_back(patch_from, patch_to);
        } else {
            for (auto &&[f,t] : popts.patches) {
                boost::replace_all(t, patch_from, patch_to);
            }
        }
        for (auto &&[f,t] : popts.patches) {
            copy_and_patch(lib, outcopy, f, t);
        }
        mp.extra_paths.push_back((lib.BinaryDir / out).parent_path());
        perl.extra_paths.push_back((lib.BinaryDir / out).parent_path());
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
        return lib.BinaryDir / out;
    };

    {
        (lib.Public + mp)->IncludeDirectoriesOnly = true;

        lib += "MANIFEST";
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
        base(lib);
        if (lib.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            lib += "win32/perllib.c";
            if (auto nsf = lib["win32/perllib.c"].as<NativeSourceFile*>())
                nsf->BuildAs = NativeSourceFile::CPP;
        }
        lib += "pub.egorpugin.primitives.response_file_handler"_dep;
        lib.patch("win32/perllib.c", "EXTERN_C HANDLE w32_perldll_handle;", "EXTERN_C  HANDLE w32_perldll_handle;\n#include <primitives/response_file_handler.h>");
        lib.patch("win32/perllib.c", "int exitstatus;", "process_response_file(&argc, &argv); int  exitstatus;");

        lib.Protected += "."_idir;

        if (lib.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            lib += "ext/Win32CORE/Win32CORE.c";

            lib.Public += "WIN32"_def;
            lib.Public += "WIN64"_def;

            lib.Protected += "win32"_idir;
            lib.Protected += "win32/include"_idir;

            lib += "ws2_32.lib"_slib;
            lib += "user32.lib"_slib;
            lib += "Advapi32.lib"_slib;
            lib += "Comctl32.lib"_slib;
        } else {
            lib += "SOCKET=int"_def;
            if (lib.getCompilerType() == CompilerType::GNU) {
                lib.Public += "quadmath"_slib;
            }
        }

        lib += "PERL_CALLCONV"_api;
        if (lib.getBuildSettings().TargetOS.Type != OSType::Windows) {
            lib += "SW_PERL_API"_api;
        }
        // for nix
        lib.patch("perl.h", "#  define __attribute__visibility__(x) __attribute__((visibility(x)))", "//#   define __attribute__visibility__(x) __attribute__((visibility(x)))");
        lib.patch("perl.h", "EXTERN_C int perl_tsa_mutex_lock(perl_mutex* mutex)", "EXTERN_C SW_EXPORT int perl_tsa_mutex_lock(perl_mutex* mutex)");
        lib.patch("perl.h", "EXTERN_C int perl_tsa_mutex_unlock(perl_mutex* mutex)", "EXTERN_C SW_EXPORT int perl_tsa_mutex_unlock(perl_mutex* mutex)");
        lib.patch("thread.h", "extern PERL_THREAD_LOCAL", "extern SW_EXPORT PERL_THREAD_LOCAL");

        lib.Public += sw::Static, "PERL_STATIC_SYMS"_def;
        lib += sw::Shared, "PERLDLL"_def;
        lib += "PERL_CORE"_def;

        // some conf
        lib.Public += "PERL_TEXTMODE_SCRIPTS"_def;
        if (lib.getBuildSettings().TargetOS.Type == OSType::Windows) {
            lib.Public += "PERL_IMPLICIT_SYS"_def;
        }
        // comment out?
        //lib.Public += "PERL_IMPLICIT_CONTEXT"_def; // but PERL_IMPLICIT_CONTEXT was removed from core perl. It does not do anything. Undeffing it for compilation
        lib.Public += "MULTIPLICITY"_def; // needed for PERL_IMPLICIT_CONTEXT
        // on nix they are set via config
        if (lib.getBuildSettings().TargetOS.Type == OSType::Windows) {
            // we need both
            lib.Public += "USE_ITHREADS"_def; // same as USE_THREADS? looks like no
            lib.Public += "USE_THREADS"_def;

            // for dynamic xsubpp modules
            lib.Public += "USE_DYNAMIC_LOADING"_def;
        }
        //

        //
        //lib.patch("cpan/IPC-Cmd/lib/IPC/Cmd.pm", "POSIX::WNOHANG", "$sw::POSIX_WNOHANG");
        //lib.patch("cpan/IPC-Cmd/lib/IPC/Cmd.pm", "POSIX->import();", "POSIX->import( ); $sw::POSIX_WNOHANG = POSIX::WNOHANG();");
        lib.patch("cpan/IPC-Cmd/lib/IPC/Cmd.pm", ", POSIX::WNOHANG", ",  POSIX::WNOHANG()");
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
            auto config_sh = lib.BinaryDir / "config.sh";

            if (lib.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
               lib.addCommand()
                    << cmd::prog(mp)
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
                    << cmd::std_out(config_sh)
                    ;
            } else {
                //config_sh = lib.SourceDir / "Porting" / "config.sh";
                if (!lib.DryRun) {
                    fs::create_directories(lib.BinaryDir);
                    copy_file(lib.SourceDir / "Porting" / "config.sh", config_sh);
                }
                //lib.configureFile("Porting/config_H", lib.BinaryDir / "config.h");
            }

            auto make_patchnum_pl = lib.BinaryDir / "make_patchnum.pl";
            auto config_git_pl = lib.BinaryDir / "Config_git.pl";
            //lib.addCommand(SW_VISIBLE_BUILTIN_FUNCTION(copy_file)) << cmd::in(lib.Source / "make_patchnum.pl") << cmd::out(make_patchnum_pl);
            if (!lib.DryRun) {
                fs::create_directories(lib.BinaryDir / "lib");
                copy_file(lib.SourceDir / "make_patchnum.pl", make_patchnum_pl);

                // prevent errors on ./abs/path
                lib.patch(make_patchnum_pl, "$file= path_to($file);", "#$file = path_to($file);");
                // out to .in.h and copy after?
                //lib.patch(make_patchnum_pl, "'git_version.h'", "'"s + normalize_path(fs::absolute((lib.BinaryDir / "git_version.h").lexically_relative(lib.SourceDir))).string() + "'");
                lib.patch(make_patchnum_pl, "'git_version.h'", "'"s + normalize_path(lib.BinaryDir / "git_version.h").string() + "'");
                // was lib/Config_git.pl
                //lib.patch(make_patchnum_pl, "'lib/Config_git.pl'", "'"s + normalize_path(fs::absolute(config_git_pl.lexically_relative(lib.SourceDir))).string() + "'");
                lib.patch(make_patchnum_pl, "'lib/Config_git.pl'", "'"s + normalize_path(config_git_pl).string() + "'");
            }

            lib.addCommand()
                << cmd::prog(mp)
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

            lib.addCommand()
                << cmd::prog(mp)
                << cmd::wdir(lib.BinaryDir)
                << "-I" << lib.SourceDir
                << "-I" << lib.SourceDir / "lib"
                << cmd::in("configpm")
                << cmd::end()
                << cmd::in(config_sh)
                << cmd::in(config_git_pl)
                << cmd::out(lib.BinaryDir / "lib" / "Config.pod")
                << cmd::out(config_pm)
                ;

            if (!lib.DryRun) {
                copy_file(lib.SourceDir / "write_buildcustomize.pl", lib.BinaryDir / "write_buildcustomize.pl");
                lib.patch(lib.BinaryDir / "write_buildcustomize.pl", "my $file = 'lib/buildcustomize.pl';", std::format("my $file = '{}/lib/buildcustomize.pl';", normalize_path(lib.BinaryDir).string()));
            }
            lib.addCommand()
                << cmd::prog(mp)
                << "-I" << lib.SourceDir / "lib"
                << "-f"
                << cmd::in(lib.BinaryDir / "write_buildcustomize.pl")
                << cmd::end()
                << cmd::out("lib/buildcustomize.pl")
                ;

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
                    ;
                if (lib.getBuildSettings().TargetOS.Type == OSType::Windows) {
                    dyna << cmd::in("ext/DynaLoader/dl_win32.xs");
                } else if (lib.getBuildSettings().TargetOS.isApple()) {
                    lib.patch("ext/DynaLoader/dl_dyld.xs", "return handle;", "return (char *)handle;");
                    dyna << cmd::in("ext/DynaLoader/dl_dyld.xs");
                } else {
                    dyna << cmd::in("ext/DynaLoader/dl_dlopen.xs");
                }
                dyna
                    << cmd::std_out("DynaLoader.c")
                    << cmd::end()
                    << cmd::in(config_pm)
                    //<< cmd::in(lib.SourceDir / "win32" / "config.h")
                    ;
                lib += "DynaLoader.c";
                lib += "ext/DynaLoader"_idir;
            }

            perl.extra_paths.push_back(lib.SourceDir / "lib");
            perl.extra_paths.push_back(lib.BinaryDir);
            perl.extra_paths.push_back(lib.BinaryDir / "lib");
            //
            perl.extra_paths.push_back(lib.BinaryDir / "ext");
            perl.extra_paths.push_back(lib.BinaryDir / "dist");
            perl.extra_paths.push_back(lib.BinaryDir / "cpan");

            // some modules depends on previous ones
            // we did not keep tracking of it yet
            auto make_module_simple = [&](auto &&disposition, auto &&name, pl_patch_options popts = {}) {
                PL_to_file(std::format("{0}/{1}/{1}_pm.PL", disposition, name), popts);
            };
            auto make_module_simple1 = [&](const path &name, pl_patch_options popts = {}) {
                make_module_simple(name.parent_path().string(), name.filename().string(), popts);
            };
            make_module_simple1("dist/XSLoader");
            lib.patch(lib.SourceDir / "ext/DynaLoader/DynaLoader_pm.PL", "croak(\"Can't locate", "$file = dl_findfile_sw($modfname) unless $file; croak( \"Can't locate");
            lib.patch(lib.SourceDir / "ext/DynaLoader/DynaLoader_pm.PL", "sub dl_findfile {", std::format(R"(sub dl_findfile_sw {{
    my $output = @_[0];
    my $output = qx({} list org.sw.demo.perl.packages.*.$output 2>&1); # -bt
    #print STDERR "$output";
    my @output = split /\n/, $output;
    my @output = split / /, $output;
    $output = "$output[0]-$output[1]";
    return dl_findfile(map("-L$_",@dirs,@INC), $output);
}}

sub dl_findfile  {{)",
                //normalize_string_copy(sw::getProgramLocation().string())) // we still have more changes, so ignore
                normalize_string_copy(sw::getSwExecutableName().string()))
            );
            make_module_simple1("ext/DynaLoader", {{
                {"DynaLoader.pm\" if","DynaLoader.pm\"  if"},
                {"DynaLoader.pm\" or","DynaLoader.pm\"  or"},
                {"DynaLoader.pm\";","DynaLoader.pm\" ;"},
            }});
            make_module_simple1("dist/lib");
        }
    }

    // perl
    {
        perl.libsdir = lib.SourceDir;

        //perl += "SW_PERL_API"_api;
        perl -= "win32/runperl.c";
        perl.Public += lib;

        if (lib.getBuildSettings().TargetOS.Type == OSType::Windows) {
            perl += "win32/runperl.c";
        } else {
            perl.writeFileOnce("makemain.pl", R"(
use ExtUtils::Miniperl;
writemain(\"perlmain.c", 'DynaLoader');
)");

            //  -MExtUtils::Miniperl -e 'writemain(\"perlmain.c", @ARGV)' DynaLoader

            auto makeperl = perl.addCommand();
            makeperl << cmd::prog(mp) << cmd::wdir(perl.BinaryDir);
            fix_perl_path_old2(makeperl);
            fix_perl_path_old(makeperl);
            makeperl
                << cmd::in("makemain.pl")
                //<< "-MExtUtils::Miniperl"
                //<< "-e"
                //<< "'writemain(\\perlmain.c, @ARGV)'"
                //<< "DynaLoader"
                // add more static exts here
                << cmd::end()
                //<< cmd::std_out(perl.BinaryDir / "perlmain.c")
                << cmd::out(perl.BinaryDir / "perlmain.c")
                ;
            //perl += perl.BinaryDir / "perlmain.c";
        }

        perl -= "lib/.*"_rr;
        perl -= "dist/.*"_rr;
        perl -= "ext/.*"_rr;
        perl -= "cpan/.*"_rr;
    }

    struct perl_module_data {
        path dir;
        std::string name;
        std::string package_name;
        std::string xs_name;
        std::string version;
        struct fixup {
            std::string filename;
            std::string from_format;
            std::string to_format_; // maybe default to "'{}'" ?

            auto from() const {
                return std::vformat(from_format, std::make_format_args(filename));
            }
            auto to_format() const {
                return to_format_.empty() ? from_format : to_format_;
            }
            auto to(const path &dir) const {
                auto s = normalize_path(dir / filename).string();
                return std::vformat(to_format(), std::make_format_args(s));
            }
        };
        std::vector<fixup> fixup_filenames;
        bool makefile_PL_wdir_source_dir{};
        std::vector<std::string> constants_outputs{"const-c.inc"s,"const-xs.inc"s};
    };

    // dedup with PL_to_file()
    auto process_module = [&](auto &t, const perl_module_data &d) {
        const path &dir = d.dir;
        const std::string &module = d.name;

        if (!d.version.empty()) {
            t += Definition{"VERSION=\"" + d.version + "\""};
            t += Definition{"XS_VERSION=\"" + d.version + "\""};
        }

        t += IncludeDirectory{t.SourceDir / dir};
        t += IncludeDirectory{t.BinaryDir / dir};

        //if (t.getBuildSettings().TargetOS.Type == OSType::Windows) {
            t += lib;
        //} else {
        //    t += perl;
        //}

        auto s = dir.filename().string();
        auto p = s.rfind('-');
        auto has_dash = p != -1;
        auto fn = has_dash ? s.substr(p + 1) : s;
        if (!module.empty()) {
            fn = module;
        }
        auto fn_upper = boost::to_upper_copy(fn);
        auto prefix = s.substr(0, p);
        auto xs_name = (d.xs_name.empty() ? fn : d.xs_name) + ".xs"s;

        t += Definition{"PERL_EXT_" + fn_upper + "_BUILD"};
        //t += Definition{"PERL_EXT_" + fn_upper + "_DEBUG"};
        t += Definition{"PERL_IN_XSUB_" + fn_upper};
        t.Public += sw::Static, Definition{"PERL_EXT_" + fn_upper + "_STATIC"};
        t += "PERL_EXT"_def;

        if (!lib.DryRun) {
            fs::create_directories(lib.BinaryDir / dir);
            auto pm = lib.SourceDir / dir / (fn + ".pm"s);
            auto has_pm = fs::exists(pm);
            if (has_pm) {
                copy_file(pm, lib.BinaryDir / dir / (fn + ".pm"s));
            }
            copy_file(lib.SourceDir / dir / xs_name, lib.BinaryDir / dir / xs_name);

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
            for (auto &&f : d.fixup_filenames) {
                lib.patch(dir / "Makefile.PL", f.from(), f.to(lib.SourceDir / dir));
            }
            auto c = lib.addCommand();
            c
                << cmd::prog(mp)
                << cmd::wdir(d.makefile_PL_wdir_source_dir ? lib.SourceDir / dir : lib.BinaryDir / dir)
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
                ;
            for (auto &&f : d.constants_outputs) {
                c << cmd::out(dir / f);
            }
            c
                << cmd::in(config_pm)
                ;
        }

        std::vector<path> typemaps{
            lib.SourceDir / "lib/ExtUtils/typemap",
        };
        if (!lib.DryRun && fs::exists(lib.SourceDir / dir / "typemap")) {
            typemaps.push_back(lib.SourceDir / dir / "typemap");
        }
        xsubpp(t, lib.BinaryDir / dir / xs_name, typemaps);
    };
    auto process_module2 = [&](const perl_module_data &d) -> decltype(auto) {
        auto n = d.dir.string();
        boost::replace_all(n, "/", ".");
        boost::replace_all(n, "-", ".");
        if (!d.name.empty()) {
            n += "."s + d.name;
        }
        if (!d.package_name.empty()) {
            n = d.package_name;
        }
        auto &t = packages.addTarget<lib_build_type>(n);
        process_module(t, d);
        return t;
    };
    auto process_module_with_c_files = [&](const perl_module_data &d) -> decltype(auto) {
        auto &t = process_module2(d);
        t.AllowEmptyRegexes = true;
        t -= FileRegex{d.dir, ".*", false};
        t += FileRegex{d.dir, ".*\\.[hc]", false};
        t.AllowEmptyRegexes = false;
        return t;
    };

    // some tricky modules
    auto &pp = packages.addTarget<lib_build_type>("dist.Devel.PPPort");
    {
        auto &t = pp;
        t += lib;

        auto RealPPPort_xs = PL_to_file("dist/Devel-PPPort/RealPPPort_xs.PL");
        auto pport_pm = PL_to_file("dist/Devel-PPPort/PPPort_pm.PL");
        {
            auto ppport_pl = copy_and_patch(lib, "dist/Devel-PPPort/ppport_h.PL", "ppport.h", normalize_path(lib.BinaryDir / "ppport.h").string());

            auto ppport = lib.addCommand();
            ppport << cmd::prog(mp)
                << cmd::wdir(lib.BinaryDir / "dist/Devel-PPPort")
                << "-I" << lib.SourceDir / "lib"
                << "-I" << pport_pm.parent_path()
                << cmd::in(ppport_pl)
                << cmd::out("ppport.h")
                << cmd::end()
                << cmd::in(pport_pm)
                ;
        }

        xsubpp(t, RealPPPort_xs, std::vector<path>{
            lib.SourceDir / "lib/ExtUtils/typemap",
            lib.SourceDir / "dist/Devel-PPPort/typemap",
        });
        t += "dist/Devel-PPPort/.*\\.c"_r;
    }

    auto &enc = packages.addTarget<lib_build_type>("cpan.Encode");
    {
        auto &t = enc;
        t -= "cpan/Encode/.*"_rr;
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

        process_module(t, {"cpan/Encode"});
    }

    auto &ext_re = packages.addTarget<lib_build_type>("ext.re");
    {
        auto &t = ext_re;
        t += FileRegex{"ext/re", ".*\\.[hc]", false};
        //t += "dquote.c";
        t += "regcomp.c";
        t += "regcomp_invlist.c";
        t += "regcomp_study.c";
        t += "regcomp_trie.c";
        t += "regcomp_debug.c";
        t += "regexec.c";

        t += "VERSION=\"0.48\""_def;
        t += "XS_VERSION=\"0.48\""_def;

        // perl has some symbols with visibility hidden - it is missing on msvc
        // so we force allow multiple same symbols across dll
        if (lib.getCompilerType() == CompilerType::MSVC) {
            t.LinkOptions.push_back("/FORCE:MULTIPLE");
        } else if (!t.getBuildSettings().TargetOS.isApple()) {
            t.LinkOptions.push_back("-Wl,-z,muldefs");
        }

        process_module(t, {"ext/re"});
    }

    // simpler modules

    process_module2({"dist/PathTools", "Cwd"}) += "DOUBLE_SLASHES_SPECIAL=0"_def;

    process_module2({.dir = "dist/Storable", .version = "3.39"});
    process_module2({.dir = "ext/Fcntl", .version = "1.20"});

    process_module_with_c_files({"cpan/Win32"}) +=
       "advapi32.lib"_slib,
       "User32.lib"_slib,
       "Winhttp.lib"_slib,
       "Version.lib"_slib,
       "ole32.lib"_slib,
       "Netapi32.lib"_slib,
       "Userenv.lib"_slib,
       "Shell32.lib"_slib
       ;

    process_module_with_c_files({"ext/File-Glob"});
    auto &io = process_module_with_c_files({"dist/IO"});
    if (io.getBuildSettings().TargetOS.Type != OSType::Windows) {
        io -= IncludeDirectory{io.SourceDir / "dist/IO"};
    }
    process_module2({"ext/POSIX"});
    process_module_with_c_files({.dir = "cpan/Scalar-List-Utils", .package_name = "cpan.Util", .xs_name = "ListUtil"});
    auto &cpan_zlib = process_module2({.dir = "cpan/Compress-Raw-Zlib", .fixup_filenames = {{"config.in","'{}'"},{"zlib-src","'./{}'","'{}'"}},.constants_outputs = {"constants.h"s, "constants.xs"s}});
    cpan_zlib += "Perl_crz_BUILD_ZLIB=1"_def, "org.sw.demo.madler.zlib"_dep;
    lib.patch("cpan/Compress-Raw-Zlib/config.in", "= ./zlib-src", "= " + normalize_path(lib.SourceDir / "cpan/Compress-Raw-Zlib/zlib-src").string());
}
