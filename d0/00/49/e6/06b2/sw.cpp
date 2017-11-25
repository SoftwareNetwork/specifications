void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo.malaterre");
    auto &p = s.addProject("gdcm", "2.8.4");
    p.Source = Git("https://github.com/malaterre/GDCM", "v{v}");

    auto &rle = p.addTarget<StaticLibraryTarget>("rle");
    rle.setRootDirectory("Utilities");

    rle +=
        "gdcmrle/.*\\.cxx"_rr,
        "gdcmrle/.*\\.h"_rr;

    //
    auto &socketxx = p.addTarget<LibraryTarget>("socketxx");
    socketxx.setRootDirectory("Utilities/socketxx");
    socketxx.ApiName = "MY_API";

    socketxx.setChecks("socketxx");

    socketxx +=
        ".*\\.cpp"_rr,
        ".*\\.h"_rr,
        "socket++/config.h.in";

    socketxx.Public +=
        "socket++"_id;

    socketxx.Private += sw::Shared, "socketxx_EXPORTS"_d;
    socketxx.Public += sw::Shared, "GDCM_BUILD_SHARED_LIBS"_d;

    socketxx.Variables["SOCKET_MAJOR_VERSION"] = socketxx.Variables["PACKAGE_VERSION_MAJOR"];
    socketxx.Variables["SOCKET_MINOR_VERSION"] = socketxx.Variables["PACKAGE_VERSION_MINOR"];
    socketxx.Variables["SOCKET_MICRO_VERSION"] = socketxx.Variables["PACKAGE_VERSION_PATCH"];
    socketxx.Variables["VERSION"] = socketxx.Variables["PACKAGE_VERSION"];

    socketxx.Variables["GDCM_HAVE_STRSIGNAL"] = socketxx.Variables["HAVE_STRSIGNAL"];
    socketxx.Variables["HAVE_STRING"] = "1";
    socketxx.Variables["HAVE_SSTREAM"] = "1";
    socketxx.Variables["HAVE_WORKING_FORK"] = "1";
    socketxx.Variables["HAVE_WORKING_VFORK"] = "1";

    if (s.Settings.TargetOS.Type == OSType::Windows)
        socketxx.replaceInFileOnce("socket++/config.h.in", "<local.h>", "<io.h>");
    socketxx.configureFile("socket++/config.h.in", "config.h");

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        socketxx.Public += "WIN32"_d;
        socketxx -= "socket++/sockunix.cpp";
        socketxx -= "socket++/sig.cpp";
        socketxx.Public += "ws2_32.lib"_lib;
    }

    socketxx.replaceInFileOnce("socket++/sockstream.h", "<windows.h>", "<WinSock2.h>");

    //
    auto &gdcm = p.addTarget<LibraryTarget>("gdcm");

    //
    auto add_jpeg = [&p, &gdcm](int bits)
    {
        const auto s = std::to_string(bits);
        auto &jpeg = p.addTarget<LibraryTarget>("jpeg" + s);
        jpeg.setRootDirectory("Utilities/gdcmjpeg");

        jpeg.setChecks("jpeg");

        jpeg +=
            ".*\\.c"_rr,
            ".*\\.h"_rr,
            ".*\\.in"_rr;

        jpeg -=
            "example.c";

        jpeg.Private += sw::Shared, "JPEGDLL"_d;
        jpeg.Public += sw::Static, "JPEGSTATIC"_d;

        jpeg.replaceInFileOnce("jmorecfg.h", "__declspec(dllexport)", "extern __declspec(dllexport)");
        jpeg.replaceInFileOnce("jmorecfg.h", "__declspec(dllimport)", "extern \"C\" __declspec(dllimport)");

        jpeg.Variables["CMAKE_BITS_IN_JSAMPLE"] = s;
        jpeg.configureFile("jpegcmake.h.in", s + "/jpegcmake.h");

        jpeg.Variables["MANGLE_PREFIX"] = "jpeg" + s;
        jpeg.configureFile("mangle_jpeg.h.in", s + "/mangle_jpeg" + s + "bits.h");

        jpeg.configureFile("jmorecfg.h", s + "/jmorecfg.h");
        jpeg.configureFile("jconfig.h", s + "/jconfig.h");
        jpeg.configureFile("jpeglib.h", s + "/jpeglib.h");
        jpeg.configureFile("jerror.h", s + "/jerror.h");
        jpeg.configureFile("jinclude.h", s + "/jinclude.h");

        jpeg += IncludeDirectory(jpeg.BinaryDir / s);

        gdcm += jpeg;
    };

    add_jpeg(8);
    add_jpeg(12);
    add_jpeg(16);

    //
    gdcm.setRootDirectory("Source");
    gdcm.setChecks("gdcm");

    gdcm +=
        ".*\\.cpp"_rr,
        ".*\\.cxx"_rr,
        ".*\\.h"_rr,
        ".*\\.hpp"_rr,
        ".*\\.txx"_rr,
        "Common/gdcmConfigure.h.in";

    gdcm -=
        "MediaStorageAndFileFormat/gdcmJPEGBITSCodec.cxx";

    gdcm -=
        "Common/gdcmTesting.cxx",
        "DataDictionary/gdcmPrepDict.cxx",
        "DataDictionary/gdcmPrepGroupName.cxx",
        "DataDictionary/gdcmRoot.cxx",
        "DataStructureAndEncodingDefinition/gdcmParser.cxx",
        "InformationObjectDefinition/gdcmXMLDictReader.cxx",
        "InformationObjectDefinition/gdcmXMLPrivateDictReader.cxx",
        "MediaStorageAndFileFormat/gdcmDeltaEncodingCodec.cxx";

    gdcm.Public +=
        "MediaStorageAndFileFormat"_id,
        "InformationObjectDefinition"_id,
        "DataStructureAndEncodingDefinition"_id,
        "DataDictionary"_id,
        "MessageExchangeDefinition"_id,
        "Common"_id;

    gdcm.Public += "GDCM_USE_OPENJPEG_V2"_d;
    gdcm.Public += "GDCM_USE_SYSTEM_CHARLS"_d;
    gdcm.Public += "GDCM_USE_SYSTEM_EXPAT"_d;
    gdcm.Public += "GDCM_USE_SYSTEM_JSON"_d;
    gdcm.Public += "GDCM_USE_SYSTEM_OPENSSL"_d;
    gdcm.Private += sw::Shared, "gdcmCommon_EXPORTS"_d;
    gdcm.Public += sw::Shared, "GDCM_BUILD_SHARED_LIBS"_d;

    gdcm.Public += "pub.cppan2.demo.expat-2"_dep;
    gdcm.Public += "pub.cppan2.demo.json_c-master"_dep;
    gdcm.Public += "pub.cppan2.demo.madler.zlib-1"_dep;
    gdcm.Public += "pub.cppan2.demo.uclouvain.openjpeg.openjp2-2"_dep;
    gdcm.Public += "pub.cppan2.demo.openssl.crypto-1"_dep;
    gdcm.Public += "pub.cppan2.demo.vbaderks.charls-1"_dep;
    gdcm += socketxx;
    gdcm += rle;

    if (s.Settings.TargetOS.Type == OSType::Windows)
        gdcm += "Rpcrt4.lib"_lib;

    gdcm.Variables["GDCM_MAJOR_VERSION"] = gdcm.Variables["PACKAGE_VERSION_MAJOR"];
    gdcm.Variables["GDCM_MINOR_VERSION"] = gdcm.Variables["PACKAGE_VERSION_MINOR"];
    gdcm.Variables["GDCM_BUILD_VERSION"] = gdcm.Variables["PACKAGE_VERSION_PATCH"];
    gdcm.Variables["GDCM_VERSION"] = gdcm.Variables["PACKAGE_VERSION"];

    gdcm.configureFile("Common/gdcmConfigure.h.in", "gdcmConfigure.h");
    gdcm.replaceInFileOnce("Common/zipstreamimpl.h", "gdcm_zlib.h", "zlib.h");
    gdcm.replaceInFileOnce("InformationObjectDefinition/gdcmTableReader.cxx", "gdcm_expat.h", "expat.h");
    gdcm.replaceInFileOnce("MediaStorageAndFileFormat/gdcmJPEG2000Codec.cxx", "gdcm_openjpeg.h", "openjpeg.h");
    gdcm.replaceInFileOnce("MediaStorageAndFileFormat/gdcmJPEG2000Codec.cxx", "gdcm_openjpeg2.h", "openjpeg.h");

    gdcm.replaceInFileOnce("MediaStorageAndFileFormat/gdcmJPEG8Codec.cxx", "gdcm_ljpeg8.h",
        "8/jinclude.h\"\n#include \"8/jpeglib.h\"\n#include \"8/jerror.h");
    gdcm.replaceInFileOnce("MediaStorageAndFileFormat/gdcmJPEG12Codec.cxx", "gdcm_ljpeg12.h",
        "12/jinclude.h\"\n#include \"12/jpeglib.h\"\n#include \"12/jerror.h");
    gdcm.replaceInFileOnce("MediaStorageAndFileFormat/gdcmJPEG16Codec.cxx", "gdcm_ljpeg16.h",
        "16/jinclude.h\"\n#include \"16/jpeglib.h\"\n#include \"16/jerror.h");

    gdcm.replaceInFileOnce("Common/gdcmOpenSSLP7CryptographicMessageSyntax.cxx", "cert_ctx", "*cert_ctx");

    gdcm.replaceInFileOnce("MediaStorageAndFileFormat/gdcmJPEGLSCodec.cxx", "#include \"gdcm_charls.h\"",
        "#include <header.h>\n#include <interface.h>\n#include <util.h>\n#include <defaulttraits.h>\n#include <losslesstraits.h>\n#include <colortransform.h>\n#include <processline.h>");

    const Files DSED2{
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
    };

    for (auto &f : DSED2)
    {
        const path o = "strict_" + f.string();
        gdcm.configureFile(path("DataStructureAndEncodingDefinition") / f, o, ConfigureFlags::CopyOnly);
        gdcm += o;
        if (auto sf = gdcm[o].as<NativeSourceFile>())
        {
            sf->compiler->add("GDCM_OVERRIDE_BROKEN_IMPLEMENTATION"_d);
            sf->compiler->add("gdcm_ns=gdcmstrict"_d);
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
    }

    {
        auto &s = c.addSet("jpeg");
        s.checkIncludeExists("stddef.h");
        s.checkIncludeExists("stdint.h");
        s.checkIncludeExists("stdlib.h");
        s.checkTypeSize("size_t");
        s.checkTypeSize("void *");
    }

    {
        auto &s = c.addSet("gdcm");
        s.checkFunctionExists("strcasecmp");
        s.checkFunctionExists("strncasecmp");
        s.checkFunctionExists("strptime");
        s.checkFunctionExists("_snprintf");
        s.checkFunctionExists("_stricmp");
        s.checkFunctionExists("_strnicmp");
        s.checkFunctionExists("uuid_generate");
        {
            auto &c = s.checkFunctionExists("UuidCreate");
            c.Parameters.Includes.push_back("rpc.h");
        }
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

        for (auto &[d, c] : s.checks)
            c->Prefixes.insert("GDCM_");
    }
}
