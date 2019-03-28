void build(Solution &s)
{
    auto &fontconfig = s.addTarget<LibraryTarget>("freedesktop.fontconfig.fontconfig", "2.13.1");
    fontconfig += RemoteFile("https://www.freedesktop.org/software/fontconfig/release/fontconfig-{v}.tar.bz2");

    fontconfig.ApiName = "SW_FONTCONFIG_LIBRARY_API";
    fontconfig.setChecks("fontconfig", true);

    fontconfig +=
        "fc-case/fccase.h",
        "fc-lang/fclang.h",
        "fontconfig/.*\\.h"_rr,
        "src/.*\\.c"_rr,
        "src/.*\\.h"_rr;

    fontconfig.Public +=
        "."_id;
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
    fontconfig += "FC_TEMPLATEDIR=\"fontconfig/conf.avail\""_d;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        fontconfig.Public += "FC_CACHEDIR=\"LOCAL_APPDATA_FONTCONFIG_CACHE\""_d;
        fontconfig.Public += "FC_DEFAULT_FONTS=\"WINDOWSFONTDIR\""_d;
    }
    else
    {
        fontconfig.Public += "FC_CACHEDIR=\"~/.fontconfig/cache/fontconfig\""_d;
        fontconfig.Public += "FC_DEFAULT_FONTS=\"/usr/share/fonts\""_d;
        fontconfig.Public += "FONTCONFIG_PATH=\"/etc/fonts/conf.d\""_d;
        fontconfig.Public += "HAVE_INTEL_ATOMIC_PRIMITIVES"_d;
    }

    fontconfig.Public += "org.sw.demo.expat-2"_dep;
    fontconfig.Public += "org.sw.demo.freetype-2"_dep;
    fontconfig.Public -= "org.sw.demo.tronkko.dirent-master"_dep;

    {
        auto o1 = fontconfig.BinaryDir / "fcobjshash1.h";
        auto o2 = fontconfig.BinaryDir / "fcobjshash2.h";
        auto o3 = fontconfig.BinaryDir / "fcobjshash.gperf";

        auto cc = std::static_pointer_cast<Compiler>(s.findProgramByExtension(".cpp")->clone());
        auto c = cc->getCommand(fontconfig);
        c->args.push_back("-I");
        c->args.push_back(fontconfig.SourceDir.u8string());
        c->args.push_back("-E");
        c->args.push_back((fontconfig.SourceDir / "src/fcobjshash.gperf.h").u8string());
        c->redirectStdout(o1);
        c->addInput(fontconfig.SourceDir / "src/fcobjshash.gperf.h");
        fontconfig.Storage.push_back(c);

        fontconfig.Public -= "org.sw.demo.gnu.sed.sed-*"_dep;
        fontconfig.Public -= "org.sw.demo.gnu.gawk.gawk-*"_dep;

        {
            auto c = fontconfig.addCommand();
            if (s.Settings.TargetOS.Type == OSType::Windows)
                c << cmd::prog("org.sw.demo.gnu.sed.sed-*"_dep);
            else
                c << "sed";
            c
                << "s/^ *//;s/ *, */,/"
                << cmd::std_in(o1)
                << cmd::std_out(o2)
                ;
        }

        {
            auto c = fontconfig.addCommand();
            if (s.Settings.TargetOS.Type == OSType::Windows)
                c << cmd::prog("org.sw.demo.gnu.gawk.gawk-*"_dep);
            else
                c << "awk";
            c
                << "\\\n\
		            /CUT_OUT_BEGIN/ { no_write=1; next; }; \\\n\
		            /CUT_OUT_END/ { no_write=0; next; }; \\\n\
		            /^$$/||/^#/ { next; }; \\\n\
		            { if (!no_write) print; next; }; \\\n\
	            "
                << cmd::std_in(o2)
                << cmd::std_out(o3)
                ;
        }

        {
            auto c = fontconfig.addCommand();
            c << cmd::prog("org.sw.demo.gnu.gperf-*"_dep)
                << cmd::wdir(fontconfig.BinaryDir)
                << "--pic"
                << "-m100"
                << cmd::in(o3)
                << cmd::std_out("fcobjshash.h")
                ;
        }
    }

    fontconfig.writeFileOnce("fcaliastail.h");
    fontconfig.writeFileOnce("fcftaliastail.h");
    fontconfig.writeFileOnce("fcstdint.h", "#include <stdint.h>");

    fontconfig.replaceInFileOnce("fontconfig/fontconfig.h", "#define FcPublic", "#define  FcPublic extern SW_FONTCONFIG_LIBRARY_API");

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        fontconfig.Public += "org.sw.demo.tronkko.dirent-master"_dep;

        fontconfig.replaceInFileOnce("src/fccache.c", "#include <sys/time.h>", "//#include <sys/time.h>");
        fontconfig.replaceInFileOnce("src/fcfreetype.c", "advances[3] = {};", "advances[3] = {0};");

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

            #include <stdint.h>
            #ifdef _WIN64
            #define ssize_t int64_t
            #else
            #define ssize_t int32_t
            #endif
)");
    }
    else
    {
        fontconfig.writeFileOnce("fcalias.h");
        fontconfig.writeFileOnce("fcftalias.h");
    }
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
