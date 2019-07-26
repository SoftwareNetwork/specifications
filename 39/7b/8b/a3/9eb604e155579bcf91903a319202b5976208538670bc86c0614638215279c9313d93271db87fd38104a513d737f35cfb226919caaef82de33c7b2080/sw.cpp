void build(Solution &s)
{
    auto &hdf = s.addProject("hdfgroup.hdf5", "1.10.5");
    hdf += RemoteFile("https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-{M}.{m}/hdf5-{v}/src/hdf5-{v}.tar.bz2");

    auto &tools = hdf.addDirectory("tools");

    auto &hdf5 = hdf.addTarget<LibraryTarget>("hdf5");
    {
        hdf5.setChecks("hdf5");

        hdf5 +=
            "config/cmake/H5pubconf.h.in",
            "src/.*\\.c"_rr,
            "src/.*\\.h"_rr;

        hdf5 -=
            "src/H5detect.c",
            "src/H5make_libsettings.c";

        hdf5.Public +=
            "src"_id;

        if (hdf5.getCompilerType() == CompilerType::MSVC)
        {
            hdf5.Public += "H5_HAVE_VISUAL_STUDIO"_d;
        }
        hdf5.Private += sw::Shared, "hdf5_shared_EXPORTS"_d;
        hdf5.Public += sw::Shared, "H5_BUILT_AS_DYNAMIC_LIB"_d;

        if (hdf5.getSettings().TargetOS.Type == OSType::Windows)
        {
            hdf5.Variables["H5_DEFAULT_PLUGINDIR"] = "%ALLUSERSPROFILE%\\\\hdf5\\\\lib\\\\plugin";
            hdf5.Variables["H5_HAVE_WIN32_API"] = 1;
        }
        else
        {
            hdf5.Variables["H5_DEFAULT_PLUGINDIR"] = "/usr/local/hdf5/lib/plugin";
        }

        hdf5.Variables["H5_PRINTF_LL_WIDTH"] = "\"ll\"";
        hdf5.Variables["H5_FC_FUNC"] = "H5_FC_FUNC(name,NAME) name ## _";
        hdf5.Variables["H5_FC_FUNC_"] = "H5_FC_FUNC_(name,NAME) name ## _";

        hdf5.replaceInFileOnce("config/cmake/H5pubconf.h.in", "/* #cmakedefine H5_MEMORY_ALLOC_SANITY_CHECK @H5_MEMORY_ALLOC_SANITY_CHECK@ ** Define in CMakeLists.txt */", "");
        hdf5.configureFile("config/cmake/H5pubconf.h.in", "H5pubconf.h");

        //
        auto &H5detect = tools.addExecutable("H5detect");
        H5detect += "src/H5detect.c";
        (H5detect + hdf5)->IncludeDirectoriesOnly = true;
        if (hdf5.getSettings().TargetOS.Type == OSType::Windows)
            H5detect += "ws2_32.lib"_lib;

        auto &H5make_libsettings = tools.addExecutable("H5make_libsettings");
        H5make_libsettings += "src/H5make_libsettings.c";
        (H5make_libsettings + hdf5)->IncludeDirectoriesOnly = true;
        if (hdf5.getSettings().TargetOS.Type == OSType::Windows)
            H5make_libsettings += "ws2_32.lib"_lib;

        {
            auto c = hdf5.addCommand();
            c << cmd::prog(H5detect)
                << cmd::std_out("H5Tinit.c")
                ;
        }

        {
            auto c = hdf5.addCommand();
            c << cmd::prog(H5make_libsettings)
                << cmd::std_out("H5make_libsettings.c")
                ;
        }
    }

    auto &hdf5cpp = hdf.addTarget<LibraryTarget>("hdf5cpp");
    hdf5cpp.setRootDirectory("c++/src");
    hdf5cpp.Private += sw::Shared, "hdf5_cpp_shared_EXPORTS"_d;
    hdf5cpp.Public += sw::Shared, "H5_BUILT_AS_DYNAMIC_LIB"_d;
    hdf5cpp.Public += hdf5;

    auto &hdf5hl = hdf.addTarget<LibraryTarget>("hdf5hl");
    hdf5hl.setRootDirectory("hl/src");
    hdf5hl.Private += sw::Shared, "hdf5_hl_shared_EXPORTS"_d;
    hdf5hl.Public += sw::Shared, "H5_BUILT_AS_DYNAMIC_LIB"_d;
    hdf5hl.Public += hdf5;

    auto &hdf5hlcpp = hdf.addTarget<LibraryTarget>("hdf5hlcpp");
    hdf5hlcpp.setRootDirectory("hl/c++/src");
    hdf5hlcpp.Private += sw::Shared, "hdf5_hl_cpp_shared_EXPORTS"_d;
    hdf5hlcpp.Public += sw::Shared, "H5_BUILT_AS_DYNAMIC_LIB"_d;
    hdf5hlcpp.Public += hdf5cpp, hdf5hl;
}

