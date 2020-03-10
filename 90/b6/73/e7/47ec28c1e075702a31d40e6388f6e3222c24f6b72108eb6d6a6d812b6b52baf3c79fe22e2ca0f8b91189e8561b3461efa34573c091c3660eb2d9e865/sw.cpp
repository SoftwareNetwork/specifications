void build(Solution &s)
{
    auto &t = s.addTarget<Library>("videolan.dav1d", "0.6.0");
    t += Git("https://code.videolan.org/videolan/dav1d");

    auto set_arch = [](auto &t)
    {
        switch (t.getBuildSettings().TargetOS.Arch)
        {
        case ArchType::aarch64:
            t += "ARCH_AARCH64"_def;
            t -= "src/x86/.*"_rr;
            break;
        case ArchType::arm:
            t += "ARCH_ARM"_def;
            t -= "src/x86/.*"_rr;
            break;
        default:
            t += "ARCH_X86"_def;
            t -= "src/arm/.*"_rr;
            t -= "src/ppc/.*"_rr;
            break;
        }
    };

    {
        t.setChecks("dav1d", true);
        t.setExtensionProgram(".asm", "org.sw.demo.nasm"_dep);

        t.ApiName = "DAV1D_API";

        t += "include/.*"_rr;
        t += "src/.*"_rr;
        t -= "src/.*\\.asm"_rr;
        t -= "src/.*_tmpl.c"_rr;
        t -= "src/itx_1d.c";
        t -= "src/ext/.*"_rr;
        t -= "src/win32/.*"_rr;

        set_arch(t);

        t.Public += "include"_idir;
        t.Protected += "include/dav1d"_idir;
        t.Protected += "."_idir;
        t.Protected += "src"_idir;
        if (t.getCompilerType() == CompilerType::MSVC)
            t.Protected += "include/compat/msvc"_idir;
        else
            t.Protected += "include/compat/gcc"_idir;
        if (t.getBuildSettings().TargetOS.is(OSType::Windows))
            t += "src/win32/.*"_rr;

        t.writeFileOnce(t.BinaryPrivateDir / "config.asm");
        t.writeFileOnce(t.BinaryPrivateDir / "config.h", R"(
#include <stdint.h>

#define CONFIG_8BPC 1
#define CONFIG_16BPC 1
)");

        t.Variables["DAV1D_API_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
        t.Variables["DAV1D_API_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
        t.Variables["DAV1D_API_VERSION_PATCH"] = t.Variables["PACKAGE_VERSION_PATCH"];
        t.configureFile("include/dav1d/version.h.in", "version.h");

        t.configureFile("include/vcs_version.h.in", "vcs_version.h");
    }

    for (auto depth : { 8, 16 })
    {
        auto &d = t.addStaticLibrary("bitdepth" + std::to_string(depth));

        d += Definition("BITDEPTH=" + std::to_string(depth));

        d.setChecks("dav1d", true);
        d.setExtensionProgram(".asm", "org.sw.demo.nasm"_dep);

        d += "src/.*\\.asm"_rr;
        d += "src/.*_tmpl.c"_rr;
        d += IncludeDirectory(t.BinaryPrivateDir);

        set_arch(d);

        (d + t)->IncludeDirectoriesOnly = true;
        t += d;
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("dav1d");
    s.checkFunctionExists("_aligned_malloc", "HAVE_ALIGNED_MALLOC"s);
    s.checkFunctionExists("memalign");
    s.checkFunctionExists("posix_memalign");
}
