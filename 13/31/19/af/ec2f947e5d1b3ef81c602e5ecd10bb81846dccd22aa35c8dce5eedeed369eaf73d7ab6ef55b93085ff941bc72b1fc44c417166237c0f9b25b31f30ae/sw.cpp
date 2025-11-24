void build(Solution &s)
{
    auto &tiff = s.addTarget<LibraryTarget>("tiff", "4.7.1");
    tiff += Git("https://gitlab.com/libtiff/libtiff", "v{v}");

    tiff.setChecks("tiff");

    tiff += gnu99;
    //if (tiff.getBuildSettings().TargetOS.Type != OSType::Windows)
    tiff.ExportAllSymbols = true;

    tiff +=
        //"libtiff/libtiff.def",
        "libtiff/t4.h",
        "libtiff/tif_aux.c",
        "libtiff/tif_close.c",
        "libtiff/tif_codec.c",
        "libtiff/tif_color.c",
        "libtiff/tif_compress.c",
        "libtiff/tif_config.h.cmake.in",
        "libtiff/tif_dir.c",
        "libtiff/tif_dir.h",
        "libtiff/tif_dirinfo.c",
        "libtiff/tif_dirread.c",
        "libtiff/tif_dirwrite.c",
        "libtiff/tif_dumpmode.c",
        "libtiff/tif_error.c",
        "libtiff/tif_extension.c",
        "libtiff/tif_fax3.c",
        "libtiff/tif_fax3.h",
        "libtiff/tif_fax3sm.c",
        "libtiff/tif_flush.c",
        "libtiff/tif_hash_set.c",
        "libtiff/tif_getimage.c",
        "libtiff/tif_jbig.c",
        "libtiff/tif_jpeg.c",
        "libtiff/tif_jpeg_12.c",
        "libtiff/tif_luv.c",
        "libtiff/tif_lzma.c",
        "libtiff/tif_lzw.c",
        "libtiff/tif_next.c",
        "libtiff/tif_ojpeg.c",
        "libtiff/tif_open.c",
        "libtiff/tif_packbits.c",
        "libtiff/tif_pixarlog.c",
        "libtiff/tif_predict.c",
        "libtiff/tif_predict.h",
        "libtiff/tif_print.c",
        "libtiff/tif_read.c",
        "libtiff/tif_strip.c",
        "libtiff/tif_swab.c",
        "libtiff/tif_thunder.c",
        "libtiff/tif_tile.c",
        "libtiff/tif_unix.c",
        "libtiff/tif_version.c",
        "libtiff/tif_warning.c",
        "libtiff/tif_webp.c",
        "libtiff/tif_win32.c",
        "libtiff/tif_write.c",
        "libtiff/tif_zip.c",
        "libtiff/tif_zstd.c",

        "libtiff/tiff.h",
        "libtiff/tiffconf.h.cmake.in",
        "libtiff/tiffvers.h.cmake.in",
        "libtiff/tif_hash_set.h",
        "libtiff/tiffio.h",
        "libtiff/tiffiop.h",
        //"libtiff/tiffvers.h",
        "libtiff/uvcode.h"
        ;

    tiff.Public +=
        "libtiff"_id;

    tiff.Public += "org.sw.demo.xz_utils.lzma"_dep;
    tiff.Public += "org.sw.demo.jpeg"_dep;
    tiff.Public += "org.sw.demo.uclouvain.openjpeg.openjp2"_dep;
    tiff.Public += "org.sw.demo.madler.zlib"_dep;
    tiff.Public += "org.sw.demo.mgk25.jbig.jbig"_dep;
    tiff.Public += "org.sw.demo.facebook.zstd.zstd"_dep;
    tiff.Public += "org.sw.demo.webmproject.webp"_dep;

    if (tiff.getBuildSettings().TargetOS.Type == OSType::Windows)
        tiff -= "libtiff/tif_unix.c";
    else
        tiff -= "libtiff/tif_win32.c";

    tiff += "JBIG_SUPPORT=1"_v;
    tiff += "JPEG_SUPPORT=1"_v;
    tiff += "LZMA_SUPPORT=1"_v;
    tiff += "OJPEG_SUPPORT=1"_v;
    tiff += "TIF_PLATFORM_CONSOLE=1"_v;
    tiff += "WEBP_SUPPORT=1"_v;
    tiff += "ZIP_SUPPORT=1"_v;
    tiff += "ZSTD_SUPPORT=1"_v;

    tiff.Variables["TIFF_MAX_DIR_COUNT"] = 1048576;
    tiff.Variables["HAVE_IEEEFP"] = 1;
    tiff.Variables["HOST_FILLORDER"] = "FILLORDER_LSB2MSB";
    tiff.Variables["HOST_BIG_ENDIAN"] = tiff.Variables["WORDS_BIGENDIAN"];

    tiff.Variables["TIFF_INT8_T"] = "signed char";
    tiff.Variables["TIFF_UINT8_T"] = "unsigned char";

    tiff.Variables["TIFF_INT16_T"] = "signed short";
    tiff.Variables["TIFF_UINT16_T"] = "unsigned short";

    if (tiff.Variables["SIZEOF_SIGNED_INT"] == 4)
    {
        tiff.Variables["TIFF_INT32_T"] = "signed int";
        tiff.Variables["TIFF_INT32_FORMAT"] = "%d";
    }
    else if (tiff.Variables["SIZEOF_SIGNED_LONG"] == 4)
    {
        tiff.Variables["TIFF_INT32_T"] = "signed long";
        tiff.Variables["TIFF_INT32_FORMAT"] = "%ld";
    }
    else if (!tiff.DryRun)
        throw SW_RUNTIME_ERROR("no type: " + tiff.Variables["SIZEOF_SIGNED_INT"].toString() + ", " + tiff.Variables["SIZEOF_SIGNED_LONG"].toString());

    if (tiff.Variables["SIZEOF_UNSIGNED_INT"] == 4)
    {
        tiff.Variables["TIFF_UINT32_T"] = "unsigned int";
        tiff.Variables["TIFF_UINT32_FORMAT"] = "%d";
    }
    else if (tiff.Variables["SIZEOF_UNSIGNED_LONG"] == 4)
    {
        tiff.Variables["TIFF_UINT32_T"] = "unsigned long";
        tiff.Variables["TIFF_UINT32_FORMAT"] = "%ld";
    }
    else if (!tiff.DryRun)
        throw SW_RUNTIME_ERROR("no type");

    if (tiff.Variables["SIZEOF_SIGNED_LONG"] == 8)
    {
        tiff.Variables["TIFF_INT64_T"] = "signed long";
        tiff.Variables["TIFF_INT64_FORMAT"] = "%ld";
    }
    else if (tiff.Variables["SIZEOF_SIGNED_LONG_LONG"] == 8)
    {
        tiff.Variables["TIFF_INT64_T"] = "signed long long";
        tiff.Variables["TIFF_INT64_FORMAT"] = "%lld";
        /*
        if (MINGW)
        set(TIFF_INT64_FORMAT "%I64d")
        else()*/
    }
    else if (!tiff.DryRun)
        throw SW_RUNTIME_ERROR("no type");

    if (tiff.Variables["SIZEOF_UNSIGNED_LONG"] == 8)
    {
        tiff.Variables["TIFF_UINT64_T"] = "unsigned long";
        tiff.Variables["TIFF_UINT64_FORMAT"] = "%lu";
    }
    else if (tiff.Variables["SIZEOF_UNSIGNED_LONG_LONG"] == 8)
    {
        tiff.Variables["TIFF_UINT64_T"] = "unsigned long long";
        tiff.Variables["TIFF_UINT64_FORMAT"] = "%llu";
        /*
        if (MINGW)
        set(TIFF_UINT64_FORMAT "%I64u")
        else()*/
    }
    else if (!tiff.DryRun)
        throw SW_RUNTIME_ERROR("no type");

    if (tiff.Variables["SIZEOF_UNSIGNED_INT"] == tiff.Variables["SIZEOF_SIZE_T"])
    {
        tiff.Variables["TIFF_SIZE_T"] = "unsigned int";
        tiff.Variables["TIFF_SIZE_FORMAT"] = "%u";
    }
    else if (tiff.Variables["SIZEOF_UNSIGNED_LONG"] == tiff.Variables["SIZEOF_SIZE_T"])
    {
        tiff.Variables["TIFF_SIZE_T"] = "unsigned long";
        tiff.Variables["TIFF_SIZE_FORMAT"] = "%lu";
    }
    else if (tiff.Variables["SIZEOF_UNSIGNED_LONG_LONG"] == tiff.Variables["SIZEOF_SIZE_T"])
    {
        tiff.Variables["TIFF_SIZE_T"] = "unsigned long long";
        tiff.Variables["TIFF_SIZE_FORMAT"] = "%llu";
        /*
        if (MINGW)
        set(TIFF_UINT64_FORMAT "%I64u")
        else()*/
    }
    else if (!tiff.DryRun)
        throw SW_RUNTIME_ERROR("no type");

    if (tiff.Variables["SIZEOF_SIGNED_INT"] == tiff.Variables["SIZEOF_UNSIGNED_CHAR_P"])
    {
        tiff.Variables["TIFF_SSIZE_T"] = "signed int";
        tiff.Variables["TIFF_SSIZE_FORMAT"] = "%d";
    }
    else if (tiff.Variables["SIZEOF_SIGNED_LONG"] == tiff.Variables["SIZEOF_UNSIGNED_CHAR_P"])
    {
        tiff.Variables["TIFF_SSIZE_T"] = "signed long";
        tiff.Variables["TIFF_SSIZE_FORMAT"] = "%ld";
    }
    else if (tiff.Variables["SIZEOF_SIGNED_LONG_LONG"] == tiff.Variables["SIZEOF_UNSIGNED_CHAR_P"])
    {
        tiff.Variables["TIFF_SSIZE_T"] = "signed long long";
        tiff.Variables["TIFF_SSIZE_FORMAT"] = "%lld";
        /*
        if (MINGW)
        set(TIFF_UINT64_FORMAT "%I64d")
        else()*/
    }
    else if (!tiff.DryRun)
        throw SW_RUNTIME_ERROR("no type");

    if (tiff.Variables.find("SIZEOF_PTRDIFF_T") == tiff.Variables.end())
    {
        tiff.Variables["TIFF_PTRDIFF_T"] = tiff.Variables["TIFF_SSIZE_T"];
        tiff.Variables["TIFF_PTRDIFF_FORMAT"] = tiff.Variables["SSIZE_FORMAT"];
    }
    else
    {
        tiff.Variables["TIFF_PTRDIFF_T"] = "ptrdiff_t";
        tiff.Variables["TIFF_PTRDIFF_FORMAT"] = "%ld";
    }

    if (tiff.getBuildSettings().TargetOS.Type != OSType::Windows)
    {
        //tiff += "_FILE_OFFSET_BITS=64"_d;
        tiff.Variables["FILE_OFFSET_BITS"] = "64";
    }

    tiff.Variables["CCITT_SUPPORT"] = "1";
    tiff.Variables["PACKBITS_SUPPORT"] = "1";
    tiff.Variables["LZW_SUPPORT"] = "1";
    tiff.Variables["THUNDER_SUPPORT"] = "1";
    tiff.Variables["NEXT_SUPPORT"] = "1";
    tiff.Variables["LOGLUV_SUPPORT"] = "1";
    tiff.Variables["MDI_SUPPORT"] = "1";

    tiff.Variables["TIFF_DEFAULT_STRIP_SIZE"] = "8192";
    tiff.Variables["STRIPCHOP_DEFAULT"] = "1";
    tiff.Variables["STRIP_SIZE_DEFAULT"] = tiff.Variables["TIFF_DEFAULT_STRIP_SIZE"];
    tiff.Variables["SUBIFD_SUPPORT"] = "1";
    tiff.Variables["DEFAULT_EXTRASAMPLE_AS_ALPHA"] = "1";
    tiff.Variables["CHECK_JPEG_YCBCR_SUBSAMPLING"] = "1";

    tiff.Variables["LIBTIFF_RELEASE_DATE"] = "0"; // 20230908
    tiff.Variables["LIBTIFF_MAJOR_VERSION"] = tiff.Variables["PACKAGE_VERSION_MAJOR"];
    tiff.Variables["LIBTIFF_MINOR_VERSION"] = tiff.Variables["PACKAGE_VERSION_MINOR"];
    tiff.Variables["LIBTIFF_MICRO_VERSION"] = tiff.Variables["PACKAGE_VERSION_PATCH"];
    tiff.Variables["LIBTIFF_VERSION"] = tiff.Variables["PACKAGE_VERSION"];

    tiff.Variables["USE_WIN32_FILEIO"] = "0";

    tiff.configureFile("libtiff/tif_config.h.cmake.in", "tif_config.h");
    tiff.configureFile("libtiff/tiffconf.h.cmake.in", "tiffconf.h");
    tiff.configureFile("libtiff/tiffvers.h.cmake.in", "tiffvers.h");

    tiff.pushBackToFileOnce("libtiff/libtiff.def", "\t_TIFFDataSize\n");
    tiff -= sw::Static, "libtiff/libtiff.def";
}

