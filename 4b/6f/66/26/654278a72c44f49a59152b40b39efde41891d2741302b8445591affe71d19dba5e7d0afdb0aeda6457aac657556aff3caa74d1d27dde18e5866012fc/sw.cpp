void build(Solution &s)
{
    auto &openjp2 = s.addTarget<LibraryTarget>("uclouvain.openjpeg.openjp2", "2.4.0");
    openjp2 += Git("https://github.com/uclouvain/openjpeg", "v{v}");

    openjp2.setChecks("openjp2");

    openjp2 +=
        "src/lib/openjp2/.*\\.h"_rr,
        "src/lib/openjp2/bio.c",
        "src/lib/openjp2/cio.c",
        "src/lib/openjp2/dwt.c",
        "src/lib/openjp2/event.c",
        "src/lib/openjp2/function_list.c",
        "src/lib/openjp2/image.c",
        "src/lib/openjp2/invert.c",
        "src/lib/openjp2/j2k.c",
        "src/lib/openjp2/jp2.c",
        "src/lib/openjp2/mct.c",
        "src/lib/openjp2/mqc.c",
        "src/lib/openjp2/openjpeg.c",
        "src/lib/openjp2/opj_clock.c",
        "src/lib/openjp2/opj_malloc.c",
        "src/lib/openjp2/pi.c",
        "src/lib/openjp2/sparse_array.*"_rr,
        "src/lib/openjp2/t1.c",
        "src/lib/openjp2/t2.c",
        "src/lib/openjp2/tcd.c",
        "src/lib/openjp2/tgt.c",
        "src/lib/openjp2/thread.*"_rr;

    openjp2.Public +=
        "src/lib/openjp2"_id;

    openjp2.Private += sw::Shared, "OPJ_EXPORTS"_d;
    openjp2.Public += sw::Static, "OPJ_STATIC"_d;

    openjp2.writeFileOnce("opj_config.h.in", R"(
//#ifdef HAVE_STDINT_H
#define OPJ_HAVE_STDINT_H 1
//#endif
#define OPJ_VERSION_MAJOR @PACKAGE_VERSION_MAJOR@
#define OPJ_VERSION_MINOR @PACKAGE_VERSION_MINOR@
#define OPJ_VERSION_BUILD @PACKAGE_VERSION_PATCH@
)");

    openjp2.writeFileOnce("opj_config_private.h.in", R"(
//#ifdef HAVE_INTTYPES_H
#define OPJ_HAVE_INTTYPES_H 1
//#endif
#define OPJ_PACKAGE_VERSION "@PACKAGE_VERSION@"

#cmakedefine _LARGEFILE_SOURCE
#cmakedefine _LARGE_FILES
#cmakedefine _FILE_OFFSET_BITS @_FILE_OFFSET_BITS@
#cmakedefine OPJ_HAVE_FSEEKO @HAVE_FSEEKO@

/* find whether or not have <malloc.h> */
#cmakedefine OPJ_HAVE_MALLOC_H @HAVE_MALLOC_H@
/* check if function `aligned_alloc` exists */
#cmakedefine OPJ_HAVE_ALIGNED_ALLOC @HAVE_ALIGNED_ALLOC@
/* check if function `_aligned_malloc` exists */
#cmakedefine OPJ_HAVE__ALIGNED_MALLOC @HAVE__ALIGNED_MALLOC@
/* check if function `memalign` exists */
#cmakedefine OPJ_HAVE_MEMALIGN @HAVE_MEMALIGN@
/* check if function `posix_memalign` exists */
#cmakedefine OPJ_HAVE_POSIX_MEMALIGN @HAVE_POSIX_MEMALIGN@

#if !defined(_POSIX_C_SOURCE)
#if defined(OPJ_HAVE_FSEEKO) || defined(OPJ_HAVE_POSIX_MEMALIGN)
/* Get declarations of fseeko, ftello, posix_memalign. */
#define _POSIX_C_SOURCE 200112L
#endif
#endif

    #if @WORDS_BIGENDIAN@
    #define OPJ_BIG_ENDIAN @WORDS_BIGENDIAN@
    #endif
)");

    openjp2.configureFile("opj_config.h.in", "opj_config.h");
    openjp2.configureFile("opj_config_private.h.in", "opj_config_private.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("openjp2");
    s.checkFunctionExists("aligned_alloc");
    s.checkFunctionExists("fseeko");
    s.checkFunctionExists("memalign");
    s.checkFunctionExists("posix_memalign");
    s.checkFunctionExists("_aligned_malloc");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("malloc.h");
    s.checkIncludeExists("stdint.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
}
