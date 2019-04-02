void build(Solution &s)
{
    auto &p = s.addProject("xmlsoft.xslt", "1.1.33");
    p += RemoteFile("https://gitlab.gnome.org/GNOME/libxslt/-/archive/v{v}/libxslt-v{v}.tar.bz2");

    auto &libxslt = p.addTarget<LibraryTarget>("libxslt");
    {
        libxslt.setChecks("libxslt");

        libxslt +=
            "libxslt/.*\\.c"_rr,
            "libxslt/.*\\.h"_rr,
            "libxslt/xsltconfig.h.in";

        libxslt.Public += sw::Static, "LIBXSLT_STATIC"_d;

        libxslt.Public += "org.sw.demo.xmlsoft.libxml2"_dep;

        libxslt.Variables["VERSION"] = libxslt.Variables["PACKAGE_VERSION"];
        libxslt.Variables["LIBXSLT_VERSION_NUMBER"] = libxslt.Variables["PACKAGE_VERSION_MAJOR"].toString() + "0" +
            libxslt.Variables["PACKAGE_VERSION_MINOR"].toString() + "0" + libxslt.Variables["PACKAGE_VERSION_PATCH"].toString();

        libxslt.Variables["WITH_XSLT_DEBUG"] = 0;
        libxslt.Variables["WITH_MEM_DEBUG"] = 0;
        libxslt.Variables["WITH_TRIO"] = 0;
        libxslt.Variables["WITH_DEBUGGER"] = 0;
        libxslt.Variables["WITH_MODULES"] = 0;

        libxslt.Variables["XSLT_LOCALE_WINAPI"] = 0;
        libxslt.Variables["XSLT_LOCALE_XLOCALE"] = 0;

        if (s.Settings.TargetOS.Type == OSType::Windows)
            libxslt.Variables["XSLT_LOCALE_WINAPI"] = 1;
        else
            libxslt.Variables["XSLT_LOCALE_XLOCALE"] = 1;

        libxslt.configureFile("libxslt/xsltconfig.h.in", "libxslt/xsltconfig.h");
    }

    auto &libexslt = p.addTarget<LibraryTarget>("libexslt");
    {
        libexslt.setChecks("libxslt");

        libexslt +=
            "libexslt/.*\\.c"_rr,
            "libexslt/.*\\.h"_rr,
            "libexslt/exsltconfig.h.in";

        libexslt.Public += sw::Static, "LIBEXSLT_STATIC"_d;
        libexslt.Public += libxslt;

        libexslt.Variables["VERSION"] = libexslt.Variables["PACKAGE_VERSION"];
        libexslt.Variables["LIBEXSLT_VERSION_NUMBER"] = libexslt.Variables["PACKAGE_VERSION_MAJOR"].toString() + "0" +
            libexslt.Variables["PACKAGE_VERSION_MINOR"].toString() + "0" + libexslt.Variables["PACKAGE_VERSION_PATCH"].toString();

        libexslt.Variables["WITH_CRYPTO"] = 0;

        libexslt.configureFile("libexslt/exsltconfig.h.in", "libexslt/exsltconfig.h");
        libexslt.writeFileOnce(libexslt.BinaryPrivateDir / "config.h");
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("libxslt");
    s.checkFunctionExists("asctime");
    s.checkFunctionExists("clock_gettime");
    s.checkFunctionExists("fabs");
    s.checkFunctionExists("floor");
    s.checkFunctionExists("fprintf");
    s.checkFunctionExists("ftime");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("gmtime");
    s.checkFunctionExists("gmtime_r");
    s.checkFunctionExists("localtime");
    s.checkFunctionExists("localtime_r");
    s.checkFunctionExists("mktime");
    s.checkFunctionExists("pow");
    s.checkFunctionExists("printf");
    s.checkFunctionExists("sprintf");
    s.checkFunctionExists("sscanf");
    s.checkFunctionExists("stat");
    s.checkFunctionExists("time");
    s.checkFunctionExists("vfprintf");
    s.checkFunctionExists("vsnprintf");
    s.checkFunctionExists("vsprintf");
    s.checkFunctionExists("_stat");
    s.checkIncludeExists("ansidecl.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("float.h");
    s.checkIncludeExists("fp_class.h");
    s.checkIncludeExists("ieeefp.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("math.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("nan.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("stdarg.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/timeb.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("time.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("xlocale.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
}
