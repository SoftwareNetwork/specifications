struct SedExecutable : ExecutableTarget
{
    using ExecutableTarget::ExecutableTarget;

    void setupCommand(builder::Command &c) const override
    {
        if (getBuildSettings().TargetOS.Type != OSType::Windows)
        {
            c.setProgram("sed");
            return;
        }
        ExecutableTarget::setupCommand(c);
    }
};

void build(Solution &s)
{
    auto &sed = s.addProject("gnu.sed", "4.7.0");
    sed += RemoteFile("https://ftp.gnu.org/gnu/sed/sed-{M}.{m}.tar.xz");

    auto &gnulib = sed.addTarget<StaticLibraryTarget>("gnulib");
    {
        gnulib +=
            "lib/.*\\.h"_rr,
            "lib/acl-internal.c",
            "lib/copy-acl.c",
            "lib/fwriting.c",
            "lib/get-permissions.c",
            "lib/getdelim.c",
            "lib/gettimeofday.c",
            "lib/mkostemp.c",
            "lib/progname.c",
            "lib/qcopy-acl.c",
            "lib/se-context.c",
            "lib/se-selinux.c",
            "lib/set-permissions.c",
            "lib/strverscmp.c",
            "lib/tempname.c";

        gnulib.Public +=
            "lib"_id;

        gnulib.Private += "__USE_GNU"_d;

        gnulib.Public += "org.sw.demo.gnu.grep.gnulib-*"_dep;

        gnulib.writeFileOnce(gnulib.BinaryPrivateDir / "configmake.h");
        gnulib.writeFileOnce(gnulib.BinaryPrivateDir / "config.h",
            "#define PACKAGE_NAME \"" + gnulib.Variables["PACKAGE_NAME"].toString() + "\"\n" +
            "#define VERSION \"" + gnulib.Variables["PACKAGE_VERSION"].toString() + "\"\n" +
            R"(
#define mode_t int
#define nlink_t short

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#include <stdint.h>

#ifdef _WIN32
    #ifdef _WIN64
    #define ssize_t long long
    #else
    #define ssize_t int
    #endif

    #define __builtin_expect(cond, v) (cond)
    #define lstat stat

    #define restrict


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

    #if !defined S_ISDIR && defined S_IFDIR
    # define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
    #endif
    #if !S_IRUSR && S_IREAD
    # define S_IRUSR S_IREAD
    #endif
    #if !S_IRUSR
    # define S_IRUSR 00400
    #endif
    #if !S_IWUSR && S_IWRITE
    # define S_IWUSR S_IWRITE
    #endif
    #if !S_IWUSR
    # define S_IWUSR 00200
    #endif
    #if !S_IXUSR && S_IEXEC
    # define S_IXUSR S_IEXEC
    #endif
    #if !S_IXUSR
    # define S_IXUSR 00100
    #endif

    #define _S_IRWXU  S_IRUSR | S_IWUSR | S_IXUSR

    #define S_IFREG    0100000

    #ifndef S_ISREG
    # ifdef S_IFREG
    #  define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
    # else
    #  define S_ISREG(m) 0
    # endif
    #endif

    #define O_ACCMODE 3



    #define _CRT_DECLARE_NONSTDC_NAMES 1

#endif

void * memrchr (const void *, int, size_t);

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define FLEXIBLE_ARRAY_MEMBER

/* The __pure__ attribute was added in gcc 2.96.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
# define _GL_ATTRIBUTE_PURE __attribute__ ((__pure__))
#else
# define _GL_ATTRIBUTE_PURE /* empty */
#endif

#define _GL_ATTRIBUTE_MALLOC

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

#ifndef _GL_UNUSED_PARAMETER
# if __GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#  define _GL_UNUSED_PARAMETER __attribute__ ((__unused__))
# else
#  define _GL_UNUSED_PARAMETER
# endif
#endif
)"
);

        gnulib.writeFileOnce(gnulib.BinaryPrivateDir / "sys/time.h", "#include <time.h>\n#include <windows.h>");

        if (gnulib.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            gnulib.writeFileOnce(gnulib.BinaryDir / "alloca.h");
            gnulib.writeFileOnce(gnulib.BinaryDir / "unistd.h");

            gnulib.configureFile("lib/se-selinux.in.h", "selinux/selinux.h");
            gnulib.configureFile("lib/se-context.in.h", "selinux/context.h");
        }
    }

    auto &sed2 = sed.addTarget<SedExecutable>("sed");
    if (!sed2.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        return;
    }

    {
        auto &sed = sed2;
        sed +=
            "basicdefs.h",
            "sed/.*"_rr;

        sed += "."_idir;

        sed.Private += "HAVE_ISASCII"_d;
        sed.Private += "STDC_HEADERS"_d;
        sed.Private += "__USE_GNU"_d;

        sed.Public += gnulib;
        sed.Public += "org.sw.demo.gnu.gettext.intl-0"_dep;
        sed.Public -= "org.sw.demo.kimgr.getopt_port-master"_dep;

        if (sed.getBuildSettings().TargetOS.Type == OSType::Windows)
            sed.Public += "org.sw.demo.kimgr.getopt_port-master"_dep;

#ifdef SW_CPP_DRIVER_API_VERSION
#if SW_CPP_DRIVER_API_VERSION == 2
        if (auto L = sed.getLinker().as<VisualStudioLinker*>(); L)
#else
        if (auto L = sed.Linker->as<VisualStudioLinker*>(); L)
#endif
#else
        if (auto L = sed.Linker->as<VisualStudioLinker>(); L)
#endif
            L->Force = vs::ForceType::Multiple;

        sed.writeFileOnce(sed.BinaryPrivateDir / "configmake.h");
        sed.writeFileOnce(sed.BinaryPrivateDir / "config.h",
            "#define PACKAGE_NAME \"" + sed.Variables["PACKAGE_NAME"].toString() + "\"\n" +
            "#define PACKAGE_VERSION \"" + sed.Variables["PACKAGE_VERSION"].toString() + "\"\n" +
            "#define PACKAGE_URL \"" + gnulib.Variables["PACKAGE_URL"].toString() + "\"\n" +
            "#define PACKAGE_BUGREPORT \"" + gnulib.Variables["PACKAGE_BUGREPORT"].toString() + "\"\n" +
            R"(
#define mode_t int
#define nlink_t short

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#include <stdint.h>

#ifdef _WIN32
    #ifdef _WIN64
    #define ssize_t long long
    #else
    #define ssize_t int
    #endif

    #define __builtin_expect(cond, v) (cond)
    #define lstat stat

    #define restrict


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

    #if !defined S_ISDIR && defined S_IFDIR
    # define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
    #endif
    #if !S_IRUSR && S_IREAD
    # define S_IRUSR S_IREAD
    #endif
    #if !S_IRUSR
    # define S_IRUSR 00400
    #endif
    #if !S_IWUSR && S_IWRITE
    # define S_IWUSR S_IWRITE
    #endif
    #if !S_IWUSR
    # define S_IWUSR 00200
    #endif
    #if !S_IXUSR && S_IEXEC
    # define S_IXUSR S_IEXEC
    #endif
    #if !S_IXUSR
    # define S_IXUSR 00100
    #endif

    #define _S_IRWXU  S_IRUSR | S_IWUSR | S_IXUSR

    #define S_IFREG    0100000

    #ifndef S_ISREG
    # ifdef S_IFREG
    #  define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
    # else
    #  define S_ISREG(m) 0
    # endif
    #endif

    #define O_ACCMODE 3



    #define _CRT_DECLARE_NONSTDC_NAMES 1

#endif

void * memrchr (const void *, int, size_t);

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define FLEXIBLE_ARRAY_MEMBER

/* The __pure__ attribute was added in gcc 2.96.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
# define _GL_ATTRIBUTE_PURE __attribute__ ((__pure__))
#else
# define _GL_ATTRIBUTE_PURE /* empty */
#endif

#define _GL_ATTRIBUTE_MALLOC

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

#ifndef _GL_UNUSED_PARAMETER
# if __GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#  define _GL_UNUSED_PARAMETER __attribute__ ((__unused__))
# else
#  define _GL_UNUSED_PARAMETER
# endif
#endif
)"
);

        sed.writeFileOnce(sed.BinaryPrivateDir / "version.h", "extern char const *Version;");
        sed.writeFileOnce(sed.BinaryPrivateDir / "version.c", "char const *Version = \"" + sed.Variables["PACKAGE_VERSION"].toString() + "\";");
        sed += sed.BinaryPrivateDir / "version.c";


        sed.replaceInFileOnce("sed/sed.c", "static void usage (int);", "");
        sed.replaceInFileOnce("sed/sed.h", "struct re_registers", "struct  re_registers1");

        sed.replaceInFileOnce("sed/utils.c",
            "int rd = rename (from, to);",
            "#ifdef _WIN32\nif(unlink(to) == -1) panic (_(\"cannot rename %s: %s\"), from, strerror (errno));\n#endif\nint rd = rename  (from, to);"
        );

        sed.replaceInFileOnce("sed/compile.c",
            "while ((ch=inchar()) == ';' || ISSPACE(ch))",
            "while (((ch=inchar()) == ';' || ISSPACE(ch)) && ch != EOF)"
        );
    }
}
