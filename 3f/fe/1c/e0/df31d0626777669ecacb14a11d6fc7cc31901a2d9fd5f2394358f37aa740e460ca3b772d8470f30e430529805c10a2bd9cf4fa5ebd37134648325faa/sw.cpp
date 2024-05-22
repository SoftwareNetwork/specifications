void build(Solution &s)
{
    auto &p = s.addProject("Microsoft", "2022.17.10");
    p += Git("https://github.com/microsoft/STL", "vs-{M}-{m}.{p}");

    auto &stl = p.addLibrary("STL");
    {
        stl.setRootDirectory("stl");

        stl -= "src/locale_implib_pure.cpp";
        stl -= "src/locale0_implib.cpp";
        stl -= "src/mpiostream.cpp";
        stl -= "src/mexcptptr.cpp";
        stl -= "src/ulocale.cpp";
        stl -= "src/syserror_import_lib.cpp";
        stl -= "src/dllmain_satellite.cpp";

        stl += cpplatest;

        stl += "_CRTBLD"_def;
        stl += sw::Shared, "CRTDLL2"_def;
        stl += sw::Static, "_STATIC_CPPLIB"_def;

        //stl += "_VCRT_ALLOW_INTERNALS"_def;
        stl += "_HAS_OLD_IOSTREAMS_MEMBERS=1"_def;
        //stl += "_STL_CONCRT_SUPPORT"_def;

        // from cmake
        if (stl.getCompilerType() == CompilerType::MSVC) {
            stl.CompileOptions.push_back("/fastfail");
            stl.CompileOptions.push_back("/guard:cf");
            stl.CompileOptions.push_back("/Zp8");
            stl.CompileOptions.push_back("/permissive-");
            stl.CompileOptions.push_back("/Zc:threadSafeInit-");
            stl.CompileOptions.push_back("/Zl");
            // # /Z7 for MSVC, /Zi for MASM
            //stl.CompileOptions.push_back("/Z7");
        }

        if (stl.getBuildSettings().TargetOS.is(ArchType::x86_64))
            stl += "_AMD64_"_def;
        else if (stl.getBuildSettings().TargetOS.is(ArchType::x86))
            stl += "_X86_"_def;
        else if (stl.getBuildSettings().TargetOS.is(ArchType::aarch64)) {
            stl += "_ARM64_"_def;
            stl -= "src/.*\\.asm"_rr;
        } else if (stl.getBuildSettings().TargetOS.is(ArchType::arm)) {
            stl += "_ARM_"_def;
            stl -= "src/.*\\.asm"_rr;
        } else {
            throw SW_RUNTIME_ERROR("Unsupported arch");
        }

        //stl += "com.Microsoft.VisualStudio.VC.concrt"_dep;
        stl += "com.Microsoft.VisualStudio.VC.runtime"_dep;
        stl += "org.sw.demo.boost.math"_dep;

        // for _crtGetTempPath2W
        //stl -= "src/winapisupp.cpp";
        stl += "com.Microsoft.Windows.SDK.winrt"_dep;

        stl += "advapi32.lib"_slib;
        stl += "Synchronization.lib"_slib;
    }
}
