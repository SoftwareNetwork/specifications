void build(Solution &s)
{
    auto &t = s.addLibrary("OSGeo.gdal", "3.12.0");
    t += Git("https://github.com/OSGeo/gdal", "v{v}");
    {
        t += cpp17;
        t.setChecks("gdal");
        t.ApiName = "CPL_DLL";
        //t.setRootDirectory("gdal");

        t += "port/.*"_rr;
        //t -= "port/cpl_vsil_simple.cpp";
        //t -= "port/xmlreformat.cpp";
        t -= "port/cpl_odbc.cpp";
        t.Public += "port"_idir;

        t += "gcore/.*"_r;
        t += "gcore/mdreader/.*"_r;
        t -= "gcore/embedded_resources.c";
        t.Public += "gcore"_idir;

        t += "ogr/.*"_r;
        t += "ogr/ogrsf_frmts/.*"_r;
        t -= "ogr/generate_encoding_table.c";
        t -= "ogr/.*test.*"_r;
        t.Public += "ogr"_idir;

        t += "alg/.*"_r;
        t += "alg/marching_squares/.*"_r;
        t -= "alg/polygonize_polygonizer.cpp";
        t += "alg/marching_squares"_idir;
        t.Public += "alg"_idir;

        t += "apps/.*\\.h"_r;
        t += "apps/gdalalg_main.cpp";
        t += "apps/argparse/.*"_r;
        t += "apps/commonutils.*"_r;
        t += "apps/gdalargumentparser.*"_r;
        t += "apps/.*_lib.*.cpp"_r;
        t += "apps"_idir;

        t += "third_party/fast_float/.*"_rr;
        t += "third_party/libdivide/.*"_rr;

        t += "GDAL_COMPILATION"_def;
        t.Public += "CPL_INTERNAL="_def;
        t.Public += "CPL_UNSTABLE_API="_def;
        t.Public += "CPL_STDCALL="_def;

        if (t.getCompilerType() == CompilerType::MSVC) {
            t.CompileOptions.push_back("/bigobj");
        }

        auto add_dir = [&t](const path &dir)
        {
            t += FileRegex(dir, ".*", false);
            t.AllowEmptyRegexes = true;
            t -= FileRegex(dir, ".*test.*", false);
            t.AllowEmptyRegexes = false;
            t += IncludeDirectory(dir);
        };

        auto add_frmt = [&t, &add_dir](const path &dir)
        {
            add_dir(dir);
            t += Definition("FRMT_" + dir.filename().string());
        };

        t += "ogr/ogrsf_frmts"_idir;

        add_dir("ogr/ogrsf_frmts/generic");
        add_dir("ogr/ogrsf_frmts/geojson");
        add_dir("ogr/ogrsf_frmts/kml");
        //add_dir("ogr/ogrsf_frmts/mem");
        add_dir("ogr/ogrsf_frmts/mitab");

        // formats
        {
            t += "frmts/.*"_r;
            t += "frmts"_idir;
            add_frmt("frmts/derived");
            add_frmt("frmts/gtiff");
            add_frmt("frmts/gtiff/libgeotiff");
            add_frmt("frmts/gtiff/libtiff");
            add_frmt("frmts/hfa");
            add_frmt("frmts/mem");
            add_frmt("frmts/png");
            t -= "frmts/png/filter_sse2_intrinsics.c";
            t += "org.sw.demo.glennrp.png"_dep;

            add_frmt("frmts/vrt");
            t += "GDAL_VRT_ENABLE_MUPARSER"_def;
            t += "org.sw.demo.beltoforion.muparser"_dep;
            t += "GDAL_VRT_ENABLE_EXPRTK"_def;
            t += "org.sw.demo.ArashPartow.exprtk-master"_dep;

            /*{
            t += "frmts/.*"_r;
            t += "frmts"_idir;
            StringSet fmts;
            for (auto &d : fs::directory_iterator(t.SourceDir / "frmts"))
            {
            if (d.is_directory())
            fmts.insert(d.path().filename().string());
            }
            // skip
            for (auto &s : {
            "postgisraster", "tiledb",  "netcdf", "rdb",      "sdts",     "wcs",       "pcidsk",      "ecw",
            "jp2lura",       "hdf4",    "jp2kak", "epsilon",  "pcraster", "georaster", "mrsid_lidar", "aigrid",
            "msg",           "kea",     "bpg",    "rasdaman", "mrsid",    "rasdaman",  "msgn",        "sde",
            "gta",           "jpipkak", "dods",   "mbtiles",  "grass",
            "fits",
            "mrf",
            "dds",
            "jpegls",

            // cl ok, but not link
            "exr",
            "iso8211",
            "adrg",
            "bsb",
            "nitf",
            "hfa",
            })
            {
            fmts.erase(s);
            }
            for (auto &f : fmts)
            add_frmt("frmts/" + f);
            add_frmt("frmts/gtiff/libgeotiff");
            t += "org.sw.demo.tiff"_dep;
            t += "org.sw.demo.glennrp.png"_dep;
            t += "org.sw.demo.gif"_dep;
            t += "org.sw.demo.hdfgroup.hdf5.hdf5"_dep;
            //t += "org.sw.demo.openexr.IlmImf"_dep;
            t += "org.sw.demo.uclouvain.openjpeg.openjp2"_dep;
            t += "org.sw.demo.Esri.lerc"_dep; // ?
            t += "org.sw.demo.mdadams.jasper"_dep;
            t += "org.sw.demo.vbaderks.charls"_dep;
            }*/

            t -= "frmts/gtiff/generate_quant_table_md5sum.cpp";
        }

        t.configureFile("cmake/template/cpl_config.h.in", "cpl_config.h", ConfigureFlags::EnableUndefReplacements);
        t.configureFile("gcore/gdal_version.h.in", "gdal_version_full/gdal_version.h", ConfigureFlags::EnableUndefReplacements);
        t.configureFile("gcore/gdal_version.h.in", "gdal_version.h", ConfigureFlags::EnableUndefReplacements);

        t.Variables["SFCGAL_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
        t.Variables["SFCGAL_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
        t.Variables["SFCGAL_VERSION_PATCH"] = t.Variables["PACKAGE_VERSION_PATCH"];
        t.configureFile("ogr/ogr_sfcgal.h.in", "ogr_sfcgal.h");

        {
            t += "HAVE_LIBZ"_def;
            t += "org.sw.demo.madler.zlib"_dep;

            t += "HAVE_EXPAT"_def;
            t += "org.sw.demo.expat"_dep;

            t += "org.sw.demo.json_c"_dep;
            //t += "org.sw.demo.tiff"_dep;
            t += "org.sw.demo.Esri.lerc"_dep;

            t += "org.sw.demo.OSGeo.PROJ"_dep;

            t += "HAVE_OPENSSL_CRYPTO"_def;
            t += "HAVE_CURL"_def;
            t += "org.sw.demo.badger.curl.libcurl"_dep;

            t += "HAVE_LIBXML2"_def;
            t += "org.sw.demo.xmlsoft.libxml2"_dep;

            t += "org.sw.demo.jxl-0.8"_dep;

            //t += "org.sw.demo.p_ranav.argparse"_dep;
        }

        if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            t -= "port/vsipreload.cpp";

            t += "NOMINMAX"_def;
            t += "VSI_STAT64=_stat64"_def;
            t += "VSI_STAT64_T=__stat64"_def;

            if (t.getBuildSettings().Native.ConfigurationType != ConfigurationType::Debug)
                t += "comsuppw.lib"_slib;
            else
                t += "comsuppwd.lib"_slib;

            t += "odbc32.lib"_slib;
            t += "odbccp32.lib"_slib;
            t += "ole32.lib"_slib;
            t += "OleAut32.lib"_slib;
            t += "user32.lib"_slib;
            t += "ws2_32.lib"_slib;
            t += "wbemuuid.lib"_slib;

            t += "com.Microsoft.VisualStudio.VC.ATLMFC"_dep;
        }
        else
        {
            t += "dl"_slib;
        }
        if (t.getBuildSettings().TargetOS.isApple())
        {
            t -= "port/vsipreload.cpp";
        }
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("gdal");
    s.checkLibraryFunctionExists("dlopen", "dlopen.dl");
    s.checkLibraryFunctionExists("nanosleep", "nanosleep.rt");
    s.checkLibraryFunctionExists("sin", "sin.m");
    s.checkIncludeExists("stdlib.h");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkIncludeExists("assert.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("dbmalloc.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("values.h");
    s.checkIncludeExists("float.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("direct.h");
    s.checkIncludeExists("linux/userfaultfd.h");
    s.checkTypeSize("long long");
    s.checkTypeSize("int");
    s.checkTypeSize("unsigned");
    s.checkTypeSize("unsigned long");
    s.checkTypeSize("long");
    s.checkTypeSize("void*");
    s.checkTypeSize("void *");
    s.checkTypeSize("int8");
    s.checkTypeSize("int16");
    s.checkTypeSize("int32");
    s.checkTypeSize("uintptr_t");
    //s.checkTypeSize("off_t");
    //s.checkTypeSize("off64_t");
    //s.checkTypeSize("struct stat64");
    //s.checkTypeSize("stat64");
    s.checkFunctionExists("vprintf");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
    s.checkFunctionExists("vsnprintf");
    s.checkFunctionExists("atoll");
    s.checkFunctionExists("strtoull");
    s.checkFunctionExists("strtof");
    s.checkFunctionExists("getcwd");
    s.checkDeclarationExists("strtof");
    s.checkFunctionExists("readlink");
    s.checkFunctionExists("lstat");
    s.checkFunctionExists("posix_spawnp");
    s.checkFunctionExists("posix_memalign");
    s.checkFunctionExists("vfork");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("sigaction");
    s.checkFunctionExists("statvfs");
    s.checkFunctionExists("statvfs64");
    s.checkFunctionExists("getrlimit");

    return;

    s.checkLibraryFunctionExists("pthread_create", "pthread_create.pthread");
    s.checkLibraryFunctionExists("pthread_create", "pthread_create.c");
    s.checkLibraryFunctionExists("deflateInit_", "deflateInit_.z");
    s.checkLibraryFunctionExists("inflateCopy", "inflateCopy.z");
    s.checkLibraryFunctionExists("deflateInit_", "deflateInit_.z");
    s.checkLibraryFunctionExists("inflateCopy", "inflateCopy.z");
    s.checkLibraryFunctionExists("proj_create_from_wkt", "proj_create_from_wkt.proj");
    s.checkLibraryFunctionExists("internal_proj_create_from_wkt", "internal_proj_create_from_wkt.proj");
    s.checkLibraryFunctionExists("internal_proj_create_from_wkt", "internal_proj_create_from_wkt.internalproj");
    s.checkIncludeExists("proj.h");
    s.checkLibraryFunctionExists("proj_create_from_wkt", "proj_create_from_wkt.proj");
    s.checkLibraryFunctionExists("proj_create_from_wkt", "proj_create_from_wkt.proj");
    s.checkLibraryFunctionExists("proj_create_from_wkt", "proj_create_from_wkt.proj");
    s.checkLibraryFunctionExists("internal_proj_create_from_wkt", "internal_proj_create_from_wkt.proj");
    s.checkLibraryFunctionExists("internal_proj_create_from_wkt", "internal_proj_create_from_wkt.proj");
    s.checkLibraryFunctionExists("internal_proj_create_from_wkt", "internal_proj_create_from_wkt.internalproj");
    s.checkLibraryFunctionExists("internal_proj_create_from_wkt", "internal_proj_create_from_wkt.internalproj");
    s.checkLibraryFunctionExists("lzma_code", "lzma_code.lzma");
    s.checkIncludeExists("lzma.h");
    s.checkLibraryFunctionExists("ZSTD_decompressStream", "ZSTD_decompressStream.zstd");
    s.checkLibraryFunctionExists("ZSTD_decompressStream", "ZSTD_decompressStream.zstd");
    s.checkLibraryFunctionExists("PQconnectdb", "PQconnectdb.pq");
    s.checkLibraryFunctionExists("G_is_initialized", "G_is_initialized.grass_gis");
    s.checkLibraryFunctionExists("G_asprintf", "G_asprintf.grass_gis");
    s.checkLibraryFunctionExists("G_gisinit_2", "G_gisinit_2.grass5");
    s.checkLibraryFunctionExists("G_gisinit_2", "G_gisinit_2.grass5");
    s.checkLibraryFunctionExists("ffopen", "ffopen.cfitsio");
    s.checkLibraryFunctionExists("Mopen", "Mopen.csf");
    s.checkIncludeExists("csf.h");
    s.checkLibraryFunctionExists("png_set_IHDR", "png_set_IHDR.png");
    s.checkIncludeExists("png.h");
    s.checkLibraryFunctionExists("gta_version", "gta_version.gta");
    s.checkLibraryFunctionExists("TIFFScanlineSize64", "TIFFScanlineSize64.tiff");
    s.checkLibraryFunctionExists("_TIFFsetDoubleArray", "_TIFFsetDoubleArray.tiff");
    s.checkLibraryFunctionExists("TIFFScanlineSize64", "TIFFScanlineSize64.tiff");
    s.checkLibraryFunctionExists("GTIFAttachPROJContext", "GTIFAttachPROJContext.geotiff");
    s.checkIncludeExists("geotiff.h");
    s.checkLibraryFunctionExists("GTIFAttachPROJContext", "GTIFAttachPROJContext.geotiff");
    s.checkLibraryFunctionExists("GTIFAttachPROJContext", "GTIFAttachPROJContext.geotiff");
    s.checkLibraryFunctionExists("jpeg_read_scanlines", "jpeg_read_scanlines.jpeg");
    s.checkIncludeExists("jpeglib.h");
    s.checkIncludeExists("CharLS/interface.h");
    s.checkLibraryFunctionExists("JpegLsDecode", "JpegLsDecode.CharLS");
    s.checkIncludeExists("CharLS/charls.h");
    s.checkLibraryFunctionExists("JpegLsDecode", "JpegLsDecode.CharLS");
    s.checkIncludeExists("charls/charls.h");
    s.checkLibraryFunctionExists("JpegLsDecode", "JpegLsDecode.charls");
    s.checkLibraryFunctionExists("DGifOpenFileName", "DGifOpenFileName.gif");
    s.checkIncludeExists("gif_lib.h");
    s.checkIncludeExists("ecs.h");
    s.checkLibraryFunctionExists("cln_GetLayerCapabilities", "cln_GetLayerCapabilities.ogdi");
    s.checkLibraryFunctionExists("cln_GetLayerCapabilities", "cln_GetLayerCapabilities.ogdi31");
    s.checkLibraryFunctionExists("cln_GetLayerCapabilities", "cln_GetLayerCapabilities.ogdi");
    s.checkLibraryFunctionExists("cln_GetLayerCapabilities", "cln_GetLayerCapabilities.ogdi31");
    s.checkLibraryFunctionExists("SDreaddata", "SDreaddata.mfhdfalt");
    s.checkLibraryFunctionExists("SDreaddata", "SDreaddata.mfhdf");
    s.checkLibraryFunctionExists("SDreaddata", "SDreaddata.hdf4");
    s.checkLibraryFunctionExists("SDreaddata", "SDreaddata.mfhdf");
    s.checkLibraryFunctionExists("SDreaddata", "SDreaddata.mfhdf");
    s.checkLibraryFunctionExists("SDget_maxopenfiles", "SDget_maxopenfiles.mfhdfalt");
    s.checkLibraryFunctionExists("SDget_maxopenfiles", "SDget_maxopenfiles.mfhdf");
    s.checkLibraryFunctionExists("H5Fopen", "H5Fopen.hdf5");
    s.checkLibraryFunctionExists("H5Fopen", "H5Fopen.hdf5");
    s.checkLibraryFunctionExists("H5Fopen", "H5Fopen.hdf5");
    s.checkLibraryFunctionExists("nc_open", "nc_open.netcdf");
    s.checkLibraryFunctionExists("nc_open", "nc_open.netcdf");
    s.checkLibraryFunctionExists("nc_open", "nc_open.netcdf");
    s.checkIncludeExists("$NETCDF_INCLUDEDIR/netcdf_mem.h");
    s.checkLibraryFunctionExists("jpc_decode", "jpc_decode.jasper");
    s.checkLibraryFunctionExists("jp2_decode", "jp2_decode.jasper");
    s.checkLibraryFunctionExists("pgx_decode", "pgx_decode.jasper");
    s.checkLibraryFunctionExists("jp2_encode_uuid", "jp2_encode_uuid.jasper");
    s.checkLibraryFunctionExists("opj_setup_decoder", "opj_setup_decoder.openjp2");
    s.checkLibraryFunctionExists("pj_datums", "pj_datums.FileGDBAPI");
    s.checkLibraryFunctionExists("NCScbmOpenFileView", "NCScbmOpenFileView.NCSEcw");
    s.checkLibraryFunctionExists("NCScbmOpenFileView", "NCScbmOpenFileView.ecwj2");
    s.checkLibraryFunctionExists("JP2_Decompress_SetLicense", "JP2_Decompress_SetLicense._lwf_jp2");
    s.checkLibraryFunctionExists("SQLConnect", "SQLConnect.odbc");
    s.checkLibraryFunctionExists("SQLInstallDriverEx", "SQLInstallDriverEx.odbcinst");
    s.checkLibraryFunctionExists("main", "main.odbc32");
    s.checkLibraryFunctionExists("SQLInstallDriverEx", "SQLInstallDriverEx.odbccp32");
    s.checkIncludeExists("sql.h");
    s.checkLibraryFunctionExists("SQLConnect", "SQLConnect.odbc");
    s.checkLibraryFunctionExists("main", "main.odbc32");
    s.checkLibraryFunctionExists("curl_global_init", "curl_global_init.curl");
    s.checkLibraryFunctionExists("xmlParseDoc", "xmlParseDoc.xml2");
    s.checkIncludeExists("sqlite3.h");
    s.checkLibraryFunctionExists("spatialite_init", "spatialite_init.spatialite");
    s.checkLibraryFunctionExists("spatialite_init", "spatialite_init.spatialite");
    s.checkLibraryFunctionExists("spatialite_target_cpu", "spatialite_target_cpu.spatialite");
    s.checkLibraryFunctionExists("sqlite3_column_table_name", "sqlite3_column_table_name.sqlite3");
    s.checkIncludeExists("rasterlite2/rasterlite2.h");
    s.checkLibraryFunctionExists("rl2_load_raw_tiles_into_dbms", "rl2_load_raw_tiles_into_dbms.rasterlite2");
    s.checkLibraryFunctionExists("rl2_load_raw_tiles_into_dbms", "rl2_load_raw_tiles_into_dbms.rasterlite2");
    s.checkIncludeExists("pcre.h");
    s.checkLibraryFunctionExists("pcre_compile", "pcre_compile.pcre");
    s.checkLibraryFunctionExists("ifx_srvinfo", "ifx_srvinfo.ifsql");
    s.checkLibraryFunctionExists("SE_connection_create", "SE_connection_create.sde$SDE_VERSION");
    s.checkLibraryFunctionExists("main", "main.pthread");
    s.checkLibraryFunctionExists("main", "main.socket");
    s.checkLibraryFunctionExists("main", "main.dl");
    s.checkLibraryFunctionExists("main", "main.pthread");
    s.checkLibraryFunctionExists("main", "main.socket");
    s.checkLibraryFunctionExists("main", "main.dl");
    s.checkLibraryFunctionExists("eps_read_block_header", "eps_read_block_header.epsilon");
    s.checkLibraryFunctionExists("eps_read_block_header", "eps_read_block_header.epsilon");
    s.checkLibraryFunctionExists("WebPDecodeRGB", "WebPDecodeRGB.webp");
    s.checkLibraryFunctionExists("WebPDecodeRGB", "WebPDecodeRGB.webp");
    s.checkIncludeExists("qhull/libqhull.h");
    s.checkLibraryFunctionExists("qh_new_qhull", "qh_new_qhull.qhull");
    s.checkIncludeExists("libqhull/libqhull.h");
    s.checkLibraryFunctionExists("qh_new_qhull", "qh_new_qhull.qhull");
    s.checkIncludeExists("CL/opencl.h");
    s.checkLibraryFunctionExists("clGetPlatformIDs", "clGetPlatformIDs.OpenCL");
    s.checkIncludeExists("freexl.h");
    s.checkLibraryFunctionExists("freexl_open", "freexl_open.freexl");
    s.checkLibraryFunctionExists("freexl_open", "freexl_open.freexl");
    s.checkLibraryFunctionExists("freexl_open", "freexl_open.freexl");
    s.checkLibraryFunctionExists("freexl_open", "freexl_open.freexl");
    s.checkLibraryFunctionExists("freexl_open", "freexl_open.freexl");
    s.checkLibraryFunctionExists("freexl_open", "freexl_open.freexl");
    s.checkLibraryFunctionExists("freexl_open", "freexl_open.freexl");
    s.checkLibraryFunctionExists("freexl_open", "freexl_open.freexl");
    s.checkLibraryFunctionExists("freexl_open", "freexl_open.freexl");
    s.checkLibraryFunctionExists("freexl_open", "freexl_open.freexl");
    s.checkLibraryFunctionExists("json_object_set_serializer", "json_object_set_serializer.json-c");
    s.checkLibraryFunctionExists("json_object_set_serializer", "json_object_set_serializer.json-c");
    s.checkLibraryFunctionExists("hdfsConnect", "hdfsConnect.hdfs");
    s.checkLibraryFunctionExists("tiledb_coords", "tiledb_coords.tiledb");
    s.checkLibraryFunctionExists("tiledb_coords", "tiledb_coords.tiledb");
    s.checkLibraryFunctionExists("JNI_CreateJavaVM", "JNI_CreateJavaVM.jvm");
    s.checkLibraryFunctionExists("JNI_CreateJavaVM", "JNI_CreateJavaVM.jvm");
    s.checkLibraryFunctionExists("JNI_CreateJavaVM", "JNI_CreateJavaVM.jvm");
    s.checkLibraryFunctionExists("main", "main.raslib");
    s.checkLibraryFunctionExists("rdb_library_name", "rdb_library_name.rdb");
    s.checkLibraryFunctionExists("rdb_library_name", "rdb_library_name.rdb");
    s.checkLibraryFunctionExists("EVP_sha256", "EVP_sha256.crypto");
    s.checkIncludeExists("openssl/bio.h");
    s.checkLibraryFunctionExists("ImfTiledInputReadTile", "ImfTiledInputReadTile.IlmImf");
}
