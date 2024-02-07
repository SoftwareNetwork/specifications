void build(Solution &s)
{
    auto &libxml2 = s.addTarget<LibraryTarget>("xmlsoft.libxml2", "2.12.5");
    libxml2 += Git("https://gitlab.gnome.org/GNOME/libxml2", "v{v}");

    libxml2.setChecks("libxml2", true);

    if (libxml2.getBuildSettings().TargetOS.Type != OSType::Windows)
        libxml2.ExportAllSymbols = true;

    libxml2 +=
        ".*\\.h"_rr,
        "HTMLparser.c",
        "HTMLtree.c",
        "SAX.c",
        "SAX2.c",
        "buf.c",
        "c14n.c",
        "catalog.c",
        "chvalid.c",
        "debugXML.c",
        "dict.c",
        "encoding.c",
        "entities.c",
        "error.c",
        "globals.c",
        "hash.c",
        "include/libxml/xmlversion.h.in",
        "legacy.c",
        "list.c",
        "nanoftp.c",
        "nanohttp.c",
        "parser.c",
        "parserInternals.c",
        "pattern.c",
        "relaxng.c",
        "schematron.c",
        "threads.c",
        "tree.c",
        "trio.c",
        "trionan.c",
        "triostr.c",
        "uri.c",
        "valid.c",
        "xinclude.c",
        "xlink.c",
        "xmlIO.c",
        "xmlmemory.c",
        "xmlmodule.c",
        "xmlreader.c",
        "xmlregexp.c",
        "xmlsave.c",
        "xmlschemas.c",
        "xmlschemastypes.c",
        "xmlstring.c",
        "xmlunicode.c",
        "xmlwriter.c",
        "xpath.c",
        "xpointer.c",
        "xzlib.c";

    libxml2 -=
        "trio.c",
        "trionan.c",
        "triostr.c";

    libxml2.Public +=
        "include"_id;

    libxml2.Private += "HAVE_ZLIB_H"_d;
    libxml2.Private += "IN_LIBXML"_d;
    //libxml2.Private += "LIBXML_THREAD_ENABLED"_d;
    libxml2.Private += "HAVE_VA_COPY"_d;
    if (libxml2.getBuildSettings().TargetOS.Type == OSType::Windows ||
        libxml2.getBuildSettings().TargetOS.Type == OSType::Mingw)
    {
        libxml2.Private += "WIN32"_d;
        libxml2.Private += "HAVE_WIN32_THREADS"_d;
    }
    libxml2.Public += sw::Static, "LIBXML_STATIC"_d;

    libxml2.Public += "org.sw.demo.gnu.iconv.libiconv"_dep;
    libxml2.Public += "org.sw.demo.madler.zlib"_dep;

    if (libxml2.getBuildSettings().TargetOS.Type == OSType::Windows ||
        libxml2.getBuildSettings().TargetOS.Type == OSType::Mingw)
        libxml2 += "ws2_32.lib"_slib;

    libxml2.Variables["VERSION"] = libxml2.getPackage().getVersion().toString();
    libxml2.Variables["LIBXML_VERSION_NUMBER"] =
        libxml2.getPackage().getVersion().getMajor() * 10000 +
        libxml2.getPackage().getVersion().getMinor() * 100 +
        libxml2.getPackage().getVersion().getPatch() * 1
        ;
    libxml2 +=
        "WITH_ZLIB=1"_v,
        "WITH_LZMA=0"_v,
        "WITH_ICU=0"_v,
        "WITH_ICONV=1"_v,
        "WITH_TRIO=0"_v,
        "WITH_TREE=1"_v,
        "WITH_OUTPUT=1"_v,
        "WITH_PUSH=1"_v,
        "WITH_READER=1"_v,
        "WITH_PATTERN=1"_v,
        "WITH_WRITER=1"_v,
        "WITH_SAX1=1"_v,
        "WITH_FTP=1"_v,
        "WITH_HTTP=1"_v,
        "WITH_VALID=1"_v,
        "WITH_HTML=1"_v,
        "WITH_LEGACY=1"_v,
        "WITH_C14N=1"_v,
        "WITH_CATALOG=1"_v,
        "WITH_DOCB=0"_v,
        "WITH_XPATH=1"_v,
        "WITH_XPTR=1"_v,
        "WITH_XPTR_LOCS=1"_v,
        "WITH_XINCLUDE=1"_v,
        "WITH_ISO8859X=1"_v,
        "WITH_DEBUG=0"_v,
        "WITH_MEM_DEBUG=0"_v,
        "WITH_RUN_DEBUG=0"_v,
        "WITH_REGEXPS=1"_v,
        "WITH_SCHEMAS=1"_v,
        "WITH_SCHEMATRON=1"_v,
        "WITH_MODULES=0"_v,
        "WITH_ISO8859X=1"_v,
        "WITH_ISO8859X=1"_v,
        "WITH_ISO8859X=1"_v,
        "WITH_ISO8859X=1"_v,
        "WITH_THREADS=1"_v,
        "WITH_THREAD_ALLOC=0"_v
        ;
    libxml2.configureFile(
        libxml2.SourceDir / "include/libxml/xmlversion.h.in",
        libxml2.BinaryDir / "libxml/xmlversion.h");
    libxml2.writeFileOnce(libxml2.BinaryPrivateDir / "config.h",
        R"(
#ifdef _WIN32
#define _WINSOCKAPI_ 1
#define NEED_SOCKETS
#include <win32config.h>
#else

#define STDC_HEADERS 1

#define XML_SOCKLEN_T socklen_t

#define SEND_ARG2_CAST (const void *)
#define GETHOSTBYNAME_ARG_CAST (const char *)

#define VA_LIST_IS_ARRAY 1

#define SUPPORT_IP6 0

#endif
)"
);
}

