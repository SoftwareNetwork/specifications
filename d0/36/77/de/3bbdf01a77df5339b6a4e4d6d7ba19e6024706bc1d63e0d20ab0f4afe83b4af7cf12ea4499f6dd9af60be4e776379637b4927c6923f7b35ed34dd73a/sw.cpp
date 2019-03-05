void build(Solution &s)
{
    auto &xerces = s.addTarget<LibraryTarget>("apache.xerces_c", "3.2.2");
    xerces += Git("https://github.com/apache/xerces-c", "Xerces-C_{M}_{m}_{p}");

    xerces.setChecks("xerces", true);

    xerces +=
        "src/xercesc/.*"_rr;

    xerces -=
        "src/xercesc/.*\\.c"_rr,
        "src/xercesc/.*\\.rc"_rr;

    xerces.Public +=
        "src"_id;

    xerces += "XERCES_BUILDING_LIBRARY"_d;
    xerces.Public += "XERCES_AUTOCONF"_d;
    xerces.Public += "XERCES_USE_MSGLOADER_INMEMORY"_d;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        xerces.Public += "WIN32"_d;
        xerces += "advapi32.lib"_slib;

        xerces.Public += "XERCES_USE_FILEMGR_WINDOWS"_d;
        xerces.Public += "XERCES_USE_MUTEXMGR_WINDOWS"_d;
        xerces.Public += "XERCES_USE_NETACCESSOR_WINSOCK"_d;
        xerces.Public += "XERCES_USE_TRANSCODER_WINDOWS"_d;
        xerces.Public += "XERCES_USE_WIN32_MSGLOADER"_d;

        xerces -=
            "src/xercesc/util/FileManagers/PosixFileMgr.cpp",
            "src/xercesc/util/MutexManagers/PosixMutexMgr.cpp",
            "src/xercesc/util/NetAccessors/Socket/UnixHTTPURLInputStream.cpp",
            "src/xercesc/util/NetAccessors/Socket/SocketNetAccessor.cpp",
            "src/xercesc/util/Transcoders/MacOSUnicodeConverter/MacOSUnicodeConverter.cpp",
            "src/xercesc/util/MsgLoaders/ICU/ICUMsgLoader.cpp",
            "src/xercesc/util/NetAccessors/MacOSURLAccessCF/URLAccessCFBinInputStream.cpp",
            "src/xercesc/util/NetAccessors/Curl/CurlURLInputStream.cpp",
            "src/xercesc/util/Transcoders/ICU/ICUTransService.cpp",
            "src/xercesc/util/NetAccessors/Curl/CurlNetAccessor.cpp",
            "src/xercesc/util/MsgLoaders/MsgCatalog/MsgCatalogLoader.cpp",
            "src/xercesc/util/NetAccessors/MacOSURLAccessCF/MacOSURLAccessCF.cpp",
            "src/xercesc/util/Transcoders/IconvGNU/IconvGNUTransService.cpp"
            ;
    }
    else
    {
        xerces.Public += "XERCES_USE_FILEMGR_POSIX"_d;
        xerces.Public += "XERCES_USE_MUTEXMGR_POSIX"_d;
        xerces.Public += "XERCES_USE_NETACCESSOR_SOCKET"_d;
    }
    xerces += sw::Shared, "XERCES_USE_CHAR16_T"_d;
    xerces.Public += sw::Shared, "XERCES_DLL_EXPORT"_d;
    xerces.Public += sw::Static, "XERCES_STATIC_LIBRARY"_d;

    xerces.Variables["XERCES_HAS_CPP_NAMESPACE"] = 1;
    xerces.Variables["XERCES_STD_NAMESPACE"] = 1;
    xerces.Variables["XERCES_NEW_IOSTREAMS"] = 1;

    xerces.Variables["XERCES_S16BIT_INT"] = "short";
    xerces.Variables["XERCES_S32BIT_INT"] = "int";
    xerces.Variables["XERCES_S64BIT_INT"] = "long long";
    xerces.Variables["XERCES_U16BIT_INT"] = "unsigned short";
    xerces.Variables["XERCES_U32BIT_INT"] = "unsigned";
    xerces.Variables["XERCES_U64BIT_INT"] = "unsigned long long";
    xerces.Variables["XERCES_XMLCH_T"] = "char16_t";
    xerces.Variables["XERCES_SIZE_T"] = "size_t";
    xerces.Variables["XERCES_SSIZE_T"] = "long";

    xerces.Variables["XERCES_SIZE_MAX"] = "SIZE_MAX";
    xerces.Variables["XERCES_SSIZE_MAX"] = "LONG_MAX";

    xerces.ApiName = "SW_XERCES_API";
    xerces.Variables["XERCES_PLATFORM_EXPORT"] = xerces.ApiName;
    xerces.Variables["XERCES_PLATFORM_IMPORT"] = xerces.ApiName;

    xerces.Variables["XERCES_VERSION_MAJOR"] = xerces.Variables["PACKAGE_VERSION_MAJOR"];
    xerces.Variables["XERCES_VERSION_MINOR"] = xerces.Variables["PACKAGE_VERSION_MINOR"];
    xerces.Variables["XERCES_VERSION_REVISION"] = xerces.Variables["PACKAGE_VERSION_PATCH"];

    xerces.Variables["XERCES_GRAMMAR_SERIALIZATION_LEVEL"] = 7;

    xerces.writeFileOnce(xerces.BinaryPrivateDir / "config.h");
    xerces.configureFile("src/xercesc/util/Xerces_autoconf_config.hpp.cmake.in", "xercesc/util/Xerces_autoconf_config.hpp");
    xerces.configureFile("src/xercesc/util/XercesVersion.hpp.cmake.in", "xercesc/util/XercesVersion.hpp");
}

