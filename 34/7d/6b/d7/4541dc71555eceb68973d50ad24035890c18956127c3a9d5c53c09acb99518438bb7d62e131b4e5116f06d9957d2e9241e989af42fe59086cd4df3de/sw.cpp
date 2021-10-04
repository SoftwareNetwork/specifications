void build(Solution &s)
{
    auto &p = s.addProject("intel", "2021.4.0");
    p += Git("https://github.com/intel/tbb", "v{v}");

    auto make_def_fn = [](auto &t, auto &&name)
    {
        String arch;
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            arch += "win";
        else if (t.getBuildSettings().TargetOS.isApple())
            arch += "mac";
        else
            arch += "lin";
        if (t.getBuildSettings().TargetOS.is(ArchType::x86_64))
            arch += "64";
        else
            arch += "32";
        arch += "-";
        arch += name;
        arch += ".def";
        return arch;
    };

    auto &tbb = p.addTarget<LibraryTarget>("tbb");
    {
        tbb +=
            "include/.*"_rr,
            "src/tbb/.*\\.cpp"_rr,
            "src/tbb/.*\\.h"_rr;
        tbb ^= "include/tbb/tbbmalloc_proxy.h";
        tbb -= "src/tbb/.*\\.def"_rr;
        tbb.patch("src/tbb/governor.cpp", "return pteb->StackBase;", "return (std::uintptr_t)pteb->StackBase;");

        tbb += "org.sw.demo.open_mpi.hwloc"_dep;

        tbb.Private += "__TBB_BUILD"_d;
        tbb.Public += "__TBB_NO_IMPLICIT_LINKAGE"_d;
        tbb.Public += "__TBBMALLOC_NO_IMPLICIT_LINKAGE"_d;
        if (tbb.getBuildSettings().TargetOS.Type != OSType::Windows)
            tbb.Public += "USE_PTHREAD"_d;
        else
            tbb.Public += "USE_WINTHREAD"_d;

        tbb += path("src") / "tbb" / "def" / make_def_fn(tbb, "tbb");
    }

    // tbbbind is not separate lib
    //tbb -= "src/tbb/tbb_bind.cpp";

    auto &tbbmalloc = tbb.addTarget<SharedLibraryTarget>("malloc");
    {
        tbbmalloc +=
            "include/.*"_rr,
            "src/tbbmalloc/.*\\.cpp"_rr,
            "src/tbbmalloc/.*\\.def"_rr,
            "src/tbbmalloc/.*\\.h"_rr;
        tbbmalloc -=
            "src/tbbmalloc/.*\\.def"_rr;

        // disable autolink
        tbbmalloc.patch("include/tbb/tbbmalloc_proxy.h", "#pragma comment", "//#pragma  comment");

        tbbmalloc.Private += "__TBBMALLOC_BUILD"_d;
        tbbmalloc.Public += "__TBB_NO_IMPLICIT_LINKAGE"_d;
        tbbmalloc.Public += "__TBBMALLOC_NO_IMPLICIT_LINKAGE"_d;
        if (tbbmalloc.getBuildSettings().Native.LibrariesType == LibraryType::Static)
            tbbmalloc += "__TBB_SOURCE_DIRECTLY_INCLUDED"_def;
        if (tbbmalloc.getBuildSettings().TargetOS.Type != OSType::Windows)
            tbbmalloc.Public += "USE_PTHREAD"_d;
        else
            tbbmalloc.Public += "USE_WINTHREAD"_d;

        tbbmalloc += path("src") / "tbbmalloc" / "def" / make_def_fn(tbb, "tbbmalloc");
    }

    auto &tbbmalloc_proxy = tbbmalloc.addTarget<SharedLibraryTarget>("proxy");
    {
        tbbmalloc_proxy +=
            "include/.*"_rr,
            "src/tbbmalloc_proxy.*"_rr;
        tbbmalloc_proxy -=
            "src/tbbmalloc_proxy/.*\\.def"_rr;
        tbbmalloc_proxy ^= "include/tbb/tbbmalloc_proxy.h";

        tbbmalloc_proxy.Public += "__TBB_NO_IMPLICIT_LINKAGE"_d;
        tbbmalloc_proxy.Public += "__TBBMALLOC_NO_IMPLICIT_LINKAGE"_d;
        if (tbbmalloc_proxy.getBuildSettings().TargetOS.Type != OSType::Windows)
            tbbmalloc_proxy.Public += "USE_PTHREAD"_d;
        else
            tbbmalloc_proxy.Public += "USE_WINTHREAD"_d;

        tbbmalloc_proxy.Public += tbbmalloc;

        if (tbbmalloc_proxy.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            if (tbbmalloc_proxy.getBuildSettings().TargetOS.is(ArchType::x86_64))
                tbbmalloc_proxy.Interface.LinkOptions.push_back("-INCLUDE:__TBB_malloc_proxy");
            else if (tbbmalloc_proxy.getBuildSettings().TargetOS.is(ArchType::x86))
                tbbmalloc_proxy.Interface.LinkOptions.push_back("-INCLUDE:___TBB_malloc_proxy");
        }

        // only linux present
        //tbbmalloc_proxy += path("src") / "tbbmalloc" / "def" / make_def_fn(tbb, "proxy");
    }
}
