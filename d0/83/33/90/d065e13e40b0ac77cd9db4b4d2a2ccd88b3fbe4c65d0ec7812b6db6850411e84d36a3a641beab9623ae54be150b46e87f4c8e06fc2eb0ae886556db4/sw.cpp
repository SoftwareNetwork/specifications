//#pragma sw require header pub.egorpugin.primitives.templates2
#include <d:/dev/primitives/src/templates2/include/primitives/templates2/makefile.h>

template <typename T>
struct simple_mri {
    T &target;
    std::string s;
    std::set<path> inputs;

    simple_mri(T &target) : target{target} {
        auto &t = target;
        t.AutoDetectOptions = false;
        t.HeaderOnly = false;
        t.PublicBinaryDir = false;

        s += std::format("CREATE {}\n", t.getOutputFile().string());
    }
    ~simple_mri() {
        auto &t = target;
        auto fn = t.getPackage().toString() += ".mri";
        t.writeFileOnce(fn, text());

        auto out = t.getOutputFile();
        auto c = t.addCommand()
            << cmd::prog("llvm-ar")
            << cmd::wdir(t.BinaryDir)
            << "-M"
            << cmd::std_in(fn)
            << cmd::end()
            << cmd::out(out)
            ;
        for (auto &&i : inputs) {
            c << cmd::in(i);
        }
    }
    void operator+=(const path &fn) {
        inputs.insert(fn);
        s += std::format("ADDLIB {}\n", fn.string());
    }
    void operator+=(auto &lib) {
        target.Public += lib;
        *this += lib.getOutputFile();
    }
    std::string text() const {
        auto t = s;
        t += std::format("SAVE\n");
        t += std::format("END\n");
        return t;
    }
    /*void write(const path &fn) const {
        write_file(fn, text());
    }*/
};