void check(Checker &c)
{
    auto &s = c.addSet("xerces");
    s.checkFunctionExists("clock_gettime");
    s.checkFunctionExists("error_at_line");
    s.checkFunctionExists("ftime");
    s.checkFunctionExists("getaddrinfo");
    s.checkFunctionExists("getcwd");
    s.checkFunctionExists("gethostbyaddr");
    s.checkFunctionExists("gethostbyname");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("localeconv");
    s.checkFunctionExists("malloc");
    s.checkFunctionExists("mblen");
    s.checkFunctionExists("memcmp");
    s.checkFunctionExists("memmove");
    s.checkFunctionExists("memset");
    s.checkFunctionExists("nl_langinfo");
    s.checkFunctionExists("pathconf");
    s.checkFunctionExists("realpath");
    s.checkFunctionExists("setlocale");
    s.checkFunctionExists("socket");
    s.checkFunctionExists("strcasecmp");
    s.checkFunctionExists("strchr");
    s.checkFunctionExists("strcoll");
    s.checkFunctionExists("strdup");
    s.checkFunctionExists("stricmp");
    s.checkFunctionExists("strncasecmp");
    s.checkFunctionExists("strnicmp");
    s.checkFunctionExists("strrchr");
    s.checkFunctionExists("strstr");
    s.checkFunctionExists("strtod");
    s.checkFunctionExists("strtol");
    s.checkFunctionExists("strtoul");
    s.checkFunctionExists("towlower");
    s.checkFunctionExists("towupper");
    s.checkFunctionExists("wcsicmp");
    s.checkFunctionExists("wcslwr");
    s.checkFunctionExists("wcsnicmp");
    s.checkFunctionExists("wcsupr");
    s.checkIncludeExists("arpa/inet.h");
    s.checkIncludeExists("arpa/nameser_compat.h");
    s.checkIncludeExists("CoreServices/CoreServices.h");
    s.checkIncludeExists("endian.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("float.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("langinfo.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("machine/endian.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("netdb.h");
    s.checkIncludeExists("netinet/in.h");
    s.checkIncludeExists("nl_types.h");
    s.checkIncludeExists("stdbool.h");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("cstdint").Parameters.cpp = true;
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/timeb.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("time.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("wchar.h");
    s.checkIncludeExists("wctype.h");
    s.checkTypeSize("off_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("void *");
    s.checkTypeSize("wchar_t");
    s.checkSourceCompiles("HAVE_TIME_WITH_SYS_TIME", R"sw_xxx(
    #include <time.h>
    #include <sys/time.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");

    for (auto &check : s.all)
        check->Prefixes.insert("XERCES_");
}
