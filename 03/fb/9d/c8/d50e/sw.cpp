void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &fontconfig = s.addTarget<LibraryTarget>("freedesktop.fontconfig.fontconfig", "2.12.6");
    fontconfig.Source = RemoteFile("https://www.freedesktop.org/software/fontconfig/release/fontconfig-{v}.tar.bz2");

    fontconfig.ApiName = "SW_FONTCONFIG_LIBRARY_API";
    fontconfig.setChecks("fontconfig");

    fontconfig +=
        "fc-blanks/fcblanks.h",
        "fc-case/fccase.h",
        "fc-glyphname/fcglyphname.h",
        "fc-lang/fclang.h",
        "fontconfig/.*\\.h"_rr,
        "src/.*\\.c"_rr,
        "src/.*\\.h"_rr;

    fontconfig.Public +=
        "src"_id;

    fontconfig.Public += "FLEXIBLE_ARRAY_MEMBER=1"_d;
    fontconfig.Public += "HAVE_DIRENT_H=1"_d;
    fontconfig.Public += "HAVE_FT_GET_BDF_PROPERTY=1"_d;
    fontconfig.Public += "HAVE_FT_GET_NEXT_CHAR=1"_d;
    fontconfig.Public += "HAVE_FT_GET_PS_FONT_INFO=1"_d;
    fontconfig.Public += "HAVE_FT_HAS_PS_GLYPH_NAMES=1"_d;
    fontconfig.Public += "HAVE_FT_SELECT_SIZE=1"_d;
    fontconfig += "FC_GPERF_SIZE_T=size_t"_d;
    if (s.Settings.TargetOS.Type != OSType::Windows)
    {
        fontconfig.Public += "FC_CACHEDIR=\"~/.fontconfig/cache/fontconfig\""_d;
        fontconfig.Public += "FC_DEFAULT_FONTS=\"/usr/share/fonts\""_d;
    }
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        fontconfig.Public += "FC_CACHEDIR=\"LOCAL_APPDATA_FONTCONFIG_CACHE\""_d;
        fontconfig.Public += "FC_DEFAULT_FONTS=\"WINDOWSFONTDIR\""_d;
    }

    fontconfig.Public += "pub.cppan2.demo.expat-2"_dep;
    fontconfig.Public += "pub.cppan2.demo.freetype-2"_dep;
    fontconfig.Public += "pub.cppan2.demo.tronkko.dirent-master"_dep;

    fontconfig.writeFileOnce("fcaliastail.h", "", true);
    fontconfig.writeFileOnce("fcftaliastail.h", "", true);
    fontconfig.writeFileOnce("fcstdint.h", "#include <stdint.h>", true);

    fontconfig.replaceInFileOnce("fontconfig/fontconfig.h", "#define FcPublic", "#define FcPublic extern SW_FONTCONFIG_LIBRARY_API");

    if (s.Settings.TargetOS.Type == OSType::Windows)
        fontconfig.writeFileOnce("unistd.h", R"(
 #pragma once
        #include <stdlib.h>
        #include <io.h>
        #include <process.h>
        #include <direct.h>

        #define srandom srand
        #define random rand

        #define R_OK    4       /* Test for read permission.  */
        #define W_OK    2       /* Test for write permission.  */
        //#define   X_OK    1       /* execute permission - unsupported in windows*/
        #define F_OK    0       /* Test for existence.  */

        typedef int mode_t;
)", true);
}

void check(Checker &c)
{
    auto &s = c.addSet("fontconfig");
    s.checkFunctionExists("fstatfs");
    s.checkFunctionExists("fstatvfs");
    s.checkFunctionExists("getexecname");
    s.checkFunctionExists("getopt");
    s.checkFunctionExists("getopt_long");
    s.checkFunctionExists("getprogname");
    s.checkFunctionExists("link");
    s.checkFunctionExists("lrand48");
    s.checkFunctionExists("lstat");
    s.checkFunctionExists("mkdtemp");
    s.checkFunctionExists("mkostemp");
    s.checkFunctionExists("mkstemp");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("rand");
    s.checkFunctionExists("random");
    s.checkFunctionExists("random_r");
    s.checkFunctionExists("rand_r");
    s.checkFunctionExists("readlink");
    s.checkFunctionExists("vprintf");
    s.checkFunctionExists("_mktemp_s");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("sched.h");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/mount.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/statfs.h");
    s.checkIncludeExists("sys/statvfs.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/vfs.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("pid_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    s.checkTypeAlignment("double");
    s.checkTypeAlignment("void *");
    {
        auto &c = s.checkSymbolExists("posix_fadvise");
        c.Parameters.Includes.push_back("fcntl.h");
    }
    {
        auto &c = s.checkStructMemberExists("FT_Bitmap_Size", "y_ppem");
    }
    {
        auto &c = s.checkStructMemberExists("struct dirent", "d_type");
        c.Parameters.Includes.push_back("dirent.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct statfs", "f_flags");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct statfs", "f_fstypename");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct statvfs", "f_basetype");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct statvfs", "f_fstypename");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_mtim");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("TT_OS2", "usLowerOpticalPointSize");
    }
    {
        auto &c = s.checkStructMemberExists("TT_OS2", "usUpperOpticalPointSize");
    }
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkDeclarationExists("__SUNPRO_C");
}
