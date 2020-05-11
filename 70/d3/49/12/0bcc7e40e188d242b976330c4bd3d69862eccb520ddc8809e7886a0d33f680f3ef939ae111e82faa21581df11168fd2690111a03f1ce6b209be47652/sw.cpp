void build(Solution &s)
{
    auto &gdcm = s.addProject("malaterre.GDCM", "3.0.5");
    gdcm += Git("https://github.com/malaterre/GDCM", "v{v}");

    auto &rle = gdcm.addStaticLibrary("rle");
    {
        rle += "Utilities/gdcmrle/.*\\.cxx"_rr;
        rle += "Utilities/gdcmrle/.*\\.h"_rr;
        rle -= "Utilities/gdcmrle/main.cxx";
        rle.Public += "Utilities"_id;
    }

    auto &charls = gdcm.addLibrary("util.charls");
    {
        charls += "Utilities/gdcmcharls/.*"_rr;
        charls += "Utilities/gdcm_charls.h";
        charls.Public += "Utilities"_id;
        charls += sw::Shared, "CHARLS_DLL_BUILD"_def;
        if (charls.getBuildSettings().TargetOS.Type == OSType::Windows ||
            charls.getBuildSettings().TargetOS.Type == OSType::Mingw)
        {
            charls.Public += "WIN32"_def; // stupid necessary defs
        }
        charls.Public += sw::Shared, "CHARLS_DLL"_def; // stupid necessary defs
        charls.Public += sw::Static, "CHARLS_STATIC"_def;
    }

    auto &socketxx = gdcm.addTarget<LibraryTarget>("socketxx");
    {
        socketxx.setChecks("socketxx");

        socketxx +=
            "Utilities/socketxx/.*\\.cpp"_rr,
            "Utilities/socketxx/.*\\.h"_rr,
            "Utilities/socketxx/socket++/config.h.in";
        socketxx -= "Utilities/socketxx/socket++/sig.cpp";
        socketxx -= "Utilities/socketxx/socket++/sockunix.cpp";

        socketxx.Public +=
            "Utilities/socketxx/socket++"_id;
        socketxx.Public +=
            "Utilities/socketxx"_id;

        socketxx.Private += sw::Shared, "socketxx_EXPORTS"_d;
        socketxx.Public += sw::Shared, "GDCM_BUILD_SHARED_LIBS"_d;

        socketxx.Variables["SOCKET_MAJOR_VERSION"] = socketxx.Variables["PACKAGE_VERSION_MAJOR"];
        socketxx.Variables["SOCKET_MINOR_VERSION"] = socketxx.Variables["PACKAGE_VERSION_MINOR"];
        socketxx.Variables["SOCKET_MICRO_VERSION"] = socketxx.Variables["PACKAGE_VERSION_PATCH"];
        socketxx.Variables["VERSION"] = socketxx.Variables["PACKAGE_VERSION"];

        socketxx += "GDCM_HAVE_STRSIGNAL=1"_v;
        socketxx += "HAVE_STRING=1"_v;
        socketxx += "HAVE_SSTREAM=1"_v;
        socketxx += "HAVE_WORKING_FORK=1"_v;
        socketxx += "HAVE_WORKING_VFORK=1"_v;
        socketxx += "RETSIGTYPE=void"_v;
        socketxx += "SYS_SIGLIST=sys_siglist"_v;
        socketxx += "SYS_ERRLIST=sys_errlist"_v;
        socketxx += "SIGHND_ARGTYPE=int"_v;
        socketxx += "SYS_ERRLIST_DECLARED=1"_v;
        socketxx += "_S_LIBGXX=0"_v;

        if (socketxx.getBuildSettings().TargetOS.Type == OSType::Windows ||
            socketxx.getBuildSettings().TargetOS.Type == OSType::Mingw)
        {
            socketxx.Public += "WIN32"_d;
            socketxx.Public += "ws2_32.lib"_slib;
            socketxx -= "Utilities/socketxx/socket++/sockunix.cpp";
        }
        socketxx.Protected += IncludeDirectory(socketxx.BinaryPrivateDir);
        socketxx.configureFile("Utilities/socketxx/socket++/config.h.in", socketxx.BinaryPrivateDir / "config.h");
        socketxx.replaceInFileOnce("Utilities/socketxx/socket++/sockstream.h", "#  include <windows.h>", "//#  include <windows.h>\n#include <WinSock2.h>");
        socketxx.replaceInFileOnce("Utilities/socketxx/socket++/local.h", "#	include <windows.h>", "");
    }

    return;

    std::vector<LibraryTarget*> t_bits;
    for (auto &bits : { "8"s, "12"s, "16"s })
    {
        auto &jpeg = gdcm.addTarget<LibraryTarget>("jpeg" + bits);
        t_bits.push_back(&jpeg);
        jpeg.setChecks("socketxx");
        jpeg += "Utilities/gdcmjpeg/.*\\.c"_rr;
        jpeg += "Utilities/gdcmjpeg/.*\\.h"_rr;
        jpeg += "Utilities/gdcmjpeg/.*\\.in"_rr;
        jpeg -= "Utilities/gdcmjpeg/example.c";

        jpeg.Public += sw::Shared, "JPEGDLL"_def;
        jpeg.Public += sw::Static, "JPEGSTATIC"_def;

        jpeg.replaceInFileOnce("Utilities/gdcmjpeg/jmorecfg.h", "__declspec(dllexport)", "extern  __declspec(dllexport )");
        jpeg.replaceInFileOnce("Utilities/gdcmjpeg/jmorecfg.h", "__declspec(dllimport)", "extern \"C\" __declspec(dllimport )");
        jpeg.Variables["CMAKE_BITS_IN_JSAMPLE"] = bits;
        jpeg.configureFile("Utilities/gdcmjpeg/jpegcmake.h.in", bits + "/jpegcmake.h");
        jpeg.Variables["MANGLE_PREFIX"] = "jpeg" + bits;
        jpeg.configureFile("Utilities/gdcmjpeg/mangle_jpeg.h.in", bits + "/mangle_jpeg" + bits + "bits.h");
        for (auto &f : { "jmorecfg.h"s, "jconfig.h"s, "jpeglib.h"s, "jerror.h"s, "jinclude.h"s })
            jpeg.configureFile("Utilities/gdcmjpeg/" + f, bits + "/" + f, ConfigureFlags::CopyOnly);
        jpeg.Public += IncludeDirectory(jpeg.BinaryDir / bits);
    }

    auto &gdcm2 = gdcm.addTarget<LibraryTarget>("gdcm");
    {
        auto &gdcm = gdcm2;
        gdcm.setChecks("socketxx");
        gdcm.setChecks("gdcm");

        gdcm +=
            "Source/.*\\.cxx"_rr,
            "Source/.*\\.h.*"_rr,
            "Source/.*\\.txx"_rr,
            "Source/Common/gdcmConfigure.h.in";

        gdcm -=
            "Source/Common/gdcmTesting.cxx",
            "Source/DataDictionary/gdcmPrepDict.cxx",
            "Source/DataDictionary/gdcmPrepGroupName.cxx",
            "Source/DataDictionary/gdcmRoot.cxx",
            "Source/DataStructureAndEncodingDefinition/gdcmParser.cxx",
            "Source/InformationObjectDefinition/gdcmXMLDictReader.cxx",
            "Source/InformationObjectDefinition/gdcmXMLPrivateDictReader.cxx",
            "Source/MediaStorageAndFileFormat/gdcmDeltaEncodingCodec.cxx";

        gdcm.Public +=
            "Source/DataStructureAndEncodingDefinition"_id,
            "Source/DataDictionary"_id,
            "Source/InformationObjectDefinition"_id,
            "Source/Common"_id,
            "Source/MessageExchangeDefinition"_id,
            "Source/MediaStorageAndFileFormat"_id;

        gdcm.Public += "GDCM_USE_OPENJPEG_V2"_d;
        gdcm.Public += "GDCM_USE_SYSTEM_EXPAT"_d;
        gdcm.Public += "GDCM_USE_SYSTEM_JSON"_d;
        gdcm.Public += "GDCM_USE_SYSTEM_OPENSSL"_d;
        if (gdcm.getBuildSettings().TargetOS.Type == OSType::Windows ||
            gdcm.getBuildSettings().TargetOS.Type == OSType::Mingw)
        {
            gdcm.Public += "Ws2_32.lib"_slib;
            gdcm.Public += "Rpcrt4.lib"_slib;
        }
        gdcm.Private += sw::Shared, "gdcmCommon_EXPORTS"_d;
        gdcm.Public += sw::Shared, "GDCM_BUILD_SHARED_LIBS"_d;

        gdcm += rle;
        gdcm += socketxx;
        for (auto t : t_bits)
            gdcm += *t;
        gdcm.Public += "org.sw.demo.openssl.crypto"_dep;
        gdcm.Public += "org.sw.demo.madler.zlib"_dep;
        gdcm.Public += "org.sw.demo.uclouvain.openjpeg.openjp2"_dep;
        gdcm.Public += "org.sw.demo.json_c-master"_dep;
        gdcm.Public += "org.sw.demo.expat"_dep;
        gdcm.Public += charls;

        gdcm.Variables["GDCM_MAJOR_VERSION"] = gdcm.Variables["PACKAGE_VERSION_MAJOR"];
        gdcm.Variables["GDCM_MINOR_VERSION"] = gdcm.Variables["PACKAGE_VERSION_MINOR"];
        gdcm.Variables["GDCM_BUILD_VERSION"] = gdcm.Variables["PACKAGE_VERSION_PATCH"];
        gdcm.Variables["GDCM_VERSION"] = gdcm.Variables["PACKAGE_VERSION"];

        for (auto v : {
            "HAVE_SYS_TIME_H",
            "HAVE_WINSOCK_H",
            "HAVE_BYTESWAP_H",
            "HAVE_RPC_H",
            "HAVE_STRCASECMP",
            "HAVE_STRNCASECMP",

            "HAVE_SNPRINTF",
            "HAVE__SNPRINTF",
            "HAVE_STRPTIME",
            "HAVE__STRICMP",
            "HAVE__STRNICMP",
            "HAVE__SNPRINTF",
            })
            gdcm.Variables["GDCM_"s + v] = gdcm.Variables[v];

        gdcm.configureFile("Source/Common/gdcmConfigure.h.in", "gdcmConfigure.h");
        gdcm.replaceInFileOnce("Source/Common/zipstreamimpl.h", "gdcm_zlib.h", "zlib.h");
        gdcm.replaceInFileOnce("Source/Common/zipstreamimpl.hpp", "assert", "//assert");
        gdcm.replaceInFileOnce("Source/InformationObjectDefinition/gdcmTableReader.cxx", "gdcm_expat.h", "expat.h");
        gdcm.replaceInFileOnce("Source/MediaStorageAndFileFormat/gdcmJPEG2000Codec.cxx", "gdcm_openjpeg.h", "openjpeg.h");
        gdcm.replaceInFileOnce("Source/MediaStorageAndFileFormat/gdcmJPEG2000Codec.cxx", "gdcm_openjpeg2.h", "openjpeg.h");

        gdcm.replaceInFileOnce("Source/MediaStorageAndFileFormat/gdcmJPEG8Codec.cxx", "#include \"gdcm_ljpeg8.h\"",
            "extern \"C\" {\n#include \"8/jinclude.h\"\n#include \"8/jpeglib.h\"\n#include \"8/jerror.h\"\n}");
        gdcm.replaceInFileOnce("Source/MediaStorageAndFileFormat/gdcmJPEG12Codec.cxx", "#include \"gdcm_ljpeg12.h\"",
            "extern \"C\" {\n#include \"12/jinclude.h\"\n#include \"12/jpeglib.h\"\n#include \"12/jerror.h\"\n}");
        gdcm.replaceInFileOnce("Source/MediaStorageAndFileFormat/gdcmJPEG16Codec.cxx", "#include \"gdcm_ljpeg16.h\"",
            "extern \"C\" {\n#include \"16/jinclude.h\"\n#include \"16/jpeglib.h\"\n#include \"16/jerror.h\"\n}");
        gdcm.replaceInFileOnce("Source/Common/gdcmOpenSSLP7CryptographicMessageSyntax.cxx", "cert_ctx", "*cert_ctx");

        for (auto f : {
            "gdcmByteValue.cxx",
            "gdcmDataElement.cxx",
            "gdcmDataSet.cxx",
            "gdcmExplicitDataElement.cxx",
            "gdcmFile.cxx",
            "gdcmFileMetaInformation.cxx",
            "gdcmFragment.cxx",
            "gdcmImplicitDataElement.cxx",
            "gdcmItem.cxx",
            "gdcmMediaStorage.cxx",
            "gdcmPrivateTag.cxx",
            "gdcmReader.cxx",
            "gdcmSequenceOfFragments.cxx",
            "gdcmSequenceOfItems.cxx",
            "gdcmValue.cxx",
            })
        {
            gdcm.configureFile("Source/DataStructureAndEncodingDefinition/"s + f, f, ConfigureFlags::CopyOnly);
            gdcm += f;
            gdcm[f].args.push_back("-DGDCM_OVERRIDE_BROKEN_IMPLEMENTATION");
            gdcm[f].args.push_back("-Dgdcm_ns=gdcmstrict");
        }
    }
}

