void build(Solution &s)
{
    auto &t = s.addTarget<SharedLibrary>("poppler", "0.77.0");
    t += RemoteFile("https://poppler.freedesktop.org/poppler-{v}.tar.xz");

    t += "fofi/.*"_rr;
    t += "goo/.*"_rr;
    t += "splash/.*"_rr;
    t += "poppler/.*"_r;
    t += "cpp/.*"_r;

    t += "."_idir;
    t += "fofi"_idir;
    t += "goo"_idir;
    t += "splash"_idir;
    t += "poppler"_idir;

    t += "poppler_cpp_EXPORTS"_def;

    t += "org.sw.demo.freetype"_dep;
    t += "org.sw.demo.cairographics.cairo"_dep;
    t += "org.sw.demo.tiff"_dep;
    t += "org.sw.demo.mm2.LittleCMS-master"_dep;
    t += "org.sw.demo.badger.curl.libcurl"_dep;
    t += "org.sw.demo.gnu.iconv.libiconv"_dep;

    t.Variables["USE_CMS"] = 1;
    t.Variables["ENABLE_ZLIB"] = 1;
    t.Variables["ENABLE_LIBPNG"] = 1;
    t.Variables["ENABLE_LIBTIFF"] = 1;
    t.Variables["ENABLE_LIBJPEG"] = 1;

    t.Variables["POPPLER_VERSION"] = t.Variables["PACKAGE_VERSION"];
    t.Variables["POPPLER_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
    t.Variables["POPPLER_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
    t.Variables["POPPLER_VERSION_MICRO"] = t.Variables["PACKAGE_VERSION_PATCH"];

    /*
    #cmakedefine USE_FIXEDPOINT 1
    #cmakedefine USE_FLOAT 1
    #cmakedefine OPI_SUPPORT 1
    #cmakedefine TEXTOUT_WORD_LIST 1
    #cmakedefine POPPLER_HAS_CURL_SUPPORT 1
    #cmakedefine HAVE_DIRENT_H 1
    #cmakedefine HAVE_GETTIMEOFDAY 1
    #cmakedefine HAVE_NDIR_H 1
    #cmakedefine HAVE_SYS_DIR_H 1
    #cmakedefine HAVE_SYS_NDIR_H 1
    */

    t.configureFile("poppler/poppler-config.h.cmake", "poppler-config.h");
    t.configureFile("config.h.cmake", t.BinaryPrivateDir / "config.h");
    t.configureFile("cpp/poppler-version.h.in", "poppler-version.h");
}
