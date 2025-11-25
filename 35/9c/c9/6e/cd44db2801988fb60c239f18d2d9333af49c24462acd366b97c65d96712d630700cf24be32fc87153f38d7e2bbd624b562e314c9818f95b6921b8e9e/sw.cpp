void build(Solution &s)
{
    auto &p = s.addExecutable("Kitware.CMake", "4.2.0");
    p += Git("https://github.com/Kitware/CMake", "v{v}");

    auto &iml = p.addLibrary("iml");
    iml.setRootDirectory("Utilities/KWIML");

    auto &cm3p = p.addLibrary("cm3p");
    {
        auto &t = cm3p;
        t.setRootDirectory("Utilities");

        t += "cm3p/.*"_rr;
        t += "cmvssetup/.*"_rr;
        t += "cmelf/.*"_rr;

        t.Public += iml;

        t.Public += "org.sw.demo.madler.zlib"_dep;
        t.Public += "org.sw.demo.libarchive.libarchive"_dep;
        t.Public += "org.sw.demo.badger.curl.libcurl"_dep;
        t.Public += "org.sw.demo.open_source_parsers.jsoncpp"_dep;
        t.Public += "org.sw.demo.aleksey14.rhash"_dep;
        t.Public += "org.sw.demo.expat"_dep;
        t.Public += "org.sw.demo.libuv"_dep;

        t.Variables["CMAKE_USE_SYSTEM_CURL"] = 1;
        t.Variables["CMAKE_USE_SYSTEM_EXPAT"] = 1;
        t.Variables["CMAKE_USE_SYSTEM_KWIML"] = 1;
        t.Variables["CMAKE_USE_SYSTEM_ZLIB"] = 1;
        t.Variables["CMAKE_USE_SYSTEM_BZIP2"] = 1;
        t.Variables["CMAKE_USE_SYSTEM_LIBARCHIVE"] = 1;
        t.Variables["CMAKE_USE_SYSTEM_LIBLZMA"] = 1;
        t.Variables["CMAKE_USE_SYSTEM_FORM"] = 1;
        t.Variables["CMAKE_USE_SYSTEM_JSONCPP"] = 1;
        t.Variables["CMAKE_USE_SYSTEM_LIBRHASH"] = 1;
        t.Variables["CMAKE_USE_SYSTEM_LIBUV"] = 1;
        t.Variables["CMAKE_USE_SYSTEM_ZSTD"] = 1;

        t.configureFile("cmThirdParty.h.in", "cmThirdParty.h");
    }

    auto &std = p.addStaticLibrary("std");
    {
        std.setRootDirectory("Utilities/std");
        std += cpp17;
        std += "cm/.*"_rr;
        std += "cmext/.*"_rr;
        std.Variables["CMake_HAVE_CXX_MAKE_UNIQUE"] = 1;
        std.Variables["CMake_HAVE_CXX_FILESYSTEM"] = 1;
        std.configureFile("cmSTL.hxx.in", "cmSTL.hxx");
        std.Protected += cm3p;
    }

    auto &cmcppdap = p.addStaticLibrary("cmcppdap");
    {
        cmcppdap += cpp17;
        cmcppdap += "Utilities/cmcppdap/include/.*"_rr;
        cmcppdap += "Utilities/cmcppdap/src/.*"_rr;
        cmcppdap -= "Utilities/cmcppdap/src/.*_test.cpp"_rr;
        cmcppdap -= "Utilities/cmcppdap/src/jsoncpp_json_serializer.cpp"_rr;
        cmcppdap -= "Utilities/cmcppdap/src/rapid_json_serializer.cpp"_rr;
        cmcppdap.Public += "CPPDAP_JSON_NLOHMANN"_def;
        cmcppdap.Public += "Utilities"_idir;
        cmcppdap.Public += "Utilities/cmcppdap/include"_idir;
        cmcppdap.Public += "org.sw.demo.nlohmann.json"_dep;
    }

    auto &cmllpkgc = p.addLibrary("cmllpkgc");
    {
        cmllpkgc += cpp17;
        cmllpkgc += "Utilities/cmllpkgc/.*"_rr;
        cmllpkgc.Public += "Utilities"_idir;
    }

    auto &sys = p.addLibrary("sys");
    {
        auto &t = sys;
        t.setRootDirectory("Source");
        t += cpp17;
        t += "kwsys/.*\\.[hc]x?x?"_r;
        t += "kwsys/.*\\.in"_r;
        t -= "kwsys/test.*"_r;
        t -= "kwsys/.*Tests.*"_r;
        t -= "kwsys/ProcessWin32.c";
        t -= "kwsys/ProcessUNIX.c";
        t += "kwsys/SystemTools.cxx";

        const String kwsys = "cmsys";
        t += sw::Shared, Definition(kwsys + "_EXPORTS");

        t += Definition("KWSYS_NAMESPACE=" + kwsys);
        t += "KWSYS_USE_LONG_LONG"_def;
        t += "KWSYS_IOS_HAS_ISTREAM_LONG_LONG"_def;
        t += "KWSYS_IOS_HAS_OSTREAM_LONG_LONG"_def;
        if (!t.getBuildSettings().TargetOS.isApple())
            t += "KWSYS_CXX_HAS_ENVIRON_IN_STDLIB_H"_def;
        t += "KWSYS_STRING_C"_def;
        if (t.getBuildSettings().TargetOS.is(OSType::Windows) || t.getBuildSettings().TargetOS.is(OSType::Mingw))
        {
            t += "kwsys/ProcessWin32.c";
            t.Protected += "KWSYS_ENCODING_DEFAULT_CODEPAGE=CP_ACP"_def;
            t += "advapi32.lib"_slib;
            if (t.getBuildSettings().TargetOS.is(OSType::Windows))
            {
                if (t.getBuildSettings().Native.ConfigurationType == ConfigurationType::Debug)
                    t += "comsuppwd.lib"_slib;
                else
                    t += "comsuppw.lib"_slib;
            }
            t += "Dbghelp.lib"_slib;
            t += "ole32.lib"_slib;
            t += "OleAut32.lib"_slib;
            t += "Shell32.lib"_slib;
            t += "uuid.lib"_slib;
            t += "ws2_32.lib"_slib;
            t += "PowrProf.lib"_slib;
            t += "User32.lib"_slib;
        }
        else
        {
            t += "kwsys/ProcessUNIX.c";
        }

        t.Variables["KWSYS_NAMESPACE"] = kwsys;
        t.Variables["KWSYS_NAME_IS_KWSYS"] = kwsys == "kwsys" ? 1 : 0;
        t.Variables["KWSYS_BUILD_SHARED"] = t.getBuildSettings().Native.LibrariesType == LibraryType::Shared ? 1 : 0;

        // Enable all components.
        t.Variables["KWSYS_USE_Base64"] = 1;
        t.Variables["KWSYS_USE_Directory"] = 1;
        t.Variables["KWSYS_USE_DynamicLoader"] = 1;
        t.Variables["KWSYS_USE_Encoding"] = 1;
        t.Variables["KWSYS_USE_Glob"] = 1;
        t.Variables["KWSYS_USE_MD5"] = 1;
        t.Variables["KWSYS_USE_Process"] = 1;
        t.Variables["KWSYS_USE_RegularExpression"] = 1;
        t.Variables["KWSYS_USE_System"] = 1;
        t.Variables["KWSYS_USE_SystemTools"] = 1;
        t.Variables["KWSYS_USE_CommandLineArguments"] = 1;
        t.Variables["KWSYS_USE_Terminal"] = 1;
        t.Variables["KWSYS_USE_IOStream"] = 1;
        t.Variables["KWSYS_USE_FStream"] = 1;
        t.Variables["KWSYS_USE_String"] = 1;
        t.Variables["KWSYS_USE_SystemInformation"] = 1;
        t.Variables["KWSYS_USE_ConsoleBuf"] = 1;

        t.Variables["KWSYS_IOS_HAS_ISTREAM_LONG_LONG"] = 1;
        t.Variables["KWSYS_IOS_HAS_OSTREAM_LONG_LONG"] = 1;
        t.Variables["KWSYS_USE_LONG_LONG"] = 1;

        t.Variables["KWSYS_CXX_HAS_EXT_STDIO_FILEBUF_H"] = 0;
        t.Variables["KWSYS_STL_HAS_WSTRING"] = 1;
        t.Variables["KWSYS_SYSTEMTOOLS_USE_TRANSLATION_MAP"] = 1;

        t.patch("kwsys/SystemTools.hxx.in",
            "class WindowsFileId",
            "class @KWSYS_NAMESPACE@_EXPORT WindowsFileId"
        );
        t.patch("kwsys/SystemTools.hxx.in",
            "class UnixFileId",
            "class @KWSYS_NAMESPACE@_EXPORT UnixFileId"
        );
        for (auto &[p, f] : t["kwsys/.*\\.in"_r])
            t.configureFile(p, path(kwsys) / p.stem());

        t -= "org.sw.demo.tronkko.dirent-master"_dep;
        if (t.getBuildSettings().TargetOS.is(OSType::Windows) || t.getBuildSettings().TargetOS.is(OSType::Mingw))
            t += "org.sw.demo.tronkko.dirent-master"_dep;
    }

    auto &lib = p.addStaticLibrary("lib");
    {
        auto &t = lib;

        t += cpp17;

        //
        t += "Source/.*\\.[hc]x?x?"_r;
        t += "Source/.*\\.in"_r;
        t += "Source/.*\\.tcc"_r;
        t += "Source/.*\\.def"_r;
        t += "Source/CTest/cmCTestTypes.h";

        t -= "Source/ctest.cxx";
        t -= "Source/cmCTest.cxx";
        t -= "Source/cmFileLockWin32.cxx";
        t -= "Source/cmFileLockUnix.cxx";
        //t -= "Source/cmCPluginAPI.cxx";
        t -= "Source/cmcldeps.cxx";
        t -= "Source/cmakemain.cxx";
        t -= "Source/cmcmd.cxx";
        //t -= "Source/cmELF.cxx";
        t -= "Source/cmMachO.cxx";
        t -= "Source/cmXCOFF.cxx";
        t -= "Source/cmXCodeObject.cxx";
        t -= "Source/cmGlobalXCodeGenerator.cxx";

        //
        t += "Source/LexerParser/.*"_r;
        //t ^= "Source/LexerParser/\\..*"_r;
        t -= "Source/LexerParser/cmCTestResourceGroupsLexer.cxx";

        // needed
        //t -= "Utilities/cmlibarchive/libarchive/archive_getdate.c";
        //t -= "Utilities/cmlibarchive/libarchive/archive_getdate.h";
        t.patch("Utilities/cmlibarchive/libarchive/archive_parse_date.c", R"(#include "archive.h")", R"(
#ifndef CM_PARSE_DATE
#include "archive.h"
#endif
    )");

        if (t.getBuildSettings().TargetOS.is(OSType::Windows) || t.getBuildSettings().TargetOS.is(OSType::Mingw))
        {
            t -= "Source/cmDebuggerPosixPipeConnection.cxx";
            t += "WIN32_LEAN_AND_MEAN"_def;
            t.Variables["HAVE_ENVIRON_NOT_REQUIRE_PROTOTYPE"] = 1;
        }
        else
        {
            t -= "Source/cmDebuggerWindowsPipeConnection.cxx";
            t -= "Source/cmVSSetupHelper.cxx";
            t -= "Source/.*VisualStudio.*"_r;
        }

        if (t.getBuildSettings().TargetOS.isApple())
        {
            t += "Source/cmXCodeObject.cxx";
            t += "Source/cmGlobalXCodeGenerator.cxx";
            t += "CoreServices"_framework;
        }

        //
        t.Public += "Source"_idir;
        t += "Source/LexerParser"_idir;

        t.Variables["CMake_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
        t.Variables["CMake_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
        t.Variables["CMake_VERSION_PATCH"] = t.Variables["PACKAGE_VERSION_PATCH"];
        t.Variables["CMake_VERSION"] = t.Variables["PACKAGE_VERSION"];
        t.Variables["CMake_VERSION_IS_DIRTY"] = 0;

        t.configureFile("Source/cmVersionConfig.h.in", "cmVersionConfig.h");

        //t.Variables["CMake_USE_XCOFF_PARSER"] = 1; // aix
        t.Variables["CMake_DEFAULT_RECURSION_LIMIT"] = 1000;
        t.Variables["CMAKE_BIN_DIR"] = "bin";
        t -= "Modules/.*"_rr;
        t.Variables["CMAKE_DATA_DIR"] = normalize_path(t.SourceDir / "Modules");
        t.configureFile("Source/cmConfigure.cmake.h.in", "cmConfigure.h");

        t.Public += cm3p;
        t.Public += std;
        t.Public += sys;
        t.Public += cmcppdap;
        t.Public += cmllpkgc;
    }

    auto &cmake = p;
    {
        cmake += cpp17;
        cmake += "Source/cmakemain.cxx";
        cmake += "Source/cmcmd.cxx";
        cmake += lib;
    }
}
