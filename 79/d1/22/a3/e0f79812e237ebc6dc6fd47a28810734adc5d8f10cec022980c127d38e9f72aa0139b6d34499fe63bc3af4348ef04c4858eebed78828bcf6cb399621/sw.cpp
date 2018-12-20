void build(Solution &s)
{
    auto &grep = s.addProject("gnu.grep", "3.2.0");
    grep += RemoteFile("https://ftp.gnu.org/gnu/grep/grep-{M}.{m}.tar.xz");

    auto &gnulib = grep.addTarget<StaticLibraryTarget>("gnulib");
    {
        gnulib.setChecks("gnulib");

        gnulib +=
            "lib/.*"_rr;

        gnulib -=
            "lib/at-func.c",
            "lib/closedir.c",
            "lib/colorize-posix.c",
            "lib/fnmatch_loop.c",
            "lib/fts-cycle.c",
            "lib/iswblank.c",
            "lib/localcharset.c",
            "lib/localeconv.c",
            "lib/lseek.c",
            "lib/mbsinit.c",
            "lib/memchr.c",
            "lib/opendir.*"_rr,
            "lib/readdir.c",
            "lib/regcomp.c",
            "lib/regex_internal.c",
            "lib/regexec.c",
            "lib/strerror-override.c",
            "lib/strtoimax.c",
            "lib/strtol.c",
            "lib/strtoll.c";

        gnulib -=
            "lib/iconv.*"_rr;;

        gnulib.Public +=
            "lib"_id;

        gnulib.Private += "__USE_GNU"_d;

        gnulib.Public += "org.sw.demo.gnu.gawk.getopt"_dep;
        gnulib.Public -= "org.sw.demo.tronkko.dirent-master"_dep;

        if (s.Settings.TargetOS.Type == OSType::Windows)
        {
            gnulib.Public += "org.sw.demo.tronkko.dirent-master"_dep;
        }

        gnulib.replaceInFileOnce("lib/open.c", "return open (filename, flags, mode);", "return _open (filename, flags, mode);");
        gnulib.writeFileOnce(gnulib.BinaryPrivateDir / "stdalign.h");
        gnulib.writeFileOnce(gnulib.BinaryPrivateDir / "unistd.h");
        gnulib.writeFileOnce(gnulib.BinaryPrivateDir / "configmake.h");
        gnulib.writeFileOnce(gnulib.BinaryPrivateDir / "config.h",
"#define PACKAGE_NAME \"" + gnulib.Variables["PACKAGE_NAME"].toString() + "\"\n" +
"#define VERSION \"" + gnulib.Variables["PACKAGE_VERSION"].toString() + "\"\n" +
"#define PACKAGE_URL \"" + gnulib.Variables["PACKAGE_URL"].toString() + "\"\n" +
"#define PACKAGE_BUGREPORT \"" + gnulib.Variables["PACKAGE_BUGREPORT"].toString() + "\"\n" +
            R"(
#define mode_t int
#define nlink_t short

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
    #include <io.h>

    #ifdef _WIN64
    #define ssize_t long long
    #else
    #define ssize_t int
    #endif

    #define __builtin_expect(cond, v) (cond)
    #define lstat stat


    #ifndef O_NDELAY
    # define O_NDELAY 0
    #endif

    #ifndef O_NOATIME
    # define O_NOATIME 0
    #endif

    #ifndef O_NONBLOCK
    # define O_NONBLOCK O_NDELAY
    #endif

    #ifndef O_NOCTTY
    # define O_NOCTTY 0
    #endif

    #ifndef O_NOFOLLOW
    # define O_NOFOLLOW 0
    #endif

    #ifndef O_CLOEXEC
    #define O_CLOEXEC 0
    #endif

    /* Use the same bit pattern as Solaris 9, but with the proper
        signedness.  The bit pattern is important, in case this actually is
        Solaris with the above workaround.  */
    #ifndef AT_FDCWD
    # define AT_FDCWD (-3041965)
    #endif

    /* Use the same values as Solaris 9.  This shouldn't matter, but
        there's no real reason to differ.  */
    #ifndef AT_SYMLINK_NOFOLLOW
    # define AT_SYMLINK_NOFOLLOW 4096
    #endif
#endif

void *alloca (size_t);
void * memrchr (const void *, int, size_t);
struct DIR* fdopendir(int fd);
char *getcwd(char *buf, size_t size);
void *mempcpy(void * __dest, void const * __src,
    size_t __n);

#define FLEXIBLE_ARRAY_MEMBER

/* The __pure__ attribute was added in gcc 2.96.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
# define _GL_ATTRIBUTE_PURE __attribute__ ((__pure__))
#else
# define _GL_ATTRIBUTE_PURE /* empty */
#endif

#define HAVE_WORKING_O_NOFOLLOW 1

#ifndef O_DIRECTORY
# define O_DIRECTORY        0200000
#endif

#ifndef O_SEARCH
# define O_SEARCH        0
#endif

#define GNULIB_MSVC_NOTHROW 1
#define HAVE_DECL_STRERROR_R 1


#define _GL_ATTRIBUTE_FORMAT_PRINTF(x,y)
#define _GL_ARG_NONNULL(x)

#define PROMOTED_MODE_T int

#define UNSIGNED 1

#define _GL_INLINE inline
#define _GL_EXTERN_INLINE inline

# define _GL_INLINE_HEADER_BEGIN
# define _GL_INLINE_HEADER_END


/* The _Noreturn keyword of C11.  */
#if ! (defined _Noreturn \
        || (defined __STDC_VERSION__ && 201112 <= __STDC_VERSION__))
# if (3 <= __GNUC__ || (__GNUC__ == 2 && 8 <= __GNUC_MINOR__) \
        || 0x5110 <= __SUNPRO_C)
#  define _Noreturn __attribute__ ((__noreturn__))
# elif defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn
# endif
#endif

#define _GL_UNUSED
#define __getopt_argv_const const
#define _GL_ATTRIBUTE_CONST

#define	F_OK		0	/* test for existence of file */
#define	X_OK		0x01	/* test for execute or search permission */
#define	W_OK		0x02	/* test for write permission */
#define	R_OK		0x04	/* test for read permission */

/* command values */
#define	F_DUPFD		0		/* duplicate file descriptor */
#define	F_GETFD		1		/* get file descriptor flags */
#define	F_SETFD		2		/* set file descriptor flags */
#define	F_GETFL		3		/* get file status flags */
#define	F_SETFL		4		/* set file status flags */
#ifndef _POSIX_SOURCE
#define	F_GETOWN	5		/* get SIGIO/SIGURG proc/pgrp */
#define F_SETOWN	6		/* set SIGIO/SIGURG proc/pgrp */
#endif
#define	F_GETLK		7		/* get record locking information */
#define	F_SETLK		8		/* set record locking information */
#define	F_SETLKW	9		/* F_SETLK; wait if blocked */

/* file descriptor flags (F_GETFD, F_SETFD) */
#define	FD_CLOEXEC	1		/* close-on-exec flag */

/* record locking flags (F_GETLK, F_SETLK, F_SETLKW) */
#define	F_RDLCK		1		/* shared or read lock */
#define	F_UNLCK		2		/* unlock */
#define	F_WRLCK		3		/* exclusive or write lock */
#ifdef KERNEL
#define	F_WAIT		0x010		/* Wait until lock is granted */
#define	F_FLOCK		0x020	 	/* Use flock(2) semantics for lock */
#define	F_POSIX		0x040	 	/* Use POSIX semantics for lock */
#endif

# define F_DUPFD_CLOEXEC 1030        /* Duplicate file descriptor with
                                    close-on-exit set.  */

#define strcasecmp stricmp
#define strncasecmp strnicmp

#ifndef S_TYPEISSEM
# ifdef S_INSEM
#  define S_TYPEISSEM(p) (S_ISNAM ((p)->st_mode) && (p)->st_rdev == S_INSEM)
# else
#  define S_TYPEISSEM(p) 0
# endif
#endif

#ifndef S_TYPEISSHM
# ifdef S_INSHD
#  define S_TYPEISSHM(p) (S_ISNAM ((p)->st_mode) && (p)->st_rdev == S_INSHD)
# else
#  define S_TYPEISSHM(p) 0
# endif
#endif

#ifndef S_TYPEISMQ
# define S_TYPEISMQ(p) 0
#endif

#ifndef S_TYPEISTMO
# define S_TYPEISTMO(p) 0
#endif

#ifdef DIR_FD_MEMBER_NAME
# define DIR_TO_FD(Dir_p) ((Dir_p)->DIR_FD_MEMBER_NAME)
#else
# define DIR_TO_FD(Dir_p) -1
#endif

#define _GL_EXTERN_C extern

#define _GL_FUNCDECL_SYS(func,rettype,parameters_and_attributes) \
    _GL_EXTERN_C rettype func parameters_and_attributes

# define _GL_CXXALIAS_SYS(func,rettype,parameters) \
    _GL_EXTERN_C int _gl_cxxalias_dummy

#define HAVE_DECL_STRTOULL 1
        )");

        gnulib.Variables["GUARD_PREFIX"] = "_CPPAN_GNULIB";
        gnulib.Variables["INCLUDE_NEXT"] = "include";

        //#set(HAVE_DIRENT_H 1)

        gnulib.Variables["GNULIB_OPENDIR"] = "0";
        gnulib.Variables["GNULIB_READDIR"] = "0";
        gnulib.Variables["GNULIB_REWINDDIR"] = "0";
        gnulib.Variables["GNULIB_CLOSEDIR"] = "0";
        gnulib.Variables["GNULIB_DIRFD"] = "0";
        gnulib.Variables["GNULIB_FDOPENDIR"] = "0";
        gnulib.Variables["GNULIB_SCANDIR"] = "0";
        gnulib.Variables["GNULIB_ALPHASORT"] = "0";

        if (s.Settings.TargetOS.Type == OSType::Windows)
            gnulib.Variables["NEXT_DIRENT_H"] = "\"dirent-private.h\"";

        //#set(HAVE_GETOPT_H 1)

        gnulib.writeFileOnce(gnulib.BinaryPrivateDir / "alloca.h");

        //gnulib.configureFile("lib/getopt.in.h", "getopt.h");
        gnulib.configureFile("lib/fnmatch.in.h", "fnmatch.h");
        //gnulib.configureFile("lib/dirent.in.h", "dirent.h");
        gnulib.configureFile("lib/unistr.in.h", "unistr.h");
        gnulib.configureFile("lib/uniwidth.in.h", "uniwidth.h");
        gnulib.configureFile("lib/unitypes.in.h", "unitypes.h");

        gnulib.Variables["HAVE_LANGINFO_H"] = "0";
        gnulib.Variables["HAVE_LANGINFO_CODESET"] = "0";
        gnulib.Variables["HAVE_LANGINFO_T_FMT_AMPM"] = "0";
        gnulib.Variables["HAVE_LANGINFO_ERA"] = "0";
        gnulib.Variables["HAVE_LANGINFO_YESEXPR"] = "0";
        gnulib.Variables["GNULIB_NL_LANGINFO"] = "1";
        gnulib.Variables["REPLACE_NL_LANGINFO"] = "0";
        gnulib.Variables["HAVE_NL_LANGINFO"] = "0";
        gnulib.configureFile("lib/langinfo.in.h", "langinfo.h");

        gnulib.replaceInFileOnce("lib/getprogname.c", "# else", "#elif _WIN32\nreturn __argv[0]; \n# else\n");
        gnulib.replaceInFileOnce("lib/regcomp.c",
            "codeset_name = nl_langinfo (CODESET);",
            "char *nl_langinfo(int item);\ncodeset_name =  nl_langinfo (CODESET);");
    }

    auto &grep2 = grep.addTarget<ExecutableTarget>("grep");
    {
        auto &grep = grep2;
        grep.setChecks("grep");
        grep +=
            "src/.*"_rr;
        grep.Public +=
            "src"_id;
        grep.Private += "__USE_GNU"_d;
        grep.Public += gnulib;

        grep.writeFileOnce(grep.BinaryPrivateDir / "unistd.h");
        grep.writeFileOnce(grep.BinaryPrivateDir / "configmake.h");
        grep.writeFileOnce(grep.BinaryPrivateDir / "config.h",
"#define PACKAGE_NAME \"" + grep.Variables["PACKAGE_NAME"].toString() + "\"\n" +
"#define VERSION \"" + grep.Variables["PACKAGE_VERSION"].toString() + "\"\n" +
R"(
#define mode_t int
#define nlink_t short

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
    #include <io.h>

    #ifdef _WIN64
    #define ssize_t long long
    #else
    #define ssize_t int
    #endif

    #define __builtin_expect(cond, v) (cond)
    #define lstat stat


    #ifndef O_NDELAY
    # define O_NDELAY 0
    #endif

    #ifndef O_NOATIME
    # define O_NOATIME 0
    #endif

    #ifndef O_NONBLOCK
    # define O_NONBLOCK O_NDELAY
    #endif

    #ifndef O_NOCTTY
    # define O_NOCTTY 0
    #endif

    #ifndef O_NOFOLLOW
    # define O_NOFOLLOW 0
    #endif

    #ifndef O_CLOEXEC
    #define O_CLOEXEC 0
    #endif

    /* Use the same bit pattern as Solaris 9, but with the proper
        signedness.  The bit pattern is important, in case this actually is
        Solaris with the above workaround.  */
    #ifndef AT_FDCWD
    # define AT_FDCWD (-3041965)
    #endif

    /* Use the same values as Solaris 9.  This shouldn't matter, but
        there's no real reason to differ.  */
    #ifndef AT_SYMLINK_NOFOLLOW
    # define AT_SYMLINK_NOFOLLOW 4096
    #endif
#endif

void *alloca (size_t);
void * memrchr (const void *, int, size_t);
struct DIR *fdopendir(int fd);
char *getcwd(char *buf, size_t size);
void *mempcpy(void * __dest, void const * __src,
    size_t __n);

#define FLEXIBLE_ARRAY_MEMBER

/* The __pure__ attribute was added in gcc 2.96.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
# define _GL_ATTRIBUTE_PURE __attribute__ ((__pure__))
#else
# define _GL_ATTRIBUTE_PURE /* empty */
#endif

#define HAVE_WORKING_O_NOFOLLOW 1

#ifndef O_DIRECTORY
# define O_DIRECTORY        0200000
#endif

#ifndef O_SEARCH
# define O_SEARCH        0
#endif

#define GNULIB_MSVC_NOTHROW 1
#define HAVE_DECL_STRERROR_R 1


#define _GL_ATTRIBUTE_FORMAT_PRINTF(x,y)
#define _GL_ARG_NONNULL(x)

#define PROMOTED_MODE_T int

#define UNSIGNED 1

#define _GL_INLINE inline
#define _GL_EXTERN_INLINE inline

# define _GL_INLINE_HEADER_BEGIN
# define _GL_INLINE_HEADER_END


/* The _Noreturn keyword of C11.  */
#if ! (defined _Noreturn \
        || (defined __STDC_VERSION__ && 201112 <= __STDC_VERSION__))
# if (3 <= __GNUC__ || (__GNUC__ == 2 && 8 <= __GNUC_MINOR__) \
        || 0x5110 <= __SUNPRO_C)
