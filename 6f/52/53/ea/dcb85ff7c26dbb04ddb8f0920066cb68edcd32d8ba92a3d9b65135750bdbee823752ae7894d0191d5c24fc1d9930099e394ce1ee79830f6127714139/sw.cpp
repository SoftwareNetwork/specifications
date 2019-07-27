void build(Solution &s)
{
    auto &uClibc_ng = s.addProject("uClibc_ng", "1.0.31");
    uClibc_ng += RemoteFile("https://downloads.uclibc-ng.org/releases/1.0.31/uClibc-ng-1.0.31.tar.xz");

    String arch;
    auto &config = uClibc_ng.addLibrary("config");
    {
        auto &t = config;
        t.AutoDetectOptions = false;
        t.HeaderOnly = true;

        //
        std::set<String> arch_list
        {
            "aarch64",
            "arm",
            "i386",
            "microblaze",
            "mips",
            "mipsn32",
            "mips64",
            "m68k",
            "or1k",
            "powerpc",
            "powerpc64",
            "s390x",
            "sh",
            "x32",
            "x86_64",
        };

        switch (t.getSettings().TargetOS.Arch)
        {
        case ArchType::x86_64:
            arch = toString(t.getSettings().TargetOS.Arch);
            break;
        case ArchType::x86:
            arch = "i386";
            break;
        default:
            throw SW_RUNTIME_ERROR("arch is not implemented");
        }

        t += "include/.*"_rr;
        t.Public += "include"_idir;

        t.Public += IncludeDirectory("libc/sysdeps/linux/" + arch); // before common
        t.Public += IncludeDirectory("libc/sysdeps/linux/common"s);
        t.Public += IncludeDirectory("libc/sysdeps/linux"s);

        // for stdio
        t.Protected += IncludeDirectory("libpthread/nptl"s);
        t.Protected += IncludeDirectory("libpthread/nptl/sysdeps/" + arch);
        t.Protected += IncludeDirectory("libpthread/nptl/sysdeps/pthread"s);
        t.Protected += IncludeDirectory("libpthread/nptl/sysdeps/unix/sysv/linux/" + arch);
        t.Protected += IncludeDirectory("libpthread/nptl/sysdeps/unix/sysv/linux/"s);

        if (t.getSettings().Native.LibrariesType == LibraryType::Static)
            t.Protected += "__BCC__"_def; // shared?

        t.Public.CompileOptions.push_back("-include");
        t.Public.CompileOptions.push_back(normalize_path(t.SourceDir / "include/libc-symbols.h"));

        // changes:
        //  xlocale
        //  custom streams
        //  susv4 compat

        t.writeFileOnce("bits/uClibc_config.h", R"(
#if !defined _FEATURES_H && !defined __need_uClibc_config_h
# error Never include <bits/uClibc_config.h> directly; use <features.h> instead
#endif

#define __UCLIBC_MAJOR__ 1
#define __UCLIBC_MINOR__ 0
#define __UCLIBC_SUBLEVEL__ 31
/* Automatically generated file; DO NOT EDIT. */
/* uClibc-ng 1.0.31 C Library Configuration */
#undef __TARGET_aarch64__
#undef __TARGET_alpha__
#undef __TARGET_arc__
#undef __TARGET_arm__
#undef __TARGET_avr32__
#undef __TARGET_bfin__
#undef __TARGET_c6x__
#undef __TARGET_cris__
#undef __TARGET_csky__
#undef __TARGET_frv__
#undef __TARGET_h8300__
#undef __TARGET_hppa__
#undef __TARGET_i386__
#undef __TARGET_ia64__
#undef __TARGET_lm32__
#undef __TARGET_m68k__
#undef __TARGET_metag__
#undef __TARGET_microblaze__
#undef __TARGET_mips__
#undef __TARGET_nds32__
#undef __TARGET_nios2__
#undef __TARGET_or1k__
#undef __TARGET_powerpc__
#undef __TARGET_riscv64__
#undef __TARGET_sh__
#undef __TARGET_sparc__
#undef __TARGET_sparc64__
#undef __TARGET_tile__
#define __TARGET_x86_64__ 1
#undef __TARGET_xtensa__

/* Target Architecture Features and Options */
#define __TARGET_ARCH__ "x86_64"
#define __FORCE_OPTIONS_FOR_ARCH__ 1
#define __TARGET_SUBARCH__ ""

/* Using ELF file format */
#define __ARCH_HAS_DEPRECATED_SYSCALLS__ 1
#define __ARCH_LITTLE_ENDIAN__ 1

/* Using Little Endian */
#define __ARCH_HAS_MMU__ 1
#define __ARCH_USE_MMU__ 1
#define __UCLIBC_HAS_FLOATS__ 1
#define __UCLIBC_HAS_FPU__ 1
#undef __DO_C99_MATH__
#undef __DO_XSI_MATH__
#undef __UCLIBC_HAS_FENV__
#define __KERNEL_HEADERS__ "/usr/include -I/usr/include/x86_64-linux-gnu/"
#define __HAVE_DOT_CONFIG__ 1

/* General Library Settings */
#define __DOPIC__ 1
#define __ARCH_HAS_UCONTEXT__ 1
#define __HAVE_SHARED__ 1
#undef __FORCE_SHAREABLE_TEXT_SEGMENTS__
#define __LDSO_LDD_SUPPORT__ 1
#define __LDSO_CACHE_SUPPORT__ 1
#define __LDSO_PRELOAD_ENV_SUPPORT__ 1
#undef __LDSO_PRELOAD_FILE_SUPPORT__
#define __LDSO_BASE_FILENAME__ "ld.so"
#undef __LDSO_STANDALONE_SUPPORT__
#undef __LDSO_PRELINK_SUPPORT__
#define __UCLIBC_STATIC_LDCONFIG__ 1
#define __LDSO_RUNPATH__ 1
#undef __LDSO_RUNPATH_OF_EXECUTABLE__
#define __LDSO_SAFE_RUNPATH__ 1
#define __LDSO_SEARCH_INTERP_PATH__ 1
#define __LDSO_LD_LIBRARY_PATH__ 1
#define __UCLIBC_CTOR_DTOR__ 1
#undef __LDSO_GNU_HASH_SUPPORT__
#define __HAS_NO_THREADS__ 1
#undef __UCLIBC_HAS_LINUXTHREADS__
#undef __UCLIBC_HAS_THREADS_NATIVE__
#define __UCLIBC_HAS_SYSLOG__ 1
#define __UCLIBC_HAS_LFS__ 1
#undef __MALLOC__
#undef __MALLOC_SIMPLE__
#define __MALLOC_STANDARD__ 1
#define __UCLIBC_DYNAMIC_ATEXIT__ 1
#undef __UCLIBC_HAS_UTMPX__
#undef __UCLIBC_SUSV2_LEGACY__
#undef __UCLIBC_SUSV3_LEGACY__
#undef __UCLIBC_SUSV3_LEGACY_MACROS__
#define __UCLIBC_SUSV4_LEGACY__ 1
#undef __UCLIBC_STRICT_HEADERS__
#undef __UCLIBC_HAS_STUBS__
#define __UCLIBC_HAS_SHADOW__ 1
#undef __UCLIBC_HAS_PROGRAM_INVOCATION_NAME__
#define __UCLIBC_HAS___PROGNAME__ 1
#define __UCLIBC_HAS_PTY__ 1
#define __ASSUME_DEVPTS__ 1
#define __UNIX98PTY_ONLY__ 1
#undef __UCLIBC_HAS_GETPT__
#undef __UCLIBC_HAS_LIBUTIL__
#define __UCLIBC_HAS_TM_EXTENSIONS__ 1
#define __UCLIBC_HAS_TZ_CACHING__ 1
#define __UCLIBC_HAS_TZ_FILE__ 1
#define __UCLIBC_HAS_TZ_FILE_READ_MANY__ 1
#define __UCLIBC_TZ_FILE_PATH__ "/etc/TZ"
#define __UCLIBC_FALLBACK_TO_ETC_LOCALTIME__ 1

/* Advanced Library Settings */
#define __UCLIBC_PWD_BUFFER_SIZE__ 256
#define __UCLIBC_GRP_BUFFER_SIZE__ 256

/* Support various families of functions */
#define __UCLIBC_LINUX_SPECIFIC__ 1
#define __UCLIBC_HAS_GNU_ERROR__ 1
#define __UCLIBC_BSD_SPECIFIC__ 1
#define __UCLIBC_HAS_BSD_ERR__ 1
#undef __UCLIBC_HAS_OBSOLETE_BSD_SIGNAL__
#undef __UCLIBC_HAS_OBSOLETE_SYSV_SIGNAL__
#undef __UCLIBC_NTP_LEGACY__
#undef __UCLIBC_SV4_DEPRECATED__
#define __UCLIBC_HAS_REALTIME__ 1
#define __UCLIBC_HAS_ADVANCED_REALTIME__ 1
#define __UCLIBC_HAS_EPOLL__ 1
#define __UCLIBC_HAS_XATTR__ 1
#define __UCLIBC_HAS_PROFILING__ 1
#define __UCLIBC_HAS_CRYPT_IMPL__ 1
#undef __UCLIBC_HAS_SHA256_CRYPT_IMPL__
#undef __UCLIBC_HAS_SHA512_CRYPT_IMPL__
#define __UCLIBC_HAS_CRYPT__ 1
#define __UCLIBC_HAS_NETWORK_SUPPORT__ 1
#define __UCLIBC_HAS_SOCKET__ 1
#define __UCLIBC_HAS_IPV4__ 1
#undef __UCLIBC_HAS_IPV6__
#undef __UCLIBC_USE_NETLINK__
#undef __UCLIBC_HAS_BSD_RES_CLOSE__
#define __UCLIBC_HAS_COMPAT_RES_STATE__ 1
#undef __UCLIBC_HAS_EXTRA_COMPAT_RES_STATE__
#undef __UCLIBC_HAS_RESOLVER_SUPPORT__

/* String and Stdio Support */
#define __UCLIBC_HAS_STRING_GENERIC_OPT__ 1
#define __UCLIBC_HAS_STRING_ARCH_OPT__ 1
#define __UCLIBC_HAS_CTYPE_TABLES__ 1
#define __UCLIBC_HAS_CTYPE_SIGNED__ 1
#define __UCLIBC_HAS_CTYPE_UNSAFE__ 1
#undef __UCLIBC_HAS_CTYPE_CHECKED__
#undef __UCLIBC_HAS_CTYPE_ENFORCED__
#define __UCLIBC_HAS_WCHAR__ 1
#define __UCLIBC_HAS_LIBICONV__ 1
#undef __UCLIBC_HAS_LIBINTL__
#define __UCLIBC_HAS_LOCALE__ 1
#undef __UCLIBC_BUILD_MINIMAL_LOCALE__
#define __UCLIBC_BUILD_ALL_LOCALE__ 1
#define __UCLIBC_HAS_XLOCALE__ 1
#undef __UCLIBC_HAS_HEXADECIMAL_FLOATS__
#undef __UCLIBC_HAS_GLIBC_DIGIT_GROUPING__
#undef __UCLIBC_HAS_GLIBC_CUSTOM_PRINTF__
#define __UCLIBC_PRINTF_SCANF_POSITIONAL_ARGS__ 9
#undef __UCLIBC_HAS_STDIO_BUFSIZ_256__
#undef __UCLIBC_HAS_STDIO_BUFSIZ_512__
#undef __UCLIBC_HAS_STDIO_BUFSIZ_1024__
#undef __UCLIBC_HAS_STDIO_BUFSIZ_2048__
#define __UCLIBC_HAS_STDIO_BUFSIZ_4096__ 1
#undef __UCLIBC_HAS_STDIO_BUFSIZ_8192__
#define __UCLIBC_HAS_STDIO_BUILTIN_BUFFER_NONE__ 1
#undef __UCLIBC_HAS_STDIO_BUILTIN_BUFFER_4__
#undef __UCLIBC_HAS_STDIO_BUILTIN_BUFFER_8__
#undef __UCLIBC_HAS_STDIO_SHUTDOWN_ON_ABORT__
#define __UCLIBC_HAS_STDIO_GETC_MACRO__ 1
#define __UCLIBC_HAS_STDIO_PUTC_MACRO__ 1
#define __UCLIBC_HAS_STDIO_AUTO_RW_TRANSITION__ 1
#undef __UCLIBC_HAS_FOPEN_LARGEFILE_MODE__
#undef __UCLIBC_HAS_FOPEN_EXCLUSIVE_MODE__
#undef __UCLIBC_HAS_FOPEN_CLOSEEXEC_MODE__
#define __UCLIBC_HAS_GLIBC_CUSTOM_STREAMS__ 1
#undef __UCLIBC_HAS_PRINTF_M_SPEC__
#define __UCLIBC_HAS_ERRNO_MESSAGES__ 1
#undef __UCLIBC_HAS_SYS_ERRLIST__
#define __UCLIBC_HAS_SIGNUM_MESSAGES__ 1
#undef __UCLIBC_HAS_SYS_SIGLIST__
#define __UCLIBC_HAS_GNU_GETOPT__ 1
#define __UCLIBC_HAS_GETOPT_LONG__ 1
#define __UCLIBC_HAS_GNU_GETSUBOPT__ 1
#undef __UCLIBC_HAS_ARGP__

/* Big and Tall */
#define __UCLIBC_HAS_REGEX__ 1
#define __UCLIBC_HAS_FNMATCH__ 1
#undef __UCLIBC_HAS_WORDEXP__
#undef __UCLIBC_HAS_NFTW__
#undef __UCLIBC_HAS_FTS__
#define __UCLIBC_HAS_GLOB__ 1
#undef __UCLIBC_HAS_GNU_GLOB__

/* Library Installation Options */
#define __RUNTIME_PREFIX__ "/usr/$(TARGET_ARCH)-linux-uclibc/"
#define __DEVEL_PREFIX__ "/usr/$(TARGET_ARCH)-linux-uclibc/usr/"
#define __MULTILIB_DIR__ "lib"
#define __HARDWIRED_ABSPATH__ 1

/* Security options */
#undef __UCLIBC_HAS_SSP__
#define __UCLIBC_BUILD_RELRO__ 1
#undef __UCLIBC_BUILD_NOW__
#define __UCLIBC_BUILD_NOEXECSTACK__ 1

/* Development/debugging options */
#define __CROSS_COMPILER_PREFIX__ ""
#define __UCLIBC_EXTRA_CFLAGS__ ""
#undef __DODEBUG__
#define __DOSTRIP__ 1
#undef __DOASSERTS__
#undef __SUPPORT_LD_DEBUG__
#undef __SUPPORT_LD_DEBUG_EARLY__
#undef __UCLIBC_MALLOC_DEBUGGING__
#undef __UCLIBC_HAS_BACKTRACE__
#define __WARNINGS__ "-Wall"
#undef __EXTRA_WARNINGS__
)");

        //
        t.writeFileOnce("bits/uClibc_locale_data.h", R"(
#define __lc_time_data_LEN		1034
#define __lc_time_rows_LEN		700
#define __lc_time_item_offsets_LEN		50
#define __lc_time_item_idx_LEN		123
#define __lc_numeric_data_LEN		14
#define __lc_numeric_rows_LEN		15
#define __lc_numeric_item_offsets_LEN		3
#define __lc_numeric_item_idx_LEN		9
#define __lc_monetary_data_LEN		184
#define __lc_monetary_rows_LEN		374
#define __lc_monetary_item_offsets_LEN		22
#define __lc_monetary_item_idx_LEN		99
#define __lc_messages_data_LEN		96
#define __lc_messages_rows_LEN		20
#define __lc_messages_item_offsets_LEN		4
#define __lc_messages_item_idx_LEN		14
#define __lc_ctype_data_LEN		21
#define __lc_ctype_rows_LEN		10
#define __lc_ctype_item_offsets_LEN		10
#define __lc_ctype_item_idx_LEN		10
#define __CTYPE_HAS_UTF_8_LOCALES			1
#define __LOCALE_DATA_CATEGORIES			6
#define __LOCALE_DATA_WIDTH_LOCALES			9
#define __LOCALE_DATA_NUM_LOCALES			314
#define __LOCALE_DATA_NUM_LOCALE_NAMES		167
#define __LOCALE_DATA_AT_MODIFIERS_LENGTH		18
#define __lc_names_LEN		69
#define __lc_collate_data_LEN  91141
#define __CTYPE_HAS_8_BIT_LOCALES		1

#define __LOCALE_DATA_Cctype_IDX_SHIFT	3
#define __LOCALE_DATA_Cctype_IDX_LEN		16
#define __LOCALE_DATA_Cctype_ROW_LEN		4
#define __LOCALE_DATA_Cctype_PACKED		1

#define __LOCALE_DATA_Cuplow_IDX_SHIFT	3
#define __LOCALE_DATA_Cuplow_IDX_LEN		16
#define __LOCALE_DATA_Cuplow_ROW_LEN		8

#define __LOCALE_DATA_Cc2wc_IDX_LEN		16
#define __LOCALE_DATA_Cc2wc_IDX_SHIFT		3
#define __LOCALE_DATA_Cc2wc_ROW_LEN		8

typedef struct {
	unsigned char idx8ctype[16];
	unsigned char idx8uplow[16];
	unsigned char idx8c2wc[16];
	unsigned char idx8wc2c[38];
} __codeset_8_bit_t;


#define __LOCALE_DATA_Cwc2c_DOMAIN_MAX	0x25ff
#define __LOCALE_DATA_Cwc2c_TI_SHIFT		4
#define __LOCALE_DATA_Cwc2c_TT_SHIFT		4
#define __LOCALE_DATA_Cwc2c_II_LEN		38
#define __LOCALE_DATA_Cwc2c_TI_LEN		1072
#define __LOCALE_DATA_Cwc2c_TT_LEN		3456


#define __LOCALE_DATA_Cwc2c_TBL_LEN		4528

#define __LOCALE_DATA_Cuplow_TBL_LEN		504


#define __LOCALE_DATA_Cctype_TBL_LEN		408


#define __LOCALE_DATA_Cc2wc_TBL_LEN		1760



#define __LOCALE_DATA_NUM_CODESETS		23
#define __LOCALE_DATA_CODESET_LIST \
	"\x18\x22\x28\x2f\x36\x42\x4d\x59" \
	"\x65\x71\x7d\x89\x94\x9f\xaa\xb5" \
	"\xc0\xcb\xd6\xe1\xe8\xef\xf6" \
	"\0" \
	"ARMSCII-8\0" \
	"ASCII\0" \
	"CP1251\0" \
	"CP1255\0" \
	"GEORGIAN-PS\0" \
	"ISO-8859-1\0" \
	"ISO-8859-10\0" \
	"ISO-8859-13\0" \
	"ISO-8859-14\0" \
	"ISO-8859-15\0" \
	"ISO-8859-16\0" \
	"ISO-8859-2\0" \
	"ISO-8859-3\0" \
	"ISO-8859-4\0" \
	"ISO-8859-5\0" \
	"ISO-8859-6\0" \
	"ISO-8859-7\0" \
	"ISO-8859-8\0" \
	"ISO-8859-9\0" \
	"KOI8-R\0" \
	"KOI8-T\0" \
	"KOI8-U\0" \
	"TIS-620\0"

#define __CTYPE_HAS_CODESET_ARMSCII_8
#define __CTYPE_HAS_CODESET_ASCII
#define __CTYPE_HAS_CODESET_CP1251
#define __CTYPE_HAS_CODESET_CP1255
#define __CTYPE_HAS_CODESET_GEORGIAN_PS
#define __CTYPE_HAS_CODESET_ISO_8859_1
#define __CTYPE_HAS_CODESET_ISO_8859_10
#define __CTYPE_HAS_CODESET_ISO_8859_13
#define __CTYPE_HAS_CODESET_ISO_8859_14
#define __CTYPE_HAS_CODESET_ISO_8859_15
#define __CTYPE_HAS_CODESET_ISO_8859_16
#define __CTYPE_HAS_CODESET_ISO_8859_2
#define __CTYPE_HAS_CODESET_ISO_8859_3
#define __CTYPE_HAS_CODESET_ISO_8859_4
#define __CTYPE_HAS_CODESET_ISO_8859_5
#define __CTYPE_HAS_CODESET_ISO_8859_6
#define __CTYPE_HAS_CODESET_ISO_8859_7
#define __CTYPE_HAS_CODESET_ISO_8859_8
#define __CTYPE_HAS_CODESET_ISO_8859_9
#define __CTYPE_HAS_CODESET_KOI8_R
#define __CTYPE_HAS_CODESET_KOI8_T
#define __CTYPE_HAS_CODESET_KOI8_U
#define __CTYPE_HAS_CODESET_TIS_620
#define __CTYPE_HAS_CODESET_UTF_8
#define __LOCALE_DATA_WC_TABLE_DOMAIN_MAX   0x2ffff

#define __LOCALE_DATA_WCctype_II_LEN         48
#define __LOCALE_DATA_WCctype_TI_LEN         64
#define __LOCALE_DATA_WCctype_UT_LEN        128
#define __LOCALE_DATA_WCctype_II_SHIFT        5
#define __LOCALE_DATA_WCctype_TI_SHIFT        7


#define __LOCALE_DATA_WCuplow_II_LEN         96
#define __LOCALE_DATA_WCuplow_TI_LEN         64
#define __LOCALE_DATA_WCuplow_UT_LEN        128
#define __LOCALE_DATA_WCuplow_II_SHIFT        5
#define __LOCALE_DATA_WCuplow_TI_SHIFT        6


#define __LOCALE_DATA_WCuplow_diffs        3


/* #define __LOCALE_DATA_MAGIC_SIZE 64 */
#ifndef __WCHAR_ENABLED
#if 0
#warning WHOA!!! __WCHAR_ENABLED is not defined! defining it now...
#endif
#define __WCHAR_ENABLED
#endif

/* TODO - fix */
#ifdef __WCHAR_ENABLED
#define __LOCALE_DATA_WCctype_TBL_LEN      (__LOCALE_DATA_WCctype_II_LEN + __LOCALE_DATA_WCctype_TI_LEN + __LOCALE_DATA_WCctype_UT_LEN)
#define __LOCALE_DATA_WCuplow_TBL_LEN      (__LOCALE_DATA_WCuplow_II_LEN + __LOCALE_DATA_WCuplow_TI_LEN + __LOCALE_DATA_WCuplow_UT_LEN)
#define __LOCALE_DATA_WCuplow_diff_TBL_LEN (2 * __LOCALE_DATA_WCuplow_diffs)
/* #define WCcomb_TBL_LEN		(WCcomb_II_LEN + WCcomb_TI_LEN + WCcomb_UT_LEN) */
#endif

#undef __PASTE2
#define __PASTE2(A,B)   A ## B
#undef __PASTE3
#define __PASTE3(A,B,C) A ## B ## C

#define __LOCALE_DATA_COMMON_MMAP(X) \
	unsigned char   __PASTE3(lc_,X,_data)[__PASTE3(__lc_,X,_data_LEN)];

#define __LOCALE_DATA_COMMON_MMIDX(X) \
	unsigned char   __PASTE3(lc_,X,_rows)[__PASTE3(__lc_,X,_rows_LEN)]; \
	uint16_t        __PASTE3(lc_,X,_item_offsets)[__PASTE3(__lc_,X,_item_offsets_LEN)]; \
	uint16_t        __PASTE3(lc_,X,_item_idx)[__PASTE3(__lc_,X,_item_idx_LEN)]; \


typedef struct {
#ifdef __LOCALE_DATA_MAGIC_SIZE
	unsigned char magic[__LOCALE_DATA_MAGIC_SIZE];
#endif

#ifdef __CTYPE_HAS_8_BIT_LOCALES
	const unsigned char tbl8ctype[__LOCALE_DATA_Cctype_TBL_LEN];
	const unsigned char tbl8uplow[__LOCALE_DATA_Cuplow_TBL_LEN];
#ifdef __WCHAR_ENABLED
	const uint16_t tbl8c2wc[__LOCALE_DATA_Cc2wc_TBL_LEN]; /* char > 0x7f to wide char */
	const unsigned char tbl8wc2c[__LOCALE_DATA_Cwc2c_TBL_LEN];
	/* translit  */
#endif /* __WCHAR_ENABLED */
#endif /* __CTYPE_HAS_8_BIT_LOCALES */
#ifdef __WCHAR_ENABLED
	const unsigned char tblwctype[__LOCALE_DATA_WCctype_TBL_LEN];
	const unsigned char tblwuplow[__LOCALE_DATA_WCuplow_TBL_LEN];
	const int32_t tblwuplow_diff[__LOCALE_DATA_WCuplow_diff_TBL_LEN];
/* 	const unsigned char tblwcomb[WCcomb_TBL_LEN]; */
	/* width?? */
#endif

	__LOCALE_DATA_COMMON_MMAP(ctype)
	__LOCALE_DATA_COMMON_MMAP(numeric)
	__LOCALE_DATA_COMMON_MMAP(monetary)
	__LOCALE_DATA_COMMON_MMAP(time)
	/* collate is different */
	__LOCALE_DATA_COMMON_MMAP(messages)


#ifdef __CTYPE_HAS_8_BIT_LOCALES
	const __codeset_8_bit_t codeset_8_bit[__LOCALE_DATA_NUM_CODESETS];
#endif

	__LOCALE_DATA_COMMON_MMIDX(ctype)
	__LOCALE_DATA_COMMON_MMIDX(numeric)
	__LOCALE_DATA_COMMON_MMIDX(monetary)
	__LOCALE_DATA_COMMON_MMIDX(time)
	/* collate is different */
	__LOCALE_DATA_COMMON_MMIDX(messages)

	const uint16_t collate_data[__lc_collate_data_LEN];

	unsigned char lc_common_item_offsets_LEN[__LOCALE_DATA_CATEGORIES];
	size_t lc_common_tbl_offsets[__LOCALE_DATA_CATEGORIES * 4];
	/* offsets from start of locale_mmap_t */
	/* rows, item_offsets, item_idx, data */

#ifdef __LOCALE_DATA_NUM_LOCALES
	unsigned char locales[__LOCALE_DATA_NUM_LOCALES * __LOCALE_DATA_WIDTH_LOCALES];
	unsigned char locale_names5[5*__LOCALE_DATA_NUM_LOCALE_NAMES];
	unsigned char locale_at_modifiers[__LOCALE_DATA_AT_MODIFIERS_LENGTH];
#endif

	unsigned char lc_names[__lc_names_LEN];
#ifdef __CTYPE_HAS_8_BIT_LOCALES
	unsigned char codeset_list[sizeof(__LOCALE_DATA_CODESET_LIST)]; /* TODO - fix */
#endif
} __locale_mmap_t;

extern const __locale_mmap_t *__locale_mmap;
)");
    }

    auto &libc = uClibc_ng.addLibrary("libc");
    {
        auto &t = libc;

        t += IncludeDirectory("ldso/ldso/" + arch);
        t += "ldso/include"_idir;

        t.setSourceDirectory("libc");

        t.setExtensionProgram(".S", t.findProgramByExtension(".c")->clone());

        //t += "../extra/locale/locale_data.c";
        //t["../extra/locale/locale_data.c"].args.push_back("-D__WCHAR_ENABLED");
        //t += "inet/.*"_rr;
        t += "misc/.*"_rr;
        t -= "misc/fnmatch/.*"_rr;
        t -= "misc/glob/.*"_rr;
        //t += "misc/glob/glob64.c";
        t -= "misc/regex/.*"_rr;
        t += "misc/regex/regex.c";
        t -= "misc/statfs/.*"_rr;
        t += "pwd_grp/.*"_rr;
        t -= "pwd_grp/pwd_grp_internal.c";
        t += "signal/.*"_rr;
        t += "stdio/.*"_rr;
        t -= "stdio/old_vfprintf.c";
        t += "stdlib/.*"_rr;
        t -= "stdlib/malloc/malloc_debug.c";
        t -= "stdlib/malloc-.*"_rr;
        t += "string/.*"_r;
        t += FileRegex("string/" + arch, "/.*", true);
        //t -= "string/.*casecmp.*"_rr;
        t += "sysdeps/linux/common/.*"_rr;
        t += FileRegex("sysdeps/linux/" + arch, "/.*", true);
        t -= "sysdeps/linux/.*context.*"_rr;
        t += "termios/.*"_rr;
        t += "unistd/.*"_rr;

        t.Public += config;
        t.Interface += "NOT_IN_libc"_def;

        for (auto &[p, sf] : t[FileRegex("sysdeps/linux/" + arch, "/crt.*\\.S", true)])
        {
            if (auto sf2 = sf->as<NativeSourceFile *>())
                sf2->skip_linking = true;
            sf->args.push_back("-fPIC");
        }

        auto add_crt1 = [&t, &arch](auto nt)
        {
            if (auto L = nt->Linker->as<sw::GNULinker*>())
            {
                std::decay_t<decltype(L->StartFiles())> f;
                f.push_back(t["sysdeps/linux/" + arch + "/crt1.S"].as<NativeSourceFile*>()->output);
                f.insert(f.end(), L->StartFiles().begin(), L->StartFiles().end());
                L->StartFiles() = f;
            }
        };

        auto setup_linker = [&t, &s, &arch](auto in)
        {
            if (auto L = in->as<sw::GNULinker*>())
            {
                L->nostdlib = true;
                if (auto sf = t["sysdeps/linux/" + arch + "/crti.S"].as<NativeSourceFile*>())
                    L->StartFiles().push_back(sf->output);
                //L->StartFiles().push_back("crtbegin.o");
                //L->EndFiles().push_back("crtend.o");
                if (auto sf = t["sysdeps/linux/" + arch + "/crtn.S"].as<NativeSourceFile*>())
                    L->EndFiles().push_back(sf->output);
            }
        };

        // rewrite linkers and crt for exes
        //setup_linker(t.Linker); // new targets
        for (auto &[pkg, tgts] : s.getChildren()) // and old targets (including this one)
        {
            for (auto &tgt : tgts)
            {
                if (auto nt = tgt->as<NativeExecutedTarget *>())
                {
                    if (tgt.get() == &t)
                        continue;
                    setup_linker(nt->Linker);
                    if (auto e = dynamic_cast<ExecutableTarget *>(nt))
                        add_crt1(e);
                }
            }
        }
        //


        /*
        // not working for some reason
        auto cc1 = std::static_pointer_cast<sw::CompilerBaseProgram>(t.findProgramByExtension(".c")->clone())->createCommand(t.getSolution().getContext());
        t.registerCommand(*cc1);
        cc1->push_back("-E");
        cc1->push_back("-dM");
        cc1->push_back("-");
        cc1->push_back("-I");
        cc1->push_back("d:/temp/9/usr/include/x86_64-linux-gnu");
        cc1->in.text = "#include <asm/unistd.h>";

        auto c = t.addCommand();
        c.c = std::static_pointer_cast<Command>(cc1);
        c << cmd::std_out("1.h");*/

        auto c =
        t.addCommand()
            << t.findProgramByExtension(".c")->file
            << "-E"
            << "-dM"
            << "-"
            << "-I"
            << "d:/temp/9/usr/include/x86_64-linux-gnu"
            |
        t.addCommand()
            << cmd::prog("org.sw.demo.gnu.sed.sed"_dep)
            << "-n" << "-r"
            << "-e" << R"(s/^[ ]*#define[ ]*(__ARM_NR_|__NR_)([A-Za-z0-9_]*) *(.*)/#undef \1\2\
#define \1\2 \3\
#define SYS_\2 \1\2/gp)"
            << cmd::std_out("bits/sysnum.h")
            ;
        c.c->in.text = "#include <asm/unistd.h>";
    }

    return;

    // locales
    {
        auto &gen_collate = uClibc_ng.addExecutable("gen.collate");
        {
            gen_collate.setSourceDirectory("extra/locale");
            gen_collate += "gen_collate.c";
        }

        auto &gen_wc8bit = uClibc_ng.addExecutable("gen.wc8bit");
        {
            gen_wc8bit += "extra/locale/gen_wc8bit.c";
            gen_wc8bit += "__UCLIBC_GEN_LOCALE"_def;
            gen_wc8bit += config;
            gen_wc8bit.patch("extra/locale/gen_wc8bit.c",
                "#include \"include/bits/uClibc_ctype.h\"",
                "#include \"bits/uClibc_ctype.h\"");
        }

        auto &gen_locale = uClibc_ng.addExecutable("gen.locale");
        {
            gen_locale.setSourceDirectory("extra/locale");
            gen_locale += "gen_locale.c";

            gen_locale += "charmaps/.*"_rr;
            std::set<String> maps;
            for (auto &[p, sf] : gen_locale["charmaps/.*"_rr])
            {
                maps.insert(p.filename().string());
            }
            gen_locale.writeFileOnce("codesets.txt");

            auto c = gen_locale.addCommand();
            c << cmd::prog(gen_wc8bit)
                << cmd::in("codesets.txt")
                << cmd::std_out("c8tables.h")
                ;
        }

        return;

        auto &locale = uClibc_ng.addLibrary("locale");
        {
            locale.AutoDetectOptions = false;
            locale.HeaderOnly = true;

            /*auto c = locale.addCommand();
            c << cmd::prog(gen_locale)
            << "-o"
            << cmd::out("locale_tables.h")
            << cmd::in("extra/locale/LOCALES")
            ;*/

            /*auto c = locale.addCommand();
            c << cmd::prog("org.sw.demo.gnu.grep.grep"_dep)
            << "COL_IDX_"
            << cmd::in("locale_tables.h");*/
        }
    }

    return;
}
