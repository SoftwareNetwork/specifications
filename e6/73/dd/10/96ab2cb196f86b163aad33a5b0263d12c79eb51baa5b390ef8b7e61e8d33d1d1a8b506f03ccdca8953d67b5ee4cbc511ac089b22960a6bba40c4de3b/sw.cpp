void build(Solution &s)
{
    auto &jasper = s.addTarget<LibraryTarget>("mdadams.jasper", "3.0.3");
    jasper += Git("https://github.com/mdadams/jasper", "version-{v}");

    jasper += c11;
    jasper += "JAS_EXPORT"_api;
    jasper.SourceDir /= "src/libjasper";
    jasper.setChecks("jas");
    jasper +=
        ".*\\.c"_rr,
        ".*\\.h"_rr,
        ".*\\.in"_rr
        ;
    jasper -= "heic/.*"_rr;

    jasper.Public += "org.sw.demo.jpeg"_dep;
    jasper.Public += "org.sw.demo.uclouvain.openjpeg.openjp2"_dep;

    jasper.Variables["JAS_VERSION_MAJOR"] = jasper.Variables["PACKAGE_VERSION_MAJOR"];
    jasper.Variables["JAS_VERSION_MINOR"] = jasper.Variables["PACKAGE_VERSION_MINOR"];
    jasper.Variables["JAS_VERSION_PATCH"] = jasper.Variables["PACKAGE_VERSION_PATCH"];
    jasper.Variables["JAS_STDC_VERSION"] = 0; // ignore
    jasper.Variables["JAS_DEFAULT_MAX_MEM_USAGE"] = 1073741824;

    jasper.Variables["JAS_INCLUDE_BMP_CODEC"] = 1;
    jasper.Variables["JAS_INCLUDE_JPG_CODEC"] = 1;
    jasper.Variables["JAS_ENABLE_JPG_CODEC"] = 1;
    jasper -= "jp2/.*"_rr;
    /*jasper.Variables["JAS_INCLUDE_JP2_CODEC"] = 1;
    jasper.Variables["JAS_ENABLE_JP2_CODEC"] = 1;
    jasper.Variables["JAS_ENABLE_JPC_CODEC"] = 1;*/

    jasper.Variables["JAS_THREADS"] = 1;
    if (jasper.getBuildSettings().TargetOS.Type == OSType::Windows ||
        jasper.getBuildSettings().TargetOS.Type == OSType::Mingw)
        jasper.Variables["JAS_THREADS_WIN32"] = 1;
    else
        jasper.Variables["JAS_THREADS_PTHREAD"] = 1;

    jasper.writeFileOnce("jasper/jas_export_cmake.h", "#define JAS_LOCAL");
    jasper.configureFile("include/jasper/jas_config.h.in", "jasper/jas_config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("jas");
    s.checkFunctionExists("unlink");
    s.checkFunctionExists("close");
    s.checkFunctionExists("lseek");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/time.h");
    s.checkTypeSize("int");
    s.checkTypeSize("long");
    s.checkTypeSize("long long", "SIZEOF_LLONG");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");

    for (auto &check : s.all)
        check->Prefixes.insert("JAS_");
}