void check(Checker &c)
{
    auto &s = c.addSet("libxml2");
    s.checkFunctionExists("class");
    s.checkFunctionExists("dlopen");
    s.checkFunctionExists("finite");
    s.checkFunctionExists("fpclass");
    s.checkFunctionExists("fprintf");
    s.checkFunctionExists("fp_class");
    s.checkFunctionExists("ftime");
    s.checkFunctionExists("getaddrinfo");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("isascii");
    s.checkFunctionExists("isinf");
    s.checkFunctionExists("isnan");
    s.checkFunctionExists("isnand");
    s.checkFunctionExists("localtime");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("munmap");
    s.checkFunctionExists("printf");
    s.checkFunctionExists("putenv");
    s.checkFunctionExists("rand");
    s.checkFunctionExists("rand_r");
    s.checkFunctionExists("signal");
    s.checkFunctionExists("stat");
    s.checkFunctionExists("strdup");
    s.checkFunctionExists("strerror");
    s.checkFunctionExists("strftime");
    s.checkFunctionExists("strndup");
    s.checkFunctionExists("time");
    //s.checkFunctionExists("va_copy");
    s.checkFunctionExists("vfprintf");
    s.checkFunctionExists("vsnprintf");
    s.checkFunctionExists("vsprintf");
    s.checkFunctionExists("_stat");
    //s.checkFunctionExists("__va_copy");
    s.checkIncludeExists("ansidecl.h");
    s.checkIncludeExists("arpa/inet.h");
    s.checkIncludeExists("arpa/nameser.h");
    s.checkIncludeExists("ctype.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("dl.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("float.h");
    s.checkIncludeExists("fp_class.h");
    s.checkIncludeExists("ieeefp.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("malloc.h");
    s.checkIncludeExists("math.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("nan.h");
    s.checkIncludeExists("ndir.h");
    s.checkIncludeExists("netdb.h");
    s.checkIncludeExists("netinet/in.h");
    s.checkIncludeExists("poll.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("resolv.h");
    s.checkIncludeExists("signal.h");
    s.checkIncludeExists("stdarg.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/dir.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("sys/ndir.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/timeb.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("time.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("winsock2.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("socklen_t");
    s.checkTypeSize("void *");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
}
