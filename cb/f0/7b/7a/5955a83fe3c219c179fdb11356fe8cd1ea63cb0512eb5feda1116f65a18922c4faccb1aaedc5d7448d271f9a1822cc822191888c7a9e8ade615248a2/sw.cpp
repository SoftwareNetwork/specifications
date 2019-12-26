void build(Solution &s)
{
    auto &p = s.addProject("Microsoft", "master");
    p += Git("https://github.com/microsoft/STL");

    auto &stl = p.addLibrary("STL");
    {
        stl.setRootDirectory("stl");

        stl -= "src/locale_implib_pure.cpp";
        stl -= "src/locale0_implib.cpp";
        stl -= "src/mpiostream.cpp";
        stl -= "src/mexcptptr.cpp";
        stl -= "src/ulocale.cpp";

        stl += cpp17;

        stl += "_CRTBLD"_def;
        stl += sw::Shared, "CRTDLL2"_def;
        stl += sw::Static, "_STATIC_CPPLIB"_def;

        stl += "_VCRT_ALLOW_INTERNALS"_def;
        stl += "_HAS_OLD_IOSTREAMS_MEMBERS=1"_def;
        stl += "_STL_CONCRT_SUPPORT"_def;

        if (stl.getBuildSettings().TargetOS.is(ArchType::x86_64))
            stl += "_AMD64_"_def;
        else if (stl.getBuildSettings().TargetOS.is(ArchType::x86))
            stl += "_X86_"_def;
        else if (stl.getBuildSettings().TargetOS.is(ArchType::aarch64))
            stl += "_ARM64_"_def;
        else if (stl.getBuildSettings().TargetOS.is(ArchType::arm))
            stl += "_ARM_"_def;
        else
            throw SW_RUNTIME_ERROR("Unsupported arch");

        stl += "com.Microsoft.VisualStudio.VC.concrt"_dep;
        stl += "com.Microsoft.VisualStudio.VC.runtime"_dep;
        stl += "com.Microsoft.Windows.SDK.winrt"_dep;
        stl += "org.sw.demo.boost.math"_dep;
    }
}
