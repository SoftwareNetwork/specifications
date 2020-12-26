void build(Solution &s)
{
    auto &libarchive = s.addTarget<LibraryTarget>("libarchive.libarchive", "3.5.1");
    libarchive += Git("https://github.com/libarchive/libarchive", "v{v}");

    libarchive.setChecks("libarchive");
    libarchive +=
        "build/cmake/CheckFuncs.cmake",
        "build/cmake/CheckFuncs_stub.c.in",
        "build/cmake/CheckTypeExists.cmake",
        "build/cmake/config.h.in",
        "libarchive/[^/]*\\.c"_rr,
        "libarchive/[^/]*\\.h"_rr;

    libarchive.Public +=
        "libarchive"_id;

    libarchive.Public += "ARCHIVE_CRYPTO_MD5_NETTLE"_d;
    libarchive.Public += "ARCHIVE_CRYPTO_RMD160_NETTLE"_d;
    libarchive.Public += "ARCHIVE_CRYPTO_SHA1_NETTLE"_d;
    libarchive.Public += "ARCHIVE_CRYPTO_SHA256_NETTLE"_d;
    libarchive.Public += "ARCHIVE_CRYPTO_SHA384_NETTLE"_d;
    libarchive.Public += "ARCHIVE_CRYPTO_SHA512_NETTLE"_d;
    libarchive.Public += "HAVE_BZLIB_H"_d;
    libarchive.Public += "HAVE_CONFIG_H"_d;
    libarchive.Public += "HAVE_ICONV"_d;
    libarchive.Public += "HAVE_ICONV_H"_d;
    libarchive += "ICONV_CONST=const"_v;
    libarchive.Public += "HAVE_LIBLZ4"_d;
    libarchive.Public += "HAVE_LIBLZMA"_d;
    libarchive.Public += "HAVE_LIBNETTLE"_d;
    libarchive.Public += "HAVE_LIBXML_XMLREADER_H"_d;
    libarchive.Public += "HAVE_LIBXML_XMLWRITER_H"_d;
    libarchive.Public += "HAVE_LOCALCHARSET_H"_d;
    libarchive.Public += "HAVE_LOCALE_CHARSET"_d;
    libarchive.Public += "HAVE_LZ4HC_H"_d;
    libarchive.Public += "HAVE_LZ4_H"_d;
    libarchive.Public += "HAVE_LZMA_H"_d;
    libarchive.Public += "HAVE_LZO_LZO1X_H"_d;
    libarchive.Public += "HAVE_LZO_LZOCONF_H"_d;
    libarchive.Public += "HAVE_NETTLE_AES_H"_d;
    libarchive.Public += "HAVE_NETTLE_HMAC_H"_d;
    libarchive.Public += "HAVE_NETTLE_MD5_H"_d;
    libarchive.Public += "HAVE_NETTLE_PBKDF2_H"_d;
    libarchive.Public += "HAVE_NETTLE_RIPEMD160_H"_d;
    libarchive.Public += "HAVE_NETTLE_SHA_H"_d;
    libarchive.Public += "HAVE_ZLIB_H"_d;
    libarchive.Public += "HAVE_ZSTD_H"_d;

    auto win_or_mingw =
        libarchive.getBuildSettings().TargetOS.Type == OSType::Windows ||
        libarchive.getBuildSettings().TargetOS.Type == OSType::Mingw
        ;

    if (win_or_mingw)
        libarchive.Public += "Advapi32.lib"_slib, "User32.lib"_slib;

    libarchive.Public += "org.sw.demo.bzip2-1"_dep;
    libarchive.Public += "org.sw.demo.gnu.nettle.nettle-3"_dep;
    libarchive.Public += "org.sw.demo.lz4-1"_dep;
    libarchive.Public += "org.sw.demo.xmlsoft.libxml2"_dep;
    libarchive.Public += "org.sw.demo.madler.zlib-1"_dep;
    libarchive.Public += "org.sw.demo.oberhumer.lzo.lzo-2"_dep;
    libarchive.Public += "org.sw.demo.xz_utils.lzma-5"_dep;
    libarchive.Public += "org.sw.demo.facebook.zstd.zstd-1"_dep;

    libarchive.Variables["HAVE_WCSCPY"] = 1;
    libarchive.Variables["HAVE_WCSLEN"] = 1;
    libarchive.Variables["HAVE_WMEMCMP"] = 1;

    if (!libarchive.Variables["HAVE_DEV_T"])
    {
        if (libarchive.getCompilerType() == CompilerType::MSVC)
            libarchive.Variables["dev_t"] = "unsigned int";
    }

    if (!libarchive.Variables["HAVE_GID_T"])
    {
        if (libarchive.getBuildSettings().TargetOS.Type == OSType::Windows)
            libarchive.Variables["gid_t"] = "short";
        else
            libarchive.Variables["gid_t"] = "unsigned int";
    }

    if (!libarchive.Variables["HAVE_ID_T"])
    {
        if (libarchive.getBuildSettings().TargetOS.Type == OSType::Windows)
            libarchive.Variables["id_t"] = "short";
        else
            libarchive.Variables["id_t"] = "unsigned int";
    }

    if (!libarchive.Variables["HAVE_UID_T"])
    {
        if (libarchive.getBuildSettings().TargetOS.Type == OSType::Windows)
            libarchive.Variables["uid_t"] = "short";
        else
            libarchive.Variables["uid_t"] = "unsigned int";
    }

    if (!libarchive.Variables["HAVE_MODE_T"])
    {
        if (libarchive.getBuildSettings().TargetOS.Type == OSType::Windows)
            libarchive.Variables["mode_t"] = "unsigned short";
        else
            libarchive.Variables["mode_t"] = "int";
    }

    if (!libarchive.Variables["HAVE_OFF_T"])
    {
        libarchive.Variables["off_t"] = "__int64";
    }

    if (!libarchive.Variables["HAVE_SIZE_T"])
    {
        if (libarchive.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
            libarchive.Variables["size_t"] = "uint64_t";
        else
            libarchive.Variables["size_t"] = "uint32_t";
    }

    if (!libarchive.Variables["HAVE_SSIZE_T"])
    {
        if (libarchive.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
            libarchive.Variables["ssize_t"] = "int64_t";
        else
            libarchive.Variables["ssize_t"] = "long";
    }

    if (!libarchive.Variables["HAVE_PID_T"])
    {
        if (libarchive.getBuildSettings().TargetOS.Type == OSType::Windows)
            libarchive.Variables["pid_t"] = "int";
        else
            libarchive.Variables["pid_t"] = "pid_t";
    }

    if (!libarchive.Variables["HAVE_INTPTR_T"])
    {
        if (libarchive.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
            libarchive.Variables["intptr_t"] = "int64_t";
        else
            libarchive.Variables["intptr_t"] = "int32_t";
    }

    if (!libarchive.Variables["HAVE_UINTPTR_T"])
    {
        if (libarchive.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
            libarchive.Variables["intptr_t"] = "uint64_t";
        else
            libarchive.Variables["intptr_t"] = "uint32_t";
    }

    if (!libarchive.Variables["HAVE_SIZEOF_WCHAR_T"])
    {
        libarchive.Variables["HAVE_WCHAR_T"] = 1;
    }

    // TODO: detect it properly with check
    if (win_or_mingw)
        libarchive.Variables["HAVE_WINCRYPT_H"] = 1;

    // TODO: add 'or cygwin'
    // IF(NOT WIN32 OR CYGWIN)
    if (!win_or_mingw)
    {
        libarchive -=
            "libarchive/archive_entry_copy_bhfi.c",
            "libarchive/archive_read_disk_windows.c",
            "libarchive/archive_windows.c",
            "libarchive/archive_windows.c",
            "libarchive/archive_write_disk_windows.c",
            "libarchive/filter_fork_windows.c";
    }

    // if (UNIX)
    if (!win_or_mingw)
    {
        // TODO:
        /*
        # acl
        if (UNIX)
        CHECK_LIBRARY_EXISTS(acl "acl_get_file" "" HAVE_LIBACL)
        IF(HAVE_LIBACL)
        SET(CMAKE_REQUIRED_LIBRARIES "acl")
        FIND_LIBRARY(ACL_LIBRARY NAMES acl)
        LIST(APPEND ADDITIONAL_LIBS ${ACL_LIBRARY})
        ENDIF(HAVE_LIBACL)
        #
        include(build/cmake/CheckFuncs.cmake)
        include(build/cmake/CheckTypeExists.cmake)
        CHECK_FUNCTION_EXISTS_GLIBC(acl_create_entry HAVE_ACL_CREATE_ENTRY)
        CHECK_FUNCTION_EXISTS_GLIBC(acl_init HAVE_ACL_INIT)
        CHECK_FUNCTION_EXISTS_GLIBC(acl_set_fd HAVE_ACL_SET_FD)
        CHECK_FUNCTION_EXISTS_GLIBC(acl_set_fd_np HAVE_ACL_SET_FD_NP)
        CHECK_FUNCTION_EXISTS_GLIBC(acl_set_file HAVE_ACL_SET_FILE)
        CHECK_TYPE_EXISTS(acl_permset_t "${INCLUDES}"    HAVE_ACL_PERMSET_T)

        # The "acl_get_perm()" function was omitted from the POSIX draft.
        # (It's a pretty obvious oversight; otherwise, there's no way to
        # test for specific permissions in a permset.)  Linux uses the obvious
        # name, FreeBSD adds _np to mark it as "non-Posix extension."
        # Test for both as a double-check that we really have POSIX-style ACL support.
        CHECK_FUNCTION_EXISTS(acl_get_fd_np HAVE_ACL_GET_FD_NP)
        CHECK_FUNCTION_EXISTS(acl_get_perm HAVE_ACL_GET_PERM)
        CHECK_FUNCTION_EXISTS(acl_get_perm_np HAVE_ACL_GET_PERM_NP)
        CHECK_FUNCTION_EXISTS(acl_get_link HAVE_ACL_GET_LINK)
        CHECK_FUNCTION_EXISTS(acl_get_link_np HAVE_ACL_GET_LINK_NP)
        CHECK_FUNCTION_EXISTS(acl_is_trivial_np HAVE_ACL_IS_TRIVIAL_NP)
        CHECK_FUNCTION_EXISTS(acl_set_link_np HAVE_ACL_SET_LINK_NP)

        # MacOS has an acl.h that isn't POSIX.  It can be detected by
        # checking for ACL_USER
        CHECK_SYMBOL_EXISTS(ACL_USER "${INCLUDES}" HAVE_ACL_USER)
        endif()
        */
    }

    libarchive += "__LIBARCHIVE_BUILD"_d;
    libarchive.Public += sw::Static, "LIBARCHIVE_STATIC"_d;
    libarchive.patch("libarchive/archive_platform.h", "#define	__LIBARCHIVE_BUILD 1", "//#define	__LIBARCHIVE_BUILD   1");

    libarchive.ApiName = "SW_LIBARCHIVE_API";
    libarchive.replaceInFileOnce("libarchive/archive.h", "# define __LA_DECL", "#  define __LA_DECL SW_LIBARCHIVE_API");
    libarchive.replaceInFileOnce("libarchive/archive_entry.h", "# define __LA_DECL", "#  define __LA_DECL SW_LIBARCHIVE_API");

    libarchive.configureFile("build/cmake/config.h.in", libarchive.BinaryPrivateDir / "config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("libarchive");
    s.checkFunctionExists("acl_create_entry");
    s.checkFunctionExists("acl_get_fd_np");
    s.checkFunctionExists("acl_get_link");
    s.checkFunctionExists("acl_get_link_np");
    s.checkFunctionExists("acl_get_perm");
    s.checkFunctionExists("acl_get_perm_np");
    s.checkFunctionExists("acl_init");
    s.checkFunctionExists("acl_is_trivial_np");
    s.checkFunctionExists("acl_set_link_np");
    s.checkFunctionExists("acl_set_fd");
    s.checkFunctionExists("acl_set_fd_np");
    s.checkFunctionExists("acl_set_file");
    s.checkFunctionExists("arc4random_buf");
    s.checkFunctionExists("chflags");
    s.checkFunctionExists("chown");
    s.checkFunctionExists("chroot");
    s.checkFunctionExists("ctime_r");
    s.checkFunctionExists("cygwin_conv_path");
    s.checkFunctionExists("dirfd");
    s.checkFunctionExists("extattr_get_file");
    s.checkFunctionExists("extattr_list_file");
    s.checkFunctionExists("extattr_set_fd");
    s.checkFunctionExists("extattr_set_file");
    s.checkFunctionExists("fchdir");
    s.checkFunctionExists("fchflags");
    s.checkFunctionExists("fchmod");
    s.checkFunctionExists("fchown");
    s.checkFunctionExists("fcntl");
    s.checkFunctionExists("fdopendir");
    s.checkFunctionExists("fgetea");
    s.checkFunctionExists("fgetxattr");
    s.checkFunctionExists("flistea");
    s.checkFunctionExists("flistxattr");
    s.checkFunctionExists("fork");
    s.checkFunctionExists("fseeko");
    s.checkFunctionExists("fsetea");
    s.checkFunctionExists("fsetxattr");
    s.checkFunctionExists("fstat");
    s.checkFunctionExists("fstatat");
    s.checkFunctionExists("fstatfs");
    s.checkFunctionExists("fstatvfs");
    s.checkFunctionExists("ftruncate");
    s.checkFunctionExists("futimens");
    s.checkFunctionExists("futimes");
    s.checkFunctionExists("futimesat");
    s.checkFunctionExists("getea");
    s.checkFunctionExists("geteuid");
    s.checkFunctionExists("getgrgid_r");
    s.checkFunctionExists("getgrnam_r");
    s.checkFunctionExists("getpid");
    s.checkFunctionExists("getpwnam_r");
    s.checkFunctionExists("getpwuid_r");
    s.checkFunctionExists("getvfsbyname");
    s.checkFunctionExists("getxattr");
    s.checkFunctionExists("gmtime_r");
    s.checkFunctionExists("lchflags");
    s.checkFunctionExists("lchmod");
    s.checkFunctionExists("lchown");
    s.checkFunctionExists("lgetea");
    s.checkFunctionExists("lgetxattr");
    s.checkFunctionExists("link");
    s.checkFunctionExists("listea");
    s.checkFunctionExists("listxattr");
    s.checkFunctionExists("llistea");
    s.checkFunctionExists("llistxattr");
    s.checkFunctionExists("locale_charset");
    s.checkFunctionExists("localtime_r");
    s.checkFunctionExists("lsetea");
    s.checkFunctionExists("lsetxattr");
    s.checkFunctionExists("lstat");
    s.checkFunctionExists("lutimes");
    s.checkFunctionExists("mbrtowc");
    s.checkFunctionExists("memcmp");
    s.checkFunctionExists("memmove");
    s.checkFunctionExists("memset");
    s.checkFunctionExists("mkdir");
    s.checkFunctionExists("mkfifo");
    s.checkFunctionExists("mknod");
    s.checkFunctionExists("mkstemp");
    s.checkFunctionExists("nl_langinfo");
    s.checkFunctionExists("openat");
    s.checkFunctionExists("pipe");
    s.checkFunctionExists("PKCS5_PBKDF2_HMAC_SHA1");
    s.checkFunctionExists("poll");
    s.checkFunctionExists("posix_spawnp");
    s.checkFunctionExists("readlink");
    s.checkFunctionExists("readlinkat");
    s.checkFunctionExists("readpassphrase");
    s.checkFunctionExists("regcomp");
    s.checkFunctionExists("select");
    s.checkFunctionExists("setenv");
    s.checkFunctionExists("setlocale");
    s.checkFunctionExists("sigaction");
    s.checkFunctionExists("stat");
    s.checkFunctionExists("statfs");
    s.checkFunctionExists("statvfs");
    s.checkFunctionExists("strchr");
    s.checkFunctionExists("strdup");
    s.checkFunctionExists("strerror");
    s.checkFunctionExists("strerror_r");
    s.checkFunctionExists("strftime");
    s.checkFunctionExists("strncpy_s");
    s.checkFunctionExists("strrchr");
    s.checkFunctionExists("symlink");
    s.checkFunctionExists("timegm");
    s.checkFunctionExists("tzset");
    s.checkFunctionExists("unsetenv");
    s.checkFunctionExists("utime");
    s.checkFunctionExists("utimensat");
    s.checkFunctionExists("utimes");
    s.checkFunctionExists("vfork");
    s.checkFunctionExists("vprintf");
    s.checkFunctionExists("wcrtomb");
    s.checkFunctionExists("wcscmp");
    s.checkFunctionExists("wcscpy");
    s.checkFunctionExists("wcslen");
    s.checkFunctionExists("wctomb");
    s.checkFunctionExists("wmemcmp");
    s.checkFunctionExists("wmemcpy");
    s.checkFunctionExists("wmemmove");
    s.checkFunctionExists("_ctime64_s");
    s.checkFunctionExists("_fseeki64");
    s.checkFunctionExists("_get_timezone");
    s.checkFunctionExists("_localtime64_s");
    s.checkFunctionExists("_mkgmtime64");
    s.checkIncludeExists("acl/libacl.h");
    s.checkIncludeExists("attr/xattr.h");
    s.checkIncludeExists("Bcrypt.h");
    s.checkIncludeExists("copyfile.h");
    s.checkIncludeExists("ctype.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("expat.h");
    s.checkIncludeExists("ext2fs/ext2_fs.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("grp.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("langinfo.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("linux/fiemap.h");
    s.checkIncludeExists("linux/fs.h");
    s.checkIncludeExists("linux/magic.h");
    s.checkIncludeExists("linux/types.h");
    s.checkIncludeExists("localcharset.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("md5.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("paths.h");
    s.checkIncludeExists("pcreposix.h");
    s.checkIncludeExists("poll.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("pwd.h");
    s.checkIncludeExists("readpassphrase.h");
    s.checkIncludeExists("regex.h");
    s.checkIncludeExists("ripemd.h");
    s.checkIncludeExists("sha256.h");
    s.checkIncludeExists("sha512.h");
    s.checkIncludeExists("sha.h");
    s.checkIncludeExists("signal.h");
    s.checkIncludeExists("spawn.h");
    s.checkIncludeExists("stdarg.h");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/acl.h");
    s.checkIncludeExists("sys/cdefs.h");
    s.checkIncludeExists("sys/ea.h");
    s.checkIncludeExists("sys/extattr.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/mkdev.h");
    s.checkIncludeExists("sys/mount.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/poll.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/statfs.h");
    s.checkIncludeExists("sys/statvfs.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/utime.h");
    s.checkIncludeExists("sys/utsname.h");
    s.checkIncludeExists("sys/vfs.h");
    s.checkIncludeExists("sys/xattr.h");
    s.checkIncludeExists("time.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("utime.h");
    s.checkIncludeExists("wchar.h");
    s.checkIncludeExists("wctype.h");
    {
        // still does not work
        // investigate more
        auto &c = s.checkIncludeExists("wincrypt.h");
        c.Parameters.Includes.push_back("windows.h");
    }
    s.checkIncludeExists("windows.h");
    s.checkIncludeExists("winioctl.h");
    s.checkTypeSize("acl_permset_t");
    s.checkTypeSize("dev_t");
    s.checkTypeSize("gid_t");
    s.checkTypeSize("id_t");
    s.checkTypeSize("int16_t");
    s.checkTypeSize("int32_t");
    s.checkTypeSize("int64_t");
    s.checkTypeSize("intmax_t");
    s.checkTypeSize("intptr_t");
    s.checkTypeSize("long");
    s.checkTypeSize("mode_t");
    s.checkTypeSize("off_t");
    s.checkTypeSize("pid_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("uid_t");
    s.checkTypeSize("uint16_t");
    s.checkTypeSize("uint32_t");
    s.checkTypeSize("uint64_t");
    s.checkTypeSize("uint8_t");
    s.checkTypeSize("uintmax_t");
    s.checkTypeSize("uintptr_t");
    s.checkTypeSize("unsigned");
    s.checkTypeSize("vfsconf");
    s.checkTypeSize("void *");
    s.checkTypeSize("wchar_t");
    s.checkTypeSize("xvfsconf");
    s.checkLibraryFunctionExists("acl", "acl_get_file");
    s.checkLibraryFunctionExists("charset", "locale_charset");
    s.checkLibraryFunctionExists("md", "MD5Init");
    {
        auto &str = s.checkStructMemberExists("struct dirent", "d_namlen");
        str.Parameters.Includes.push_back("dirent.h");
    }
    {
        auto &str = s.checkStructMemberExists("struct statfs", "f_namemax");
    }
    {
        auto &str = s.checkStructMemberExists("struct statvfs", "f_iosize");
    }
    {
        auto &str = s.checkStructMemberExists("struct stat", "st_birthtime");
        str.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &str = s.checkStructMemberExists("struct stat", "st_birthtimespec.tv_nsec");
        str.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &str = s.checkStructMemberExists("struct stat", "st_blksize");
        str.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &str = s.checkStructMemberExists("struct stat", "st_flags");
        str.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &str = s.checkStructMemberExists("struct stat", "st_mtimespec.tv_nsec");
        str.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &str = s.checkStructMemberExists("struct stat", "st_mtime_n");
        str.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &str = s.checkStructMemberExists("struct stat", "st_mtime_usec");
        str.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &str = s.checkStructMemberExists("struct stat", "st_mtim.tv_nsec");
        str.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &str = s.checkStructMemberExists("struct stat", "st_umtime");
        str.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &str = s.checkStructMemberExists("struct tm", "tm_gmtoff");
        str.Parameters.Includes.push_back("time.h");
    }
    {
        auto &str = s.checkStructMemberExists("struct tm", "__tm_gmtoff");
        str.Parameters.Includes.push_back("time.h");
    }
    s.checkSourceCompiles("HAVE_READDIR_R", R"sw_xxx(#include <dirent.h>

int main() {

DIR *dir; struct dirent e, *r;
        return(readdir_r(dir, &e, &r));

; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_TIME_WITH_SYS_TIME", R"sw_xxx(
#include <time.h>
#include <sys/time.h>
int main() {return 0;}
)sw_xxx");
    s.checkSourceCompiles("MAJOR_IN_MKDEV", R"sw_xxx(
#include <sys/mkdev.h>
int main() { makedev(0, 0); return 0; }
)sw_xxx");
    s.checkSourceCompiles("MAJOR_IN_SYSMACROS", R"sw_xxx(
#include <sys/sysmacros.h>
int main() { makedev(0, 0); return 0; }
)sw_xxx");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <float.h>
int main() {return 0;}
)sw_xxx");
    s.checkDeclarationExists("ACL_USER");
    s.checkDeclarationExists("D_MD_ORDER");
    s.checkDeclarationExists("EFTYPE");
    s.checkDeclarationExists("EILSEQ");
    s.checkDeclarationExists("EXTATTR_NAMESPACE_USER");
    s.checkDeclarationExists("INT32_MAX");
    s.checkDeclarationExists("INT32_MIN");
    s.checkDeclarationExists("INT64_MAX");
    s.checkDeclarationExists("INT64_MIN");
    s.checkDeclarationExists("INTMAX_MAX");
    s.checkDeclarationExists("INTMAX_MIN");
    s.checkDeclarationExists("SIZE_MAX");
    s.checkDeclarationExists("SSIZE_MAX");
    s.checkDeclarationExists("UINT32_MAX");
    s.checkDeclarationExists("UINT64_MAX");
    s.checkDeclarationExists("UINTMAX_MAX");
}