void check(Checker &c)
{
    auto &s = c.addSet("hdf5");
    s.checkFunctionExists("alarm");
    s.checkFunctionExists("asprintf");
    s.checkFunctionExists("clock_gettime");
    s.checkFunctionExists("compress2");
    s.checkFunctionExists("difftime");
    s.checkFunctionExists("fcntl");
    s.checkFunctionExists("flock");
    s.checkFunctionExists("fork");
    s.checkFunctionExists("frexpf");
    s.checkFunctionExists("frexpl");
    s.checkFunctionExists("GetConsoleScreenBufferInfo");
    s.checkFunctionExists("gethostname");
    s.checkFunctionExists("getpwuid");
    s.checkFunctionExists("getrusage");
    s.checkFunctionExists("gettextinfo");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("ioctl");
    s.checkFunctionExists("longjmp");
    s.checkFunctionExists("lstat");
    s.checkFunctionExists("posix_memalign");
    s.checkFunctionExists("random");
    s.checkFunctionExists("rand_r");
    s.checkFunctionExists("setjmp");
    s.checkFunctionExists("setsysinfo");
    s.checkFunctionExists("siglongjmp");
    s.checkFunctionExists("signal");
    s.checkFunctionExists("sigprocmask");
    s.checkFunctionExists("sigsetjmp");
    s.checkFunctionExists("srandom");
    s.checkFunctionExists("strdup");
    s.checkFunctionExists("symlink");
    s.checkFunctionExists("system");
    s.checkFunctionExists("tmpfile");
    s.checkFunctionExists("vasprintf");
    s.checkFunctionExists("vsnprintf");
    s.checkFunctionExists("waitpid");
    s.checkFunctionExists("_getvideoconfig");
    s.checkFunctionExists("_scrsize");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("dmalloc.h");
    s.checkIncludeExists("features.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("mach/mach_time.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("mpe.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("quadmath.h");
    s.checkIncludeExists("setjmp.h");
    s.checkIncludeExists("stdbool.h");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/file.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/timeb.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("szlib.h");
    s.checkIncludeExists("time.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("winsock2.h");
    s.checkIncludeExists("zlib.h");
    s.checkTypeSize("bool");
    s.checkTypeSize("char");
    s.checkTypeSize("double");
    s.checkTypeSize("float");
    s.checkTypeSize("int");
    s.checkTypeSize("int16_t");
    s.checkTypeSize("int32_t");
    s.checkTypeSize("int64_t");
    s.checkTypeSize("int8_t");
    s.checkTypeSize("int_fast16_t");
    s.checkTypeSize("int_fast32_t");
    s.checkTypeSize("int_fast64_t");
    s.checkTypeSize("int_fast8_t");
    s.checkTypeSize("int_least16_t");
    s.checkTypeSize("int_least32_t");
    s.checkTypeSize("int_least64_t");
    s.checkTypeSize("int_least8_t");
    s.checkTypeSize("long");
    s.checkTypeSize("long double");
    s.checkTypeSize("long long");
    s.checkTypeSize("off_t");
    s.checkTypeSize("ptrdiff_t");
    s.checkTypeSize("short");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("uint16_t");
    s.checkTypeSize("uint32_t");
    s.checkTypeSize("uint64_t");
    s.checkTypeSize("uint8_t");
    s.checkTypeSize("uint_fast16_t");
    s.checkTypeSize("uint_fast32_t");
    s.checkTypeSize("uint_fast64_t");
    s.checkTypeSize("uint_fast8_t");
    s.checkTypeSize("uint_least16_t");
    s.checkTypeSize("uint_least32_t");
    s.checkTypeSize("uint_least64_t");
    s.checkTypeSize("uint_least8_t");
    s.checkTypeSize("unsigned");
    s.checkTypeSize("void *");
    s.checkTypeSize("_Quad");
    s.checkTypeSize("__float128");
    s.checkTypeSize("__int64");
    s.checkLibraryFunctionExists("m", "ceil");
    s.checkLibraryFunctionExists("z", "compress2");
    s.checkLibraryFunctionExists("dl", "dlopen");
    s.checkLibraryFunctionExists("dmalloc", "dmalloc_shutdown");
    s.checkLibraryFunctionExists("mpe", "MPE_Init_log");
    s.checkLibraryFunctionExists("pthread", "pthread_self");
    s.checkLibraryFunctionExists("sz", "SZ_BufftoBuffCompress");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
    s.checkSourceCompiles("DEV_T_IS_SCALAR", R"sw_xxx(
    #ifdef HAVE_SYS_TYPES_H
    #include <sys/types.h>
    #endif


     int main() {

    dev_t d1, d2; if(d1==d2) return 0;

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_ATTRIBUTE", R"sw_xxx(

     int main() {

    int __attribute__((unused)) x

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_C99_DESIGNATED_INITIALIZER", R"sw_xxx(

     int main() {


                    typedef struct {
                        int x;
                        union {
                            int i;
                            double d;
                        } u;
                    } di_struct_t;
                    di_struct_t x = {0, { .d = 0.0}};

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_C99_FUNC", R"sw_xxx(

     int main() {

     const char *fname = __func__;

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_FUNCTION", R"sw_xxx(

     int main() {

     const char *fname = __FUNCTION__;

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_INLINE", R"sw_xxx(static inline void f(void){return;};)sw_xxx");
    s.checkSourceCompiles("HAVE_MPI_MULTI_LANG_Comm", R"sw_xxx(
        #include <mpi.h>


     int main() {

    MPI_Comm c_comm; MPI_Comm_c2f(c_comm)

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_MPI_MULTI_LANG_Info", R"sw_xxx(#include <mpi.h>

     int main() {

    MPI_Info c_info; MPI_Info_c2f(c_info)

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_STRUCT_TEXT_INFO", R"sw_xxx(

     int main() {

    struct text_info w; w.screenwidth=0;

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_STRUCT_VIDEOCONFIG", R"sw_xxx(

     int main() {

    struct videoconfig w; w.numtextcols=0;

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_TIMEZONE", R"sw_xxx(
        #include <sys/time.h>
        #include <time.h>

     int main() {

    timezone=0;

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_TIME_WITH_SYS_TIME", R"sw_xxx(
    #include <time.h>
    #include <sys/time.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkSourceCompiles("HAVE_TIOCGETD", R"sw_xxx(
    #include <sys/ioctl.h>


     int main() {

    int w=TIOCGETD;

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_TIOCGWINSZ", R"sw_xxx(
    #include <sys/ioctl.h>


     int main() {

    int w=TIOCGWINSZ;

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_TM_GMTOFF", R"sw_xxx(
      #include <sys/time.h>
      #include <time.h>


     int main() {

    struct tm tm; tm.tm_gmtoff=0;

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE___INLINE", R"sw_xxx(static __inline void f(void){return;};)sw_xxx");
    s.checkSourceCompiles("HAVE___INLINE__", R"sw_xxx(static __inline__ void f(void){return;};)sw_xxx");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkDeclarationExists("decl");
    s.checkDeclarationExists("decl");

    for (auto &check : s.all)
        check->Prefixes.insert("H5_");
}
