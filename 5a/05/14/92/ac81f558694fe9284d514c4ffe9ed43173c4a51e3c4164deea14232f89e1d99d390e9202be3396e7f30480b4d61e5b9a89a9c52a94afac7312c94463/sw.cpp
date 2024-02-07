void build(Solution &s)
{
    auto &poco = s.addProject("poco", "1.13.1");
    poco += Git("https://github.com/pocoproject/poco", "poco-{v}-release");

    auto setup_library = [&s, &poco](auto &t, const String &name, String dir = {})
    {
        t.setRootDirectory(dir);
        t.AllowEmptyRegexes = true;
        t +=
            FileRegex("include", ".*", true),
            FileRegex("src", ".*", true);
        t -= "src/.*_.*"_rr;
        t.AllowEmptyRegexes = false;
        t.Public += "POCO_NO_AUTOMATIC_LIBS"_d;
        t.Public += "POCO_UNBUNDLED"_d;
        t.Private += sw::Shared, Definition(name + "_EXPORTS");
        t.Public += sw::Shared, "POCO_DLL"_d;
        t.Public += sw::Static, "POCO_STATIC"_d;
    };

    auto add_library = [&s, &poco, &setup_library](const String &name, String dir = {}) -> decltype(auto)
    {
        if (dir.empty())
            dir = name;
        auto &t = poco.addTarget<LibraryTarget>(name);
        setup_library(t, name, dir);
        return t;
    };

    auto &foundation = add_library("Foundation");
    {
        foundation -= "src/.*\\.cc"_rr;
        foundation.Public += "POCO_HAVE_STD_ATOMICS"_def;

        if (foundation.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            foundation -= "src/SyslogChannel.cpp";

            auto c = foundation.addCommand();
            c << cmd::prog("com.Microsoft.Windows.mc"_dep)
                << "-h" << foundation.BinaryDir
                << "-r" << foundation.BinaryDir
                << cmd::in("src/pocomsg.mc")
                << cmd::end()
                << cmd::out("pocomsg.h")
                ;

            foundation += "advapi32.lib"_slib;
            foundation += "Iphlpapi.lib"_slib;
        }

        foundation.Public += "org.sw.demo.madler.zlib"_dep;
        foundation += "org.sw.demo.pcre.pcre8-10"_dep;
        foundation.patch("src/Unicode.cpp", "_pcre2_ucp", "pcre2_ucp");
        foundation.patch("src/Unicode.cpp", "_pcre2_ucd", "pcre2_ucd");
    }

    auto &data = add_library("Data");
    data.Public += foundation;

    auto &xml = add_library("XML");
    xml.Public += foundation;
    xml.Public += "org.sw.demo.expat"_dep;
    xml -= "src/xml[[:lower:]].*"_r;

    auto &json = add_library("JSON");
    json.Public += foundation;
    if (json.getCompilerType() == CompilerType::MSVC)
    {
        for (auto *f : json.gatherSourceFiles())
            f->BuildAs = NativeSourceFile::CPP;
    }

    auto &util = add_library("Util");
    util.Public += json, xml;
    if (util.getBuildSettings().TargetOS.is(OSType::Windows))
        util.Protected += "advapi32.lib"_slib;
    else
        util -= "src/Win.*"_rr;

    auto &crypto = add_library("Crypto");
    crypto.Public += util, "org.sw.demo.openssl.ssl"_dep;

    auto &net = add_library("Net");
    net.Public += util;
    if (net.getBuildSettings().TargetOS.is(OSType::Windows))
        net.Protected += "ws2_32.lib"_slib, "iphlpapi.lib"_slib;

    auto &net_ssl = add_library("NetSSL", "NetSSL_OpenSSL");
    net_ssl.Public += net, crypto;

    auto &net_win = add_library("NetSSL_Win");
    net_win.Public += net;
    if (net_win.getBuildSettings().TargetOS.is(OSType::Windows))
        net_win.Protected += "Crypt32.lib"_slib;
    else
        net_win.HeaderOnly = true; // exclude from building on *nix

    auto &sqlite = add_library("SQLite", "Data/SQLite");
    sqlite -= "src/sqlite3.c";
    sqlite.Public += data, "org.sw.demo.sqlite3"_dep;

    auto &odbc = add_library("ODBC", "Data/ODBC");
    odbc.Public += data;
    if (odbc.getBuildSettings().TargetOS.is(OSType::Windows))
        odbc.Protected += "odbc32.lib"_slib;

    auto &zip = add_library("Zip");
    zip.Public += util;

    auto &cppparser = add_library("CppParser");
    cppparser.Public += foundation;

    auto &cppunit = add_library("CppUnit");
    cppunit += foundation;

    auto &enc = add_library("Encodings");
    enc.Public += foundation;
    enc += "src/.*_.*"_rr;

    /*auto &pdf = add_library("PDF");
    pdf.Public += util, "org.sw.demo.libharu"_dep;
    pdf ^= "src/[[:lower:]].*"_rr;*/

    auto &_7zip = add_library("SevenZip");
    _7zip.Public += util;

    auto &pc = poco.addTarget<StaticLibraryTarget>("PageCompiler");
    setup_library(pc, "PageCompiler", "PageCompiler");
    pc.Public += net;

    auto &doc = poco.addTarget<StaticLibraryTarget>("PocoDoc");
    setup_library(doc, "PocoDoc", "PocoDoc");
    doc.Public += util, cppparser;
}