void check(Checker &c)
{
    {
        auto &s = c.addSet("socketxx");
        s.checkFunctionExists("fork");
        s.checkFunctionExists("select");
        s.checkFunctionExists("strsignal");
        s.checkFunctionExists("vfork");
        s.checkIncludeExists("dlfcn.h");
        s.checkIncludeExists("inttypes.h");
        s.checkIncludeExists("memory.h");
        s.checkIncludeExists("stddef.h");
        s.checkIncludeExists("stdint.h");
        s.checkIncludeExists("stdlib.h");
        s.checkIncludeExists("strings.h");
        s.checkIncludeExists("string.h");
        s.checkIncludeExists("sys/stat.h");
        s.checkIncludeExists("sys/types.h");
        s.checkIncludeExists("sys/wait.h");
        s.checkIncludeExists("unistd.h");
        s.checkIncludeExists("vfork.h");
        s.checkTypeSize("size_t");
        s.checkTypeSize("void *");

        for (auto &check : s.all)
        {
            check->Prefixes.insert("GDCM_");
            check->Prefixes.insert("SOCKETXX_");
        }
    }

    {
        auto &s = c.addSet("gdcm");
        {
            auto &c = s.checkFunctionExists("gettimeofday");
            c.Parameters.Includes.push_back("sys/time.h");
        }
        s.checkFunctionExists("strcasecmp");
        s.checkFunctionExists("strncasecmp");
        s.checkFunctionExists("strptime");
        s.checkFunctionExists("_snprintf");
        s.checkFunctionExists("_stricmp");
        s.checkFunctionExists("_strnicmp");
        s.checkIncludeExists("byteswap.h");
        s.checkIncludeExists("langinfo.h");
        s.checkIncludeExists("rpc.h");
        s.checkIncludeExists("sys/time.h");
        s.checkIncludeExists("winsock.h");
        s.checkTypeSize("size_t");
        s.checkTypeSize("void *");
        {
            auto &c = s.checkSymbolExists("snprintf");
            c.Parameters.Includes.push_back("stdio.h");
        }

        for (auto &check : s.all)
            check->Prefixes.insert("GDCM_");
    }
}