#  define _Noreturn __attribute__ ((__noreturn__))
# elif defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn
# endif
#endif

#define _GL_UNUSED
#define __getopt_argv_const const
#define _GL_ATTRIBUTE_CONST

#define	F_OK		0	/* test for existence of file */
#define	X_OK		0x01	/* test for execute or search permission */
#define	W_OK		0x02	/* test for write permission */
#define	R_OK		0x04	/* test for read permission */

/* command values */
#define	F_DUPFD		0		/* duplicate file descriptor */
#define	F_GETFD		1		/* get file descriptor flags */
#define	F_SETFD		2		/* set file descriptor flags */
#define	F_GETFL		3		/* get file status flags */
#define	F_SETFL		4		/* set file status flags */
#ifndef _POSIX_SOURCE
#define	F_GETOWN	5		/* get SIGIO/SIGURG proc/pgrp */
#define F_SETOWN	6		/* set SIGIO/SIGURG proc/pgrp */
#endif
#define	F_GETLK		7		/* get record locking information */
#define	F_SETLK		8		/* set record locking information */
#define	F_SETLKW	9		/* F_SETLK; wait if blocked */

/* file descriptor flags (F_GETFD, F_SETFD) */
#define	FD_CLOEXEC	1		/* close-on-exec flag */

/* record locking flags (F_GETLK, F_SETLK, F_SETLKW) */
#define	F_RDLCK		1		/* shared or read lock */
#define	F_UNLCK		2		/* unlock */
#define	F_WRLCK		3		/* exclusive or write lock */
#ifdef KERNEL
#define	F_WAIT		0x010		/* Wait until lock is granted */
#define	F_FLOCK		0x020	 	/* Use flock(2) semantics for lock */
#define	F_POSIX		0x040	 	/* Use POSIX semantics for lock */
#endif

