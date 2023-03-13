void build(Solution &s)
{
    auto &t = s.addLibrary("podofo", "0.9.8");
    t += RemoteFile("https://sourceforge.net/projects/podofo/files/podofo/{v}/podofo-{v}.tar.gz");

    t += "src/.*"_rr;
    t -= "src/.*\\.rc"_rr;
    t -= "src/podofo/doc/PdfTTFWriter.cpp";

    t += "src"_idir;
    t.Public += "src"_idir;
    t += "src/podofo"_idir;

    t += "BUILDING_PODOFO"_def;
    t += sw::Shared, "COMPILING_SHARED_PODOFO"_def;
    t.Interface += sw::Shared, "USING_SHARED_PODOFO"_def;
    t += sw::Shared, "PODOFO_HAVE_GCC_SYMBOL_VISIBILITY"_def;

    t.Variables["PODOFO_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
    t.Variables["PODOFO_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
    t.Variables["PODOFO_VERSION_PATCH"] = t.Variables["PACKAGE_VERSION_PATCH"];

    t.Variables["PODOFO_MULTI_THREAD"] = 1;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        t.Variables["PODOFO_HAVE_WINSOCK2_H"] = 1;
    else
    {
        t.Variables["PODOFO_HAVE_ARPA_INET_H"] = 1;
        t.Variables["PODOFO_HAVE_MEM_H"] = 1;
        t.Variables["PODOFO_HAVE_STRINGS_H"] = 1;
    }
    t.Variables["PODOFO_HAVE_CTYPE_H"] = 1;

    t.Variables["PODOFO_HAVE_STDINT_H"] = 1;
    t.Variables["PODOFO_HAVE_BASETSD_H"] = 1;
    t.Variables["PODOFO_HAVE_SYS_TYPES_H"] = 1;

    t.Variables["SZ_INT64"] = 8;
    t.Variables["SZ_LONG"] = 8;

    t.Variables["TEST_BIG"] = t.Variables["WORDS_BIGENDIAN"];

    for (auto sz : {"8", "16", "32", "64"})
        t.Variables["PDF_UINT"s + sz + "_TYPENAME"] = "uint"s + sz + "_t";
    for (auto sz : {"8", "16", "32", "64"})
        t.Variables["PDF_INT"s + sz + "_TYPENAME"] = "int"s + sz + "_t";

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        t += "gdi32.lib"_slib;
        t += "user32.lib"_slib;
        t += "ws2_32.lib"_slib;
    }

    // PODOFO_NO_FONTMANAGER
    // PODOFO_HAVE_CPPUNIT
    /*#cmakedefine PODOFO_HAVE_LIBIDN
    #cmakedefine PODOFO_HAVE_UNISTRING_LIB*/
    t.Variables["PODOFO_HAVE_JPEG_LIB"] = 1;
    t.Variables["PODOFO_HAVE_PNG_LIB"] = 1;
    t.Variables["PODOFO_HAVE_TIFF_LIB"] = 1;
    t.Variables["PODOFO_HAVE_FONTCONFIG"] = 1;
    t.Variables["PODOFO_HAVE_LUA"] = 1;
    t.Variables["PODOFO_HAVE_BOOST"] = 1;
    t.Variables["PODOFO_HAVE_OPENSSL"] = 1;
    t.Variables["PODOFO_HAVE_OPENSSL_1_1"] = 1;

    t += "org.sw.demo.boost.config"_dep;
    t += "org.sw.demo.freedesktop.fontconfig.fontconfig"_dep;
    t += "org.sw.demo.jpeg"_dep;
    t += "org.sw.demo.openssl.ssl"_dep;
    t += "org.sw.demo.tiff"_dep;

    t.configureFile("podofo_config.h.in", "podofo_config.h");
}
