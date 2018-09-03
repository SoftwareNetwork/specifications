void build(Solution &s)
{
    auto &tiff = s.addTarget<LibraryTarget>("tiff", "4.0.9");
    tiff += RemoteFile("https://download.osgeo.org/libtiff/tiff-{v}.tar.gz");

    tiff.setChecks("tiff");

    tiff +=
        "libtiff/libtiff.def",
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
        "libtiff/tif_win32.c",
        "libtiff/tif_write.c",
        "libtiff/tif_zip.c",
        "libtiff/tiff.h",
        "libtiff/tiffconf.h.cmake.in",
        "libtiff/tiffio.h",
        "libtiff/tiffiop.h",
        "libtiff/tiffvers.h",
        "libtiff/uvcode.h";

    tiff.Public +=
        "libtiff"_id;

    tiff.Public += "JBIG_SUPPORT"_d;
    tiff.Public += "JPEG_SUPPORT"_d;
    tiff.Public += "LZMA_SUPPORT"_d;
    tiff.Public += "OJPEG_SUPPORT"_d;
    tiff.Public += "TIF_PLATFORM_CONSOLE"_d;
    tiff.Public += "ZIP_SUPPORT"_d;

    tiff.Public += "org.sw.demo.xz_utils.lzma-5"_dep;
    tiff.Public += "org.sw.demo.jpeg-9"_dep;
    tiff.Public += "org.sw.demo.uclouvain.openjpeg.openjp2-2"_dep;
    tiff.Public += "org.sw.demo.madler.zlib-1"_dep;
    tiff.Public += "org.sw.demo.mgk25.jbig.jbig-2"_dep;

    if (s.Settings.TargetOS.Type == OSType::Windows)
        tiff -= "libtiff/tif_unix.c";
    else
        tiff -= "libtiff/tif_win32.c";

    tiff.Variables["HAVE_IEEEFP"] = "1";
    tiff.Variables["HOST_FILLORDER"] = "FILLORDER_LSB2MSB";
    tiff.Variables["HOST_BIG_ENDIAN"] = tiff.Variables["WORDS_BIGENDIAN"];

    tiff.Variables["TIFF_INT8_T"] = "signed char";
    tiff.Variables["TIFF_UINT8_T"] = "unsigned char";

    tiff.Variables["TIFF_INT16_T"] = "signed short";
    tiff.Variables["TIFF_UINT16_T"] = "unsigned short";

    if (tiff.Variables["SIZEOF_SIGNED_INT"] == "4")
    {
        tiff.Variables["TIFF_INT32_T"] = "signed int";
        tiff.Variables["TIFF_INT32_FORMAT"] = "%d";
    }
    else if (tiff.Variables["SIZEOF_SIGNED_LONG"] == "4")
    {
        tiff.Variables["TIFF_INT32_T"] = "signed long";
        tiff.Variables["TIFF_INT32_FORMAT"] = "%ld";
    }

    if (tiff.Variables["SIZEOF_UNSIGNED_INT"] == "4")
    {
        tiff.Variables["TIFF_UINT32_T"] = "unsigned int";
        tiff.Variables["TIFF_UINT32_FORMAT"] = "%d";
    }
    else if (tiff.Variables["SIZEOF_UNSIGNED_LONG"] == "4")
    {
        tiff.Variables["TIFF_UINT32_T"] = "unsigned long";
        tiff.Variables["TIFF_UINT32_FORMAT"] = "%ld";
    }

    if (tiff.Variables["SIZEOF_SIGNED_LONG"] == "8")
    {
        tiff.Variables["TIFF_INT64_T"] = "signed long";
        tiff.Variables["TIFF_INT64_FORMAT"] = "%ld";
    }
    else if (tiff.Variables["SIZEOF_SIGNED_LONG_LONG"] == "8")
    {
        tiff.Variables["TIFF_INT64_T"] = "signed long long";
        /*
            if (MINGW)
                set(TIFF_INT64_FORMAT "%I64d")
            else()*/
        tiff.Variables["TIFF_INT64_FORMAT"] = "%lld";
    }

    if (tiff.Variables["SIZEOF_UNSIGNED_LONG"] == "8")
    {
        tiff.Variables["TIFF_UINT64_T"] = "unsigned long";
        tiff.Variables["TIFF_UINT64_FORMAT"] = "%lu";
    }
    else if (tiff.Variables["SIZEOF_UNSIGNED_LONG_LONG"] == "8")
    {
        tiff.Variables["TIFF_UINT64_T"] = "unsigned long long";
        /*
        if (MINGW)
        set(TIFF_UINT64_FORMAT "%I64u")
        else()*/
        tiff.Variables["TIFF_UINT64_FORMAT"] = "%llu";
    }

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
        tiff.Variables["TIFF_SIZE_T"] = "unsigned long";
        /*
        if (MINGW)
        set(TIFF_UINT64_FORMAT "%I64u")
        else()*/
        tiff.Variables["TIFF_SIZE_FORMAT"] = "%llu";
    }

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
        /*
        if (MINGW)
        set(TIFF_UINT64_FORMAT "%I64d")
        else()*/
        tiff.Variables["TIFF_SSIZE_FORMAT"] = "%lld";
    }

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

    if (s.Settings.TargetOS.Type != OSType::Windows)
    {
        tiff += "_FILE_OFFSET_BITS=64"_d;
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

    tiff.Variables["USE_WIN32_FILEIO"] = "0";

    tiff.configureFile("libtiff/tif_config.h.cmake.in", "tif_config.h");
    tiff.configureFile("libtiff/tiffconf.h.cmake.in", "tiffconf.h");

    tiff.pushBackToFileOnce("libtiff/libtiff.def", "\t_TIFFDataSize\n");
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
