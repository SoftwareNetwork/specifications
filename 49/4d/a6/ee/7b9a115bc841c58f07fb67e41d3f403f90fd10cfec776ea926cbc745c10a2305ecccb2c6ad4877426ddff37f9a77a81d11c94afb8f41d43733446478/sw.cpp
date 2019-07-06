void build(Solution &s)
{
    auto &p = s.addProject("intel", "2019.8.0");
    p += RemoteFile("https://github.com/01org/tbb/archive/{M}_U{m}.tar.gz");

    String arch;

    auto make_ver = [](auto &t)
    {
        t.writeFileOnce("version_string.ver", R"(#define __TBB_VERSION_STRINGS(N) \
#N": BUILD_HOST         @CMAKE_SYSTEM_NAME@" ENDL \
#N": BUILD_OS           @CMAKE_SYSTEM@" ENDL \
#N": BUILD_KERNEL       @CMAKE_SYSTEM_VERSION@" ENDL \
#N": BUILD_GCC          @CMAKE_CXX_COMPILER_ID@" ENDL \
#N": BUILD_LIBC         Unknown" ENDL \
#N": BUILD_LD           Unknown" ENDL \
#N": BUILD_TARGET       Unknown" ENDL \
#N": BUILD_COMMAND      Unknown" ENDL

#define __TBB_DATETIME "@_configure_date@"
)");
    };

    auto make_def = [&s](auto &t, const String &name, const String &arch)
    {
        auto cc = std::static_pointer_cast<VisualStudioCompiler>(t.findProgramByExtension(".cpp")->clone());
        cc->CompileAsC = true;
        cc->PreprocessToStdout = true;
        cc->IncludeDirectories.insert(t.SourceDir / "include");
        cc->InputFile = t.SourceDir / ("src/" + name + "/" + arch + "-" + name + "-export.def");

        auto c = cc->getCommand(t);
        c->redirectStdout(t.BinaryDir / ("" + name + ".def"));
        t.Storage.push_back(c);
        t += t.BinaryDir / ("" + name + ".def");
    };

    auto make_def_simple = [&s](auto &t, const String &name, const String &arch)
    {
        t -= "src/.*\\.def"_rr;
        t += path("src/" + name + "/" + arch + "-" + name + "-export.def");
    };

    auto &tbb = p.addTarget<LibraryTarget>("tbb");
    {
        tbb +=
            "include/.*"_rr,
            "src/old/.*"_rr,
            "src/old/.*\\.h"_rr,
            "src/rml/.*\\.h"_rr,
            "src/rml/client/rml_tbb.cpp",
            "src/tbb/.*\\.asm"_rr,
            "src/tbb/.*\\.cpp"_rr,
            "src/tbb/.*\\.def"_rr,
            "src/tbb/.*\\.h"_rr,
            "src/tbb/.*\\.lst"_rr;
        tbb ^= "include/tbb/tbbmalloc_proxy.h";

        tbb.Private +=
            "src"_id,
            "src/rml/include"_id;

        tbb.Public += "include"_id;

        tbb -= "src/tbb/.*\\.asm"_rr;
        tbb -= "src/tbb/.*\\.def"_rr;
        tbb -= "src/.*test.*"_rr;

        tbb.Private += "__TBB_BUILD"_d;
        tbb.Public += "__TBB_NO_IMPLICIT_LINKAGE"_d;
        tbb.Public += "__TBBMALLOC_NO_IMPLICIT_LINKAGE"_d;
        if (tbb.getSettings().TargetOS.Type != OSType::Windows)
        {
            tbb.Public += "USE_PTHREAD"_d;
        }
        if (tbb.getSettings().TargetOS.Type == OSType::Windows)
        {
            arch += "win";
            if (tbb.getSettings().TargetOS.is(ArchType::x86_64))
            {
                tbb += "src/tbb/intel64-masm/.*"_rr;
                arch += "64";
            }
            else
            {
                tbb += "src/tbb/ia32-masm/.*"_rr;
                arch += "32";
            }
            tbb.Public += "USE_WINTHREAD"_d;

            make_def(tbb, "tbb", arch);
        }

        make_ver(tbb);
    }

    auto &tbbmalloc = tbb.addTarget<SharedLibraryTarget>("malloc");
    {
        tbbmalloc +=
            "include/.*"_rr,
            "src/tbb/.*\\.h"_rr,
            "src/tbbmalloc/.*\\.cpp"_rr,
            "src/tbbmalloc/.*\\.def"_rr,
            "src/tbbmalloc/.*\\.h"_rr;

        tbbmalloc -=
            "src/tbbmalloc/proxy.*"_rr,
            "src/tbbmalloc/tbb_function_replacement.*"_rr;

        // disable autolink
        tbbmalloc.patch("include/tbb/tbbmalloc_proxy.h", "#pragma comment", "//#pragma  comment");

        tbbmalloc.Private += "src"_id;
        tbbmalloc.Public += "include"_id;

        tbbmalloc.Private += "__TBBMALLOC_BUILD"_d;
        tbbmalloc.Public += "__TBB_NO_IMPLICIT_LINKAGE"_d;
        tbbmalloc.Public += "__TBBMALLOC_NO_IMPLICIT_LINKAGE"_d;
        if (tbbmalloc.getSettings().Native.LibrariesType == LibraryType::Static)
            tbbmalloc += "__TBB_SOURCE_DIRECTLY_INCLUDED"_def;
        if (tbbmalloc.getSettings().TargetOS.Type != OSType::Windows)
        {
            tbbmalloc.Public += "USE_PTHREAD"_d;
        }
        if (tbbmalloc.getSettings().TargetOS.Type == OSType::Windows)
        {
            tbbmalloc.Public += "USE_WINTHREAD"_d;

            make_def_simple(tbbmalloc, "tbbmalloc", arch);
        }

        make_ver(tbbmalloc);
    }

    auto &tbbmalloc_proxy = tbbmalloc.addTarget<SharedLibraryTarget>("proxy");
    {
        tbbmalloc_proxy +=
            "include/.*"_rr,
            "src/tbb/.*\\.h"_rr,
            "src/tbbmalloc/.*\\.h"_rr,
            "src/tbbmalloc/proxy.*"_rr,
            "src/tbbmalloc/tbb_function_replacement.*"_rr;
        tbbmalloc_proxy ^= "include/tbb/tbbmalloc_proxy.h";

        tbbmalloc_proxy.Public += "__TBB_NO_IMPLICIT_LINKAGE"_d;
        tbbmalloc_proxy.Public += "__TBBMALLOC_NO_IMPLICIT_LINKAGE"_d;
        if (tbbmalloc_proxy.getSettings().TargetOS.Type != OSType::Windows)
        {
            tbbmalloc_proxy.Public += "USE_PTHREAD"_d;
        }
        if (tbbmalloc_proxy.getSettings().TargetOS.Type == OSType::Windows)
        {
            tbbmalloc_proxy.Public += "USE_WINTHREAD"_d;
        }

        tbbmalloc_proxy.Public += tbbmalloc;

        if (tbbmalloc_proxy.getSettings().TargetOS.Type == OSType::Windows)
        {
            if (tbbmalloc_proxy.getSettings().TargetOS.is(ArchType::x86_64))
                tbbmalloc_proxy.Interface.LinkOptions.push_back("-INCLUDE:__TBB_malloc_proxy");
            else if (tbbmalloc_proxy.getSettings().TargetOS.is(ArchType::x86))
                tbbmalloc_proxy.Interface.LinkOptions.push_back("-INCLUDE:___TBB_malloc_proxy");
        }
    }
}