void build(Solution &s) {
    auto &mingw = s.addProject("mingw.w64", "0.0.1");
    mingw += Git{"https://git.code.sf.net/p/mingw-w64/mingw-w64", "", "master"};

    auto cppstd = cpplatest;

    auto &libraries = mingw.addDirectory("libraries");
    auto &headers = mingw.addDirectory("headers");
    auto &windows = headers.addLibrary("windows");
    windows.setRootDirectory("mingw-w64-headers");
    windows.PublicBinaryDir = false;

    auto process_def = [](auto &t, const path &fn) {
        auto in = fn;
        if (!in.is_absolute()) {
            in = t.SourceDir / fn;
        }
        auto out = t.getOutputFile();
        t.addCommand()
            << cmd::prog("lld-link")
            << cmd::wdir(t.BinaryDir)
            << "-machine:x64"
            << "-def:" + in.string()
            << "-out:" + out.string()
            << cmd::end()
            << cmd::in(fn)
            << cmd::out(out)
            ;
        t.Public.LinkOptions.push_back(out.string());
    };
    auto process_def_in = [&](auto &t, const path &fn) {
        auto out = t.BinaryDir / fn.stem();
        t.addCommand()
            << cmd::prog("g++")
            << "-E"
            << "-P"
            << "-xc++-header"
            << "-m64"
            << cmd::in(fn)
            << "-I" << t.SourceDir / "mingw-w64-crt/def-include"
            << cmd::std_out(out);
            ;
        process_def(t, out);
    };

    auto gen_one = [&](auto &t, const path &p) -> decltype(auto) {
        if (p.extension() == ".def") {
            process_def(t, p);
            return t;
        } else if (p.extension() == ".in") {
            t += "mingw-w64-crt/def-include/.*"_rr;
            process_def_in(t, p);
            return t;

        }
        throw std::runtime_error{"unk def file: " + p.string()};
    };
    /*auto gen_dir = [&](auto &&dir) {
        for (auto &&p : fs::directory_iterator{dir}) {
            if (p.path().extension() == ".def") {
                gen_one(p.path());
            } else if (p.path().extension() == ".in") {
                gen_one(p.path());
            }
        }
    };*/
    //gen_dir("mingw-w64-crt/lib-common");
    //gen_dir("mingw-w64-crt/lib64");
    //gen_dir("mingw-w64-crt/libce");

    auto create_def_lib = [&](const std::string &name) -> StaticLibrary& {
        auto s = name;
        boost::replace_all(s, "-", "_");
        auto &t = windows.addStaticLibrary(s);
        t.AutoDetectOptions = false;
        t.HeaderOnly = false;
        t.PublicBinaryDir = false;
        return t;
    };

    std::map<std::string, StaticLibrary *> def_libs;
    auto get_def_lib = [&](const std::string &name) -> StaticLibrary& {
        auto it = def_libs.find(name);
        if (it != def_libs.end()) {
            return *it->second;
        }
        auto &t = create_def_lib(name);
        if (t.DryRun) {
            return t;
        }
        path base = t.SourceDir / "mingw-w64-crt/lib-common";
        base /= name;
        base += ".def";
        if (fs::exists(base)) {
            def_libs[name] = &gen_one(t, base);
            return t;
        }
        base += ".in";
        if (fs::exists(base)) {
            def_libs[name] = &gen_one(t, base);
            return t;
        }
        throw std::runtime_error{"unk def file: " + name};
    };
    auto &kernel32 = get_def_lib("kernel32");
    auto &ucrtbase_def = get_def_lib("ucrtbase");

    /*auto &gcc = mingw.addStaticLibrary("gcc");
    {
        //gcc.WholeArchive = true;
        gcc += "mingw-w64-libraries/winpthreads/src/libgcc/.*"_rr;
    }*/

    auto add_makefile_sources = [&](auto &&t, auto &&key) {
        if (t.DryRun) {
            return;
        }
        makefile_am f{t.SourceDir / "mingw-w64-crt/Makefile.am","LIB64="};
        for (auto &&f : f.variables[key]) {
            t += t.SourceDir / "mingw-w64-crt" / f;
        }
    };

    auto &mingw32 = mingw.addStaticLibrary("mingw32");
    {
        auto &t = mingw32;

        add_makefile_sources(t, "lib64_libmingw32_a_SOURCES");

        //t.Public += "_UNICODE"_def;
        //t.Public += "UNICODE"_def;

        t.Protected += "_SYSCRT"_def;
        t.Protected += "CRTDLL"_def; // shared only?
        t.Protected += "_CRTBLD"_def;
        // t.Protected += "_CRTIMP"_api;

        if (t.getCompilerType() == CompilerType::Clang) {
            t.Public += "__GNUC__=8"_def; // there are errors when >= 10
            t.Public += "__GNUC_MINOR__=0"_def;
            t.Public.CompileOptions.push_back("-U_MSC_VER");
            t.Public.CompileOptions.push_back("-Wno-ignored-attributes");
            t.Public.CompileOptions.push_back("-Wno-pragma-pack");
            t.Public.LinkOptions.push_back("-nostdlib");
            t.Public.LinkOptions.push_back("-fuse-ld=lld");
        }
        if (t.getCompilerType() == CompilerType::GCC) {
            t.Public.LinkOptions.push_back("-nostdlib");
            t.Public.LinkOptions.push_back("-fuse-ld=lld");
        }
        if (t.getBuildSettings().Native.MT) {
            t.Public.LinkOptions.push_back("-Wl,-Bstatic");
            t.Public.LinkOptions.push_back("-lunwind");
            t.Public.LinkOptions.push_back("-lgcc");
            t.Public.LinkOptions.push_back("-lgcc_eh");
            t.Public.LinkOptions.push_back("-Wl,-Bdynamic");

            // c++
            t.Public.LinkOptions.push_back("-Wl,-Bstatic");
            t.Public.LinkOptions.push_back("-lstdc++");
            t.Public.LinkOptions.push_back("-Wl,-Bdynamic");
            t.Public.LinkOptions.push_back("-lstdc++exp");
        } else {
            t += "gcc"_slib;
            t += "stdc++"_slib;
            t += "stdc++exp"_slib;
            t += "unwind"_slib;
        }

        t.Public.NativeCompilerOptions::System.IncludeDirectories.push_back(t.SourceDir / "mingw-w64-crt/include");
        t.Public.NativeCompilerOptions::System.IncludeDirectories.push_back(t.SourceDir / "mingw-w64-headers/crt");
        t.Public.NativeCompilerOptions::System.IncludeDirectories.push_back(t.SourceDir / "mingw-w64-headers/defaults/include");

        // 0x1400 means ucrt
        t.Public += "__USE_MINGW_ANSI_STDIO=0"_def;
        t.Public += "__MSVCRT_VERSION__=0x1400"_def;
        t.Variables["DEFAULT_MSVCRT_VERSION"] = "0x1400";
        t.Variables["DEFAULT_WIN32_WINNT"] = "0xa00";
        t.configureFile("mingw-w64-headers/crt/_mingw.h.in", "_mingw.h");

        t.Public += windows;
        t.Public += kernel32;
    }

    auto &mingwex = mingw.addStaticLibrary("mingwex");
    {
        auto &t = mingwex;
        t.AutoDetectOptions = false;
        t.PublicBinaryDir = false;

        add_makefile_sources(t, "lib64_libmingwex_a_SOURCES");

        t -= "mingw-w64-crt/stdio/_stat.c";
        t -= "mingw-w64-crt/stdio/ftello64.c";
        t -= "mingw-w64-crt/stdio/fseeko32.c";
        t -= "mingw-w64-crt/stdio/snprintf.c";
        t -= "mingw-w64-crt/stdio/ftello.c";
        t -= "mingw-w64-crt/stdio/_wstat.c";
        t -= "mingw-w64-crt/stdio/vwscanf.c";
        t -= "mingw-w64-crt/stdio/vscanf.c";
        t -= "mingw-w64-crt/stdio/fseeko64.c";

        t.Public += mingw32;
    }

    auto &ucrt_extra = mingw.addStaticLibrary("ucrt_extra");
    {
        auto &t = ucrt_extra;
        t.AutoDetectOptions = false;
        t.PublicBinaryDir = false;

        add_makefile_sources(t, "lib64_libucrt_extra_a_SOURCES");

        //t += "mingw-w64-crt/misc/mingw_matherr.c";
        t += "mingw-w64-crt/misc/__set_app_type.c";

        t.Public += mingwex;
    }

    auto &crtexe = mingw.addStaticLibrary("crtexe");
    {
        auto &t = crtexe;
        t.AutoDetectOptions = false;
        t.PublicBinaryDir = false;
        t.WholeArchive = true;
        t += "mingw-w64-crt/crt/crtexe.c";
        t += mingwex;
    }
    auto &crtdll = mingw.addStaticLibrary("crtdll");
    {
        auto &t = crtdll;
        t.AutoDetectOptions = false;
        t.PublicBinaryDir = false;
        t.WholeArchive = true;
        t += "mingw-w64-crt/crt/crtdll.c";
        t += mingwex;
    }

    auto &ucrtbase = mingw.addStaticLibrary("ucrtbase");
    {
        auto &t = ucrtbase;

        simple_mri m{t};
        m += ucrtbase_def;
        m += ucrt_extra;
    }

    auto &ucrt = mingw.addStaticLibrary("ucrt");
    {
        auto &t = ucrt;

        simple_mri m{t};
        m += get_def_lib("api-ms-win-crt-conio-l1-1-0");
        m += get_def_lib("api-ms-win-crt-convert-l1-1-0");
        m += get_def_lib("api-ms-win-crt-environment-l1-1-0");
        m += get_def_lib("api-ms-win-crt-filesystem-l1-1-0");
        m += get_def_lib("api-ms-win-crt-heap-l1-1-0");
        m += get_def_lib("api-ms-win-crt-locale-l1-1-0");
        m += get_def_lib("api-ms-win-crt-math-l1-1-0");
        m += get_def_lib("api-ms-win-crt-multibyte-l1-1-0");
        m += get_def_lib("api-ms-win-crt-private-l1-1-0");
        m += get_def_lib("api-ms-win-crt-process-l1-1-0");
        m += get_def_lib("api-ms-win-crt-runtime-l1-1-0");
        m += get_def_lib("api-ms-win-crt-stdio-l1-1-0");
        m += get_def_lib("api-ms-win-crt-string-l1-1-0");
        m += get_def_lib("api-ms-win-crt-time-l1-1-0");
        m += get_def_lib("api-ms-win-crt-utility-l1-1-0");
        m += ucrt_extra;
    }

    auto &ucrtapp_extra = mingw.addStaticLibrary("ucrtapp_extra");
    {
        auto &t = ucrtapp_extra;
        t.AutoDetectOptions = false;
        t.PublicBinaryDir = false;

        add_makefile_sources(t, "lib64_libucrtapp_extra_a_SOURCES");

        t += "__LIBMSVCRT__"_def;
        t.Public += ucrtbase;
    }

    auto &ucrtapp = mingw.addStaticLibrary("ucrtapp");
    {
        auto &t = ucrtapp;

        simple_mri m{t};
        m += get_def_lib("api-ms-win-crt-conio-l1-1-0");
        m += get_def_lib("api-ms-win-crt-convert-l1-1-0");
        m += get_def_lib("api-ms-win-crt-environment-l1-1-0");
        m += get_def_lib("api-ms-win-crt-filesystem-l1-1-0");
        m += get_def_lib("api-ms-win-crt-heap-l1-1-0");
        m += get_def_lib("api-ms-win-crt-locale-l1-1-0");
        m += get_def_lib("api-ms-win-crt-math-l1-1-0");
        m += get_def_lib("api-ms-win-crt-multibyte-l1-1-0");
        m += get_def_lib("api-ms-win-crt-process-l1-1-0");
        m += get_def_lib("api-ms-win-crt-runtime-l1-1-0");
        m += get_def_lib("api-ms-win-crt-stdio-l1-1-0");
        m += get_def_lib("api-ms-win-crt-string-l1-1-0");
        m += get_def_lib("api-ms-win-crt-time-l1-1-0");
        m += get_def_lib("api-ms-win-crt-utility-l1-1-0");
        m += ucrt_extra;
        m += ucrtapp_extra;
    }

    auto &main_def = ucrtapp;

    auto &winpthreads = libraries.addLibrary("winpthreads");
    {
        auto &t = winpthreads;
        t.PublicBinaryDir = false;
        t.setSourceDirectory("mingw-w64-libraries/winpthreads");

        t += "IN_WINPTHREAD"_def;
        t += sw::Shared, "DLL_EXPORT"_def;
        t.Public += sw::Shared, "WINPTHREADS_USE_DLLIMPORT"_def;

        //t += main_def;
        //t += crtdll;
    }

    // for libstdc++
    mingw32.Public += winpthreads;

    auto &tools = mingw.addProject("tools");
    tools.setRootDirectory("mingw-w64-tools");
    auto &gendef = tools.addExecutable("gendef");
    {
        auto &t = gendef;
        t.setRootDirectory("mingw-w64-tools/gendef");
        t += "src/.*"_rr;
        t += main_def;
    }
    auto &genidl = tools.addExecutable("genidl");
    {
        auto &t = genidl;
        t.setRootDirectory("mingw-w64-tools/genidl");
        t += main_def;
    }
    auto &genpeimg = tools.addExecutable("genpeimg");
    {
        auto &t = genpeimg;
        t.setRootDirectory("mingw-w64-tools/genpeimg");
        t += main_def;
    }
    auto &widl = tools.addExecutable("widl");
    {
        auto &t = widl;
        t.PackageDefinitions = true;
        t.setRootDirectory("mingw-w64-tools/widl");
        t += "include/.*"_rr;
        t += "src/.*"_r;
        t += "BIN_TO_INCLUDEDIR=\".\""_def;
        t += "BIN_TO_DLLDIR=\".\""_def;
        t += "DLLDIR=\".\""_def;
        t += main_def;
        t.Variables["HAVE_GETOPT_H"] = 1;
        t.Variables["HAVE_STRING_H"] = 1;
        t.Variables["PACKAGE_VERSION"] = "\"" + t.Variables["PACKAGE_VERSION"].toString() + "\"";
        t.configureFile("include/config.h.in", "config.h", ConfigureFlags::EnableUndefReplacements);
    }
}
