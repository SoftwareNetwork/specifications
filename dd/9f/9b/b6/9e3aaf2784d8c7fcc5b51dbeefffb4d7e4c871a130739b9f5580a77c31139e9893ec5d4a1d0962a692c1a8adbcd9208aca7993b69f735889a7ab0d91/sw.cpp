void build(Solution &s)
{
    auto &p = s.addTarget<Library>("fltk", "1.3.5");
    p += Git("https://github.com/fltk/fltk", "release-{M}.{m}.{p}");

    p.setChecks("fltk");

    p += "FL/.*"_rr;
    p += "src/.*"_r;
    p += "cairo/.*"_r;

    p +=
        "src/xutf8/is_right2left.c",
        "src/xutf8/is_spacing.c",
        "src/xutf8/case.c",
        "src/xutf8/utf8Input.c",
        "src/xutf8/utf8Utils.c",
        "src/xutf8/utf8Wrap.c",
        "src/xutf8/keysym2Ucs.c"
        ;

    p += "FL_LIBRARY"_def;
    p += sw::Shared, "FL_DLL"_def;

    p += "."_idir;

    p -= "src/.*_[wW][iI][nN]32\\..*"_r;
    p -= "src/.*_[mM][aA][cC]\\..*"_r;
    p -= "src/.*_x\\..*"_r;
    p -= "src/.*_xft\\..*"_r;
    p -= "src/.*_posix\\..*"_r;
    p -= "src/.*_[xX]11\\..*"_r;
    p -= "src/.*_GTK\\..*"_r;
    p -= "src/.*_FLTK\\..*"_r;
    p -= "src/.*_GDI.*"_r;

    p -= "src/cmap.cxx"_r;
    p -= "src/dump_compose.c"_r;
    p -= "src/print_panel.cxx"_r;

    p += "org.sw.demo.cairographics.cairo"_dep;
    p += "org.sw.demo.jpeg"_dep;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        p.Public += "WIN32"_def;
        p += "opengl32.lib"_slib;
    }

    p.Variables["FL_ABI_VERSION"] = p.Variables["PACKAGE_VERSION_MAJOR"].toString() + "0" +
        p.Variables["PACKAGE_VERSION_MINOR"].toString() + "0" + p.Variables["PACKAGE_VERSION_PATCH"].toString();

    p.Variables["HAVE_LIBPNG"] = 1;
    p.Variables["HAVE_LIBZ"] = 1;
    p.Variables["HAVE_LIBJPEG"] = 1;
    p.Variables["FLTK_HAVE_CAIRO"] = 1;

    p.Variables["HAVE_GL"] = 1;
    p.Variables["HAVE_PNG_H"] = 1;
    p.Variables["FLTK_USE_CAIRO"] = 1;

    p.Variables["U16"] = "unsigned short";
    p.Variables["U32"] = "unsigned int";
    p.Variables["U64"] = "unsigned long long";

    p.configureFile("configh.cmake.in", p.BinaryPrivateDir / "config.h");
    p.configureFile("abi-version.cmake.in", "FL/abi-version.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("fltk");
    s.checkFunctionExists("localeconv");
    s.checkFunctionExists("scandir");
    s.checkFunctionExists("strcasecmp");
    s.checkFunctionExists("strlcat");
    s.checkFunctionExists("strlcpy");
    s.checkFunctionExists("vsnprintf");
    s.checkIncludeExists("alsa/asoundlib.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("GL/gl.h");
    s.checkIncludeExists("GL/glu.h");
    s.checkIncludeExists("jpeglib.h");
    s.checkIncludeExists("libpng/png.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("png.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/stdtypes.h");
    s.checkIncludeExists("X11/Xregion.h");
    s.checkIncludeExists("zlib.h");
    s.checkTypeSize("int");
    s.checkTypeSize("long");
    s.checkTypeSize("short");
    s.checkLibraryFunctionExists("fontconfig", "FcPatternCreate");
    s.checkLibraryFunctionExists("GL", "glXGetProcAddressARB");
    s.checkLibraryFunctionExists("GL", "glXMakeCurrent");
    s.checkLibraryFunctionExists("z", "gzgets");
    s.checkLibraryFunctionExists("jpeg", "jpeg_CreateCompress");
    s.checkLibraryFunctionExists("png", "png_get_valid");
    s.checkLibraryFunctionExists("png", "png_read_info");
    s.checkLibraryFunctionExists("png", "png_set_tRNS_to_alpha");
    s.checkLibraryFunctionExists("Xcursor", "XcursorImageCreate");
    s.checkLibraryFunctionExists("Xext", "XdbeQueryExtension");
    s.checkLibraryFunctionExists("Xfixes", "XFixesQueryExtension");
    s.checkLibraryFunctionExists("Xft", "XftDrawCreate");
    s.checkLibraryFunctionExists("Xinerama", "XineramaIsActive");
    s.checkLibraryFunctionExists("Xrender", "XRenderQueryVersion");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
    s.checkSourceCompiles("HAVE_LONG_LONG)", R"sw_xxx(

     int main() {

    long long int i;

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_SCANDIR", R"sw_xxx(#include <dirent.h>
            int func (const char *d, dirent ***list, void *sort) {
        int n = scandir(d, list, 0, (int(*)(const dirent **, const dirent **))sort);
        return n;
    }
    int main() {

        ; return 0; })sw_xxx");
}
