void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &tiff = s.addTarget<LibraryTarget>("tiff", "4.0.8");
    tiff.Source = RemoteFile("ftp://download.osgeo.org/libtiff/tiff-4.0.8.tar.gz");

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

    tiff.Public += "pub.cppan2.demo.xz_utils.lzma-5"_dep;
    tiff.Public += "pub.cppan2.demo.jpeg-9"_dep;
    tiff.Public += "pub.cppan2.demo.openjpeg.openjp2-2"_dep;
    tiff.Public += "pub.cppan2.demo.madler.zlib-1"_dep;
    tiff.Public += "pub.cppan2.demo.mgk25.jbig.jbig-2"_dep;

    if (s.Settings.TargetOS.Type == OSType::Windows)
        tiff -= "libtiff/tif_unix.c";
    else
        tiff -= "libtiff/tif_win32.c";

    tiff.configureFile("libtiff/tif_config.h.cmake.in", "tif_config.h");
    tiff.configureFile("libtiff/tiffconf.h.cmake.in", "tiffconf.h");
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