void check(Checker &c)
{
    auto &s = c.addSet("tiff");
    s.checkFunctionExists("floor");
    s.checkFunctionExists("getopt");
    s.checkFunctionExists("isascii");
    s.checkFunctionExists("lfind");
    s.checkFunctionExists("memmove");
    s.checkFunctionExists("memset");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("pow");
    s.checkFunctionExists("setmode");
    s.checkFunctionExists("sqrt");
    s.checkFunctionExists("strcasecmp");
    s.checkFunctionExists("strchr");
    s.checkFunctionExists("strrchr");
    s.checkFunctionExists("strstr");
    s.checkFunctionExists("strtol");
    s.checkFunctionExists("strtoull");
    s.checkIncludeExists("assert.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("malloc.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("search.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("int16");
    s.checkTypeSize("int32");
    s.checkTypeSize("int8");
    s.checkTypeSize("ptrdiff_t");
    s.checkTypeSize("signed int");
    s.checkTypeSize("signed long");
    s.checkTypeSize("signed long long");
    s.checkTypeSize("signed short");
    s.checkTypeSize("size_t");
    s.checkTypeSize("unsigned char *");
    s.checkTypeSize("unsigned int");
    s.checkTypeSize("unsigned long");
    s.checkTypeSize("unsigned long long");
    s.checkTypeSize("unsigned short");
    s.checkTypeSize("void *");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
}