# define F_DUPFD_CLOEXEC 1030        /* Duplicate file descriptor with
                                    close-on-exit set.  */

#define strcasecmp stricmp
#define strncasecmp strnicmp

#ifndef S_TYPEISSEM
# ifdef S_INSEM
#  define S_TYPEISSEM(p) (S_ISNAM ((p)->st_mode) && (p)->st_rdev == S_INSEM)
# else
#  define S_TYPEISSEM(p) 0
# endif
#endif

#ifndef S_TYPEISSHM
# ifdef S_INSHD
#  define S_TYPEISSHM(p) (S_ISNAM ((p)->st_mode) && (p)->st_rdev == S_INSHD)
# else
#  define S_TYPEISSHM(p) 0
# endif
#endif

#ifndef S_TYPEISMQ
# define S_TYPEISMQ(p) 0
#endif

#ifndef S_TYPEISTMO
# define S_TYPEISTMO(p) 0
#endif

#ifdef DIR_FD_MEMBER_NAME
# define DIR_TO_FD(Dir_p) ((Dir_p)->DIR_FD_MEMBER_NAME)
#else
# define DIR_TO_FD(Dir_p) -1
#endif

#define _GL_EXTERN_C extern

#define _GL_FUNCDECL_SYS(func,rettype,parameters_and_attributes) \
    _GL_EXTERN_C rettype func parameters_and_attributes

