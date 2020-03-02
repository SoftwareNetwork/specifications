struct M4Executable : ExecutableTarget
{
    void setupCommand(builder::Command &c) const override
    {
        c.environment["M4PATH"] = (SourceDir / "m4").u8string();

        ExecutableTarget::setupCommand(c);
    }
};

void build(Solution &s)
{
    auto &m4 = s.addProject("gnu.m4", "1.4.18");
    m4 += RemoteFile("https://ftp.gnu.org/gnu/m4/m4-{v}.tar.xz");

    auto &gnulib = m4.addTarget<StaticLibraryTarget>("gnulib");
    {
        gnulib.setChecks("gnulib");

        gnulib +=
            "lib/.*"_rr;

        gnulib -=
            "lib/localcharset.c",
            "lib/localeconv.c",
            "lib/lseek.c",
            "lib/mbsinit.c",
            "lib/memchr.c",
            "lib/regcomp.c",
            "lib/regex_internal.c",
            "lib/regexec.c",
            "lib/strerror-override.c",

            "lib/ftell.c",
            "lib/ftello.c",
            "lib/fseek.c",
            "lib/snprintf.c",
            "lib/frexp.c",
            "lib/frexpl.c",
            "lib/btowc.c",
            "lib/fflush.c"
            ;

        gnulib -= "lib/spawn.*"_rr;
        gnulib += "lib/spawn-pipe.c";
        gnulib += "lib/clean-temp.c";

        gnulib.Public += "lib"_id;

        gnulib.Private += "__USE_GNU"_d;
        gnulib.Private += "OK_TO_USE_1S_CLOCK"_def;

        gnulib.Public += "org.sw.demo.gnu.gawk.getopt"_dep;
        gnulib.Public -= "org.sw.demo.tronkko.dirent-master"_dep;

        if (gnulib.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            gnulib.Public += "org.sw.demo.tronkko.dirent-master"_dep;
            //gnulib.Public += "HAVE_FCNTL"_def;
        }

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

        if (gnulib.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            //gnulib += "__inline=inline"_def;
            if (gnulib.getBuildSettings().TargetOS.is(ArchType::x86_64))
            {
                gnulib += "WINDOWS_64_BIT_ST_SIZE=1"_def;
                gnulib += "WINDOWS_64_BIT_OFF_T=1"_def;
            }

            gnulib += "GNULIB_CLOSE_STREAM=1"_def;

            gnulib.Variables["NEXT_DIRENT_H"] = "\"dirent-private.h\"";
            gnulib.writeFileOnce("sys/time.h");
        }

        //#set(HAVE_GETOPT_H 1)

        gnulib.writeFileOnce(gnulib.BinaryPrivateDir / "alloca.h");

        //gnulib.Variables["HAVE_FNMATCH"] = 0;
        //gnulib.Variables["HAVE_FNMATCH_H"] = 0;
        gnulib.Variables["REPLACE_POSIX_SPAWN"] = 0;
        gnulib.Variables["GNULIB_POSIX_SPAWN"] = 1;
        gnulib.Variables["GNULIB_POSIX_SPAWN_GNU"] = 1;

        gnulib.Variables["HAVE_STRUCT_SCHED_PARAM"] = 0;

        gnulib.Variables["GNULIB_WAITPID"] = "0";

        gnulib.Variables["GNULIB_PTHREAD_SIGMASK"] = "1";
        gnulib.Variables["REPLACE_PTHREAD_SIGMASK"] = "1";
        gnulib.Variables["HAVE_TYPE_VOLATILE_SIG_ATOMIC_T"] = "0";
        gnulib.Variables["HAVE_SIGSET_T"] = "0";
        gnulib.Variables["HAVE_SIGHANDLER_T"] = "0";
        gnulib.Variables["GNULIB_SIGNAL_H_SIGPIPE"] = "1";
        gnulib.Variables["GNULIB_SIGACTION"] = "1";
        gnulib.Variables["HAVE_SIGINFO_T"] = "0";
        gnulib.Variables["GNULIB_defined_siginfo_types"] = "0";
        gnulib.Variables["GNULIB_defined_struct_sigaction"] = "0";
        gnulib.Variables["GNULIB_RAISE"] = "0";
        gnulib.Variables["GNULIB_SIGPROCMASK"] = "1";
        gnulib.Variables["HAVE_POSIX_SIGNALBLOCKING"] = "0";

        //gnulib.configureFile("lib/getopt.in.h", "getopt.h");
        gnulib.configureFile("lib/spawn.in.h", "spawn.h");
        gnulib.configureFile("lib/sched.in.h", "sched.h");
        gnulib.configureFile("lib/sys_wait.in.h", "sys/wait.h");
        //gnulib.configureFile("lib/sys_stat.in.h", "sys/stat.h");
        //gnulib.configureFile("lib/sys_types.in.h", "sys/types.h");
        //gnulib.configureFile("lib/signal.in.h", "signal.h");
        //gnulib.configureFile("lib/uniwidth.in.h", "uniwidth.h");
        //gnulib.configureFile("lib/unitypes.in.h", "unitypes.h");
        //gnulib.configureFile("lib/wchar.in.h", "wchar.h");

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
    #define restrict

    #include <io.h>
    #include <sys/types.h>
    # include <winsock2.h>

    #define pid_t int
    //#define off_t long
    #define uid_t int

    #ifdef _WIN64
    #define ssize_t long long
    #define WINDOWS_64_BIT_ST_SIZE 1
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

    #ifndef O_EXEC
    # define O_EXEC O_RDONLY /* This is often close enough in older systems.  */
    #endif

    #ifndef O_NOCTTY
    # define O_NOCTTY 0
    #endif

    #ifndef O_NOFOLLOW
    # define O_NOFOLLOW 0
    #endif

    #ifndef O_NOLINK
    # define O_NOLINK 0
    #endif

    #ifndef O_NOLINKS
    # define O_NOLINKS 0
    #endif

    #ifndef O_NOTRANS
    # define O_NOTRANS 0
    #endif

    #ifndef O_RSYNC
    # define O_RSYNC 0
    #endif

    #ifndef O_SEARCH
    # define O_SEARCH O_RDONLY /* This is often close enough in older systems.  */
    #endif

    #ifndef O_SYNC
    # define O_SYNC 0
    #endif

    #ifndef O_TTY_INIT
    # define O_TTY_INIT 0
    #endif

    //#if ~O_ACCMODE & (O_RDONLY | O_WRONLY | O_RDWR | O_EXEC | O_SEARCH)
    //# undef O_ACCMODE
    # define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR | O_EXEC | O_SEARCH)
    //#endif


#ifndef S_IFIFO
# ifdef _S_IFIFO
#  define S_IFIFO _S_IFIFO
# endif
#endif

#ifndef S_IFMT
# define S_IFMT 0170000
#endif

#if STAT_MACROS_BROKEN
# undef S_ISBLK
# undef S_ISCHR
# undef S_ISDIR
# undef S_ISFIFO
# undef S_ISLNK
# undef S_ISNAM
# undef S_ISMPB
# undef S_ISMPC
# undef S_ISNWK
# undef S_ISREG
# undef S_ISSOCK
#endif

#ifndef S_ISBLK
# ifdef S_IFBLK
#  define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
# else
#  define S_ISBLK(m) 0
# endif
#endif

#ifndef S_ISCHR
# ifdef S_IFCHR
#  define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
# else
#  define S_ISCHR(m) 0
# endif
#endif

#ifndef S_ISDIR
# ifdef S_IFDIR
#  define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
# else
#  define S_ISDIR(m) 0
# endif
#endif

#ifndef S_ISDOOR /* Solaris 2.5 and up */
# define S_ISDOOR(m) 0
#endif

#ifndef S_ISFIFO
# ifdef S_IFIFO
#  define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
# else
#  define S_ISFIFO(m) 0
# endif
#endif

#ifndef S_ISLNK
# ifdef S_IFLNK
#  define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
# else
#  define S_ISLNK(m) 0
# endif
#endif

#ifndef S_ISMPB /* V7 */
# ifdef S_IFMPB
#  define S_ISMPB(m) (((m) & S_IFMT) == S_IFMPB)
#  define S_ISMPC(m) (((m) & S_IFMT) == S_IFMPC)
# else
#  define S_ISMPB(m) 0
#  define S_ISMPC(m) 0
# endif
#endif

#ifndef S_ISMPX /* AIX */
# define S_ISMPX(m) 0
#endif

#ifndef S_ISNAM /* Xenix */
# ifdef S_IFNAM
#  define S_ISNAM(m) (((m) & S_IFMT) == S_IFNAM)
# else
#  define S_ISNAM(m) 0
# endif
#endif

#ifndef S_ISNWK /* HP/UX */
# ifdef S_IFNWK
#  define S_ISNWK(m) (((m) & S_IFMT) == S_IFNWK)
# else
#  define S_ISNWK(m) 0
# endif
#endif

#ifndef S_ISPORT /* Solaris 10 and up */
# define S_ISPORT(m) 0
#endif

#ifndef S_ISREG
# ifdef S_IFREG
#  define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
# else
#  define S_ISREG(m) 0
# endif
#endif

#ifndef S_ISSOCK
# ifdef S_IFSOCK
#  define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
# else
#  define S_ISSOCK(m) 0
# endif
#endif


#ifndef S_TYPEISMQ
# define S_TYPEISMQ(p) 0
#endif

#ifndef S_TYPEISTMO
# define S_TYPEISTMO(p) 0
#endif


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

/* high performance ("contiguous data") */
#ifndef S_ISCTG
# define S_ISCTG(p) 0
#endif

/* Cray DMF (data migration facility): off line, with data  */
#ifndef S_ISOFD
# define S_ISOFD(p) 0
#endif

/* Cray DMF (data migration facility): off line, with no data  */
#ifndef S_ISOFL
# define S_ISOFL(p) 0
#endif

/* 4.4BSD whiteout */
#ifndef S_ISWHT
# define S_ISWHT(m) 0
#endif

/* If any of the following are undefined,
   define them to their de facto standard values.  */
#if !S_ISUID
# define S_ISUID 04000
#endif
#if !S_ISGID
# define S_ISGID 02000
#endif

/* S_ISVTX is a common extension to POSIX.  */
#ifndef S_ISVTX
# define S_ISVTX 01000
#endif

#if !S_IRUSR && S_IREAD
# define S_IRUSR S_IREAD
#endif
#if !S_IRUSR
# define S_IRUSR 00400
#endif
#if !S_IRGRP
# define S_IRGRP (S_IRUSR >> 3)
#endif
#if !S_IROTH
# define S_IROTH (S_IRUSR >> 6)
#endif

#if !S_IWUSR && S_IWRITE
# define S_IWUSR S_IWRITE
#endif
#if !S_IWUSR
# define S_IWUSR 00200
#endif
#if !S_IWGRP
# define S_IWGRP (S_IWUSR >> 3)
#endif
#if !S_IWOTH
# define S_IWOTH (S_IWUSR >> 6)
#endif

#if !S_IXUSR && S_IEXEC
# define S_IXUSR S_IEXEC
#endif
#if !S_IXUSR
# define S_IXUSR 00100
#endif
#if !S_IXGRP
# define S_IXGRP (S_IXUSR >> 3)
#endif
#if !S_IXOTH
# define S_IXOTH (S_IXUSR >> 6)
#endif

#if !S_IRWXU
# define S_IRWXU (S_IRUSR | S_IWUSR | S_IXUSR)
#endif
#if !S_IRWXG
# define S_IRWXG (S_IRGRP | S_IWGRP | S_IXGRP)
#endif
#if !S_IRWXO
# define S_IRWXO (S_IROTH | S_IWOTH | S_IXOTH)
#endif

/* S_IXUGO is a common extension to POSIX.  */
#if !S_IXUGO
# define S_IXUGO (S_IXUSR | S_IXGRP | S_IXOTH)
#endif

#ifndef S_IRWXUGO
# define S_IRWXUGO (S_IRWXU | S_IRWXG | S_IRWXO)
#endif

/* Macros for futimens and utimensat.  */
#ifndef UTIME_NOW
# define UTIME_NOW (-1)
# define UTIME_OMIT (-2)
#endif





    union sigval
    {
      int sival_int;
      void *sival_ptr;
    };


    struct siginfo_t
    {
      int si_signo;
      int si_code;
      int si_errno;
      pid_t si_pid;
      uid_t si_uid;
      void *si_addr;
      int si_status;
      long si_band;
      union sigval si_value;
    };
    typedef struct siginfo_t siginfo_t;


    //#define signal rpl_signal



#endif

void *alloca (size_t);
void * memrchr (const void *, int, size_t);
struct DIR* fdopendir(int fd);
//char *getcwd(char *buf, size_t size);
void *mempcpy(void * __dest, void const * __src,
    size_t __n);

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

#define _GL_FUNCDECL_RPL _GL_FUNCDECL_SYS
#define _GL_CXXALIAS_RPL _GL_CXXALIAS_SYS

#define HAVE_DECL_STRTOULL 1




#define sigset_t int

struct sigaction
{
    union
    {
        void(*_sa_handler) (int);
        /* Present to allow compilation, but unsupported by gnulib.  POSIX
           says that implementations may, but not must, make sa_sigaction
           overlap with sa_handler, but we know of no implementation where
           they do not overlap.  */
        void(*_sa_sigaction) (int, siginfo_t *, void *);
    } _sa_func;
    sigset_t sa_mask;
    /* Not all POSIX flags are supported.  */
    int sa_flags;
};
#   define sa_handler _sa_func._sa_handler
#   define sa_sigaction _sa_func._sa_sigaction
/* Unsupported flags are not present.  */
#   define SA_RESETHAND 1
#   define SA_NODEFER 2
#   define SA_RESTART 4

#   define GNULIB_defined_struct_sigaction 1

_GL_FUNCDECL_SYS(sigaction, int, (int, const struct sigaction *restrict,
    struct sigaction *restrict));

#  define SIG_BLOCK   0  /* blocked_set = blocked_set | *set; */
#  define SIG_SETMASK 1  /* blocked_set = *set; */
#  define SIG_UNBLOCK 2  /* blocked_set = blocked_set & ~*set; */



char * strsignal(int);


        )");
    }

    auto &m4_2 = m4.addTarget<M4Executable>("m4");
    {
        auto &m4 = m4_2;
        m4.PackageDefinitions = true;
        m4.setChecks("m4");
        m4 +=
            "src/.*"_rr;
        m4 +=
            "m4/.*"_rr;
        m4.Public +=
            "src"_id;
        m4.Private += "__USE_GNU"_d;
        m4.Private += "ENABLE_CHANGEWORD"_d;
        m4.Public += gnulib;

        if (m4.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            m4 += "RENAME_OPEN_FILE_WORKS=0"_def;
            m4 += "SYSCMD_SHELL=\"cmd\""_def;

            m4.patch("src/m4.c", "sigaction (", "// sigaction(");

            m4.patch("src/path.c", "path_end = strchr (path, ':');", R"(
#ifdef _WIN32
      path_end = strchr(path, ';');
#else
      path_end = strchr(path, ':');
#endif
)");
        }
        else
        {
            m4 += "RENAME_OPEN_FILE_WORKS=1"_def;
            m4 += "SYSCMD_SHELL=\"\""_def;
        }

        if (auto L = m4.Linker->as<VisualStudioLinker*>(); L)
            L->Force = vs::ForceType::Multiple;

        m4.writeFileOnce(m4.BinaryPrivateDir / "unistd.h");
        m4.writeFileOnce(m4.BinaryPrivateDir / "configmake.h");
        m4.writeFileOnce(m4.BinaryPrivateDir / "config.h",
            "#define PACKAGE_NAME \"" + m4.Variables["PACKAGE_NAME"].toString() + "\"\n" +
            "#define VERSION \"" + m4.Variables["PACKAGE_VERSION"].toString() + "\"\n" +
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
    #define restrict

    #include <io.h>

    #define pid_t int
    #define uid_t int

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


    #ifndef O_EXEC
    # define O_EXEC O_RDONLY /* This is often close enough in older systems.  */
    #endif

    #ifndef O_NOCTTY
    # define O_NOCTTY 0
    #endif

    #ifndef O_NOFOLLOW
    # define O_NOFOLLOW 0
    #endif

    #ifndef O_NOLINK
    # define O_NOLINK 0
    #endif

    #ifndef O_NOLINKS
    # define O_NOLINKS 0
    #endif

    #ifndef O_NOTRANS
    # define O_NOTRANS 0
    #endif

    #ifndef O_RSYNC
    # define O_RSYNC 0
    #endif

    #ifndef O_SEARCH
    # define O_SEARCH O_RDONLY /* This is often close enough in older systems.  */
    #endif

    #ifndef O_SYNC
    # define O_SYNC 0
    #endif

    #ifndef O_TTY_INIT
    # define O_TTY_INIT 0
    #endif

    //#if ~O_ACCMODE & (O_RDONLY | O_WRONLY | O_RDWR | O_EXEC | O_SEARCH)
    //# undef O_ACCMODE
    # define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR | O_EXEC | O_SEARCH)
    //#endif


#ifndef S_IFIFO
# ifdef _S_IFIFO
#  define S_IFIFO _S_IFIFO
# endif
#endif

#ifndef S_IFMT
# define S_IFMT 0170000
#endif

#if STAT_MACROS_BROKEN
# undef S_ISBLK
# undef S_ISCHR
# undef S_ISDIR
# undef S_ISFIFO
# undef S_ISLNK
# undef S_ISNAM
# undef S_ISMPB
# undef S_ISMPC
# undef S_ISNWK
# undef S_ISREG
# undef S_ISSOCK
#endif

#ifndef S_ISBLK
# ifdef S_IFBLK
#  define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
# else
#  define S_ISBLK(m) 0
# endif
#endif

#ifndef S_ISCHR
# ifdef S_IFCHR
#  define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
# else
#  define S_ISCHR(m) 0
# endif
#endif

#ifndef S_ISDIR
# ifdef S_IFDIR
#  define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
# else
#  define S_ISDIR(m) 0
# endif
#endif

#ifndef S_ISDOOR /* Solaris 2.5 and up */
# define S_ISDOOR(m) 0
#endif

#ifndef S_ISFIFO
# ifdef S_IFIFO
#  define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
# else
#  define S_ISFIFO(m) 0
# endif
#endif

#ifndef S_ISLNK
# ifdef S_IFLNK
#  define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
# else
#  define S_ISLNK(m) 0
# endif
#endif

#ifndef S_ISMPB /* V7 */
# ifdef S_IFMPB
#  define S_ISMPB(m) (((m) & S_IFMT) == S_IFMPB)
#  define S_ISMPC(m) (((m) & S_IFMT) == S_IFMPC)
# else
#  define S_ISMPB(m) 0
#  define S_ISMPC(m) 0
# endif
#endif

#ifndef S_ISMPX /* AIX */
# define S_ISMPX(m) 0
#endif

#ifndef S_ISNAM /* Xenix */
# ifdef S_IFNAM
#  define S_ISNAM(m) (((m) & S_IFMT) == S_IFNAM)
# else
#  define S_ISNAM(m) 0
# endif
#endif

#ifndef S_ISNWK /* HP/UX */
# ifdef S_IFNWK
#  define S_ISNWK(m) (((m) & S_IFMT) == S_IFNWK)
# else
#  define S_ISNWK(m) 0
# endif
#endif

#ifndef S_ISPORT /* Solaris 10 and up */
# define S_ISPORT(m) 0
#endif

#ifndef S_ISREG
# ifdef S_IFREG
#  define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
# else
#  define S_ISREG(m) 0
# endif
#endif

#ifndef S_ISSOCK
# ifdef S_IFSOCK
#  define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
# else
#  define S_ISSOCK(m) 0
# endif
#endif


#ifndef S_TYPEISMQ
# define S_TYPEISMQ(p) 0
#endif

#ifndef S_TYPEISTMO
# define S_TYPEISTMO(p) 0
#endif


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

/* high performance ("contiguous data") */
#ifndef S_ISCTG
# define S_ISCTG(p) 0
#endif

/* Cray DMF (data migration facility): off line, with data  */
#ifndef S_ISOFD
# define S_ISOFD(p) 0
#endif

/* Cray DMF (data migration facility): off line, with no data  */
#ifndef S_ISOFL
# define S_ISOFL(p) 0
#endif

/* 4.4BSD whiteout */
#ifndef S_ISWHT
# define S_ISWHT(m) 0
#endif

/* If any of the following are undefined,
   define them to their de facto standard values.  */
#if !S_ISUID
# define S_ISUID 04000
#endif
#if !S_ISGID
# define S_ISGID 02000
#endif

/* S_ISVTX is a common extension to POSIX.  */
#ifndef S_ISVTX
# define S_ISVTX 01000
#endif

#if !S_IRUSR && S_IREAD
# define S_IRUSR S_IREAD
#endif
#if !S_IRUSR
# define S_IRUSR 00400
#endif
#if !S_IRGRP
# define S_IRGRP (S_IRUSR >> 3)
#endif
#if !S_IROTH
# define S_IROTH (S_IRUSR >> 6)
#endif

#if !S_IWUSR && S_IWRITE
# define S_IWUSR S_IWRITE
#endif
#if !S_IWUSR
# define S_IWUSR 00200
#endif
#if !S_IWGRP
# define S_IWGRP (S_IWUSR >> 3)
#endif
#if !S_IWOTH
# define S_IWOTH (S_IWUSR >> 6)
#endif

#if !S_IXUSR && S_IEXEC
# define S_IXUSR S_IEXEC
#endif
#if !S_IXUSR
# define S_IXUSR 00100
#endif
#if !S_IXGRP
# define S_IXGRP (S_IXUSR >> 3)
#endif
#if !S_IXOTH
# define S_IXOTH (S_IXUSR >> 6)
#endif

#if !S_IRWXU
# define S_IRWXU (S_IRUSR | S_IWUSR | S_IXUSR)
#endif
#if !S_IRWXG
# define S_IRWXG (S_IRGRP | S_IWGRP | S_IXGRP)
#endif
#if !S_IRWXO
# define S_IRWXO (S_IROTH | S_IWOTH | S_IXOTH)
#endif

/* S_IXUGO is a common extension to POSIX.  */
#if !S_IXUGO
# define S_IXUGO (S_IXUSR | S_IXGRP | S_IXOTH)
#endif

#ifndef S_IRWXUGO
# define S_IRWXUGO (S_IRWXU | S_IRWXG | S_IRWXO)
#endif

/* Macros for futimens and utimensat.  */
#ifndef UTIME_NOW
# define UTIME_NOW (-1)
# define UTIME_OMIT (-2)
#endif


    union sigval
    {
      int sival_int;
      void *sival_ptr;
    };


    struct siginfo_t
    {
      int si_signo;
      int si_code;
      int si_errno;
      pid_t si_pid;
      uid_t si_uid;
      void *si_addr;
      int si_status;
      long si_band;
      union sigval si_value;
    };
    typedef struct siginfo_t siginfo_t;

    //#define signal rpl_signal

#endif

void *alloca (size_t);
void * memrchr (const void *, int, size_t);
struct DIR *fdopendir(int fd);
//char *getcwd(char *buf, size_t size);
void *mempcpy(void * __dest, void const * __src,
    size_t __n);

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

#define _GL_FUNCDECL_RPL _GL_FUNCDECL_SYS
#define _GL_CXXALIAS_RPL _GL_CXXALIAS_SYS




#define sigset_t int

struct sigaction
{
    union
    {
        void(*_sa_handler) (int);
        /* Present to allow compilation, but unsupported by gnulib.  POSIX
           says that implementations may, but not must, make sa_sigaction
           overlap with sa_handler, but we know of no implementation where
           they do not overlap.  */
        void(*_sa_sigaction) (int, siginfo_t *, void *);
    } _sa_func;
    sigset_t sa_mask;
    /* Not all POSIX flags are supported.  */
    int sa_flags;
};
#   define sa_handler _sa_func._sa_handler
#   define sa_sigaction _sa_func._sa_sigaction
/* Unsupported flags are not present.  */
#   define SA_RESETHAND 1
#   define SA_NODEFER 2
#   define SA_RESTART 4

#   define GNULIB_defined_struct_sigaction 1

_GL_FUNCDECL_SYS(sigaction, int, (int, const struct sigaction *restrict,
    struct sigaction *restrict));

#  define SIG_BLOCK   0  /* blocked_set = blocked_set | *set; */
#  define SIG_SETMASK 1  /* blocked_set = *set; */
#  define SIG_UNBLOCK 2  /* blocked_set = blocked_set & ~*set; */


char * strsignal(int);

)");
    }
}

void check(Checker &c)
{
    {
        auto &s = c.addSet("gnulib");
        s.checkFunctionExists("lstat");
        s.checkFunctionExists("mbsinit");
        s.checkFunctionExists("strerror_r");
        s.checkFunctionExists("_ftime");
        s.checkFunctionExists("gettimeofday");
        s.checkFunctionExists("sigaction");
        s.checkFunctionExists("pthread_sigmask");
        s.checkTypeSize("long long int");
        s.checkTypeSize("size_t");
        s.checkTypeSize("unsigned long long int");
        s.checkTypeSize("void *");
        s.checkTypeSize("posix_spawnattr_t");
        s.checkIncludeExists("wchar.h");
        s.checkIncludeExists("sys/timeb.h");
        s.checkIncludeExists("features.h");
        s.checkIncludeExists("spawn.h");
        s.checkIncludeExists("sched.h");
    }
    {
        auto &s = c.addSet("m4");
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
        s.checkIncludeExists("wchar.h");
        s.checkIncludeExists("features.h");
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