# define _GL_CXXALIAS_SYS(func,rettype,parameters) \
    _GL_EXTERN_C int _gl_cxxalias_dummy
)");
    }
}

void check(Checker &c)
{
    {
        auto &s = c.addSet("gnulib");
        s.checkFunctionExists("lstat");
        s.checkFunctionExists("strerror_r");
        s.checkTypeSize("long long int");
        s.checkTypeSize("size_t");
        s.checkTypeSize("unsigned long long int");
        s.checkTypeSize("void *");
    }
    {
        auto &s = c.addSet("grep");
        s.checkFunctionExists("lstat");
        s.checkFunctionExists("strerror_r");
        s.checkIncludeExists("inttypes.h");
        s.checkIncludeExists("memory.h");
        s.checkIncludeExists("stddef.h");
        s.checkIncludeExists("stdint.h");
        s.checkIncludeExists("stdlib.h");
        s.checkIncludeExists("strings.h");
        s.checkIncludeExists("string.h");
        s.checkIncludeExists("sys/stat.h");
        s.checkIncludeExists("sys/types.h");
        s.checkIncludeExists("unistd.h");
        s.checkTypeSize("long long int");
        s.checkTypeSize("size_t");
        s.checkTypeSize("unsigned long long int");
        s.checkTypeSize("void *");
        s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
    }
}