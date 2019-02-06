void build(Solution &s)
{
    auto &poco = s.addProject("poco", "1.9.0");
    poco += Git("https://github.com/pocoproject/poco", "poco-{v}-release");

    auto add_library = [&s, &poco](const String &name, String dir = {}) -> decltype(auto)
    {
        if (dir.empty())
            dir = name;
        auto &t = poco.addTarget<LibraryTarget>(name);
        t.setRootDirectory(dir);
        t +=
            FileRegex("include", ".*", true),
            FileRegex("src", ".*", true);
        t -= "src/.*_.*"_rr;
        t.Public += "POCO_NO_AUTOMATIC_LIBS"_d;
        t.Public += "POCO_UNBUNDLED"_d;
        t.Private += sw::Shared, Definition(name + "_EXPORTS");
        t.Public += sw::Shared, "POCO_DLL"_d;
        t.Public += sw::Static, "POCO_STATIC"_d;
        return t;
    };

    auto &foundation = add_library("Foundation");
    foundation ^= "src/pcre_.*"_rr;
    foundation -= "src/.*\\.cc"_rr;
    foundation.patch("src/Unicode.cpp", "_pcre_ucp_gentype", "pcre_ucp_gentype");

    if (s.Settings.TargetOS.is(OSType::Windows))
    {
        foundation -= "src/SyslogChannel.cpp";

        auto c = foundation.addCommand();
        c << (s.Settings.Native.SDK.getPath("bin") / toStringWindows(s.Settings.TargetOS.Arch) / "mc.exe")
            << "-h" << foundation.BinaryDir
            << "-r" << foundation.BinaryDir
            << cmd::in("src/pocomsg.mc")
            << cmd::end()
            << cmd::out("pocomsg.h")
            ;

        foundation += "advapi32.lib"_slib;
        foundation += "Iphlpapi.lib"_slib;
    }

    foundation.Public += "org.sw.demo.madler.zlib-1"_dep;
    foundation.Public += "org.sw.demo.pcre.pcre8-8"_dep;

    auto &data = add_library("Data");
    data.Public += foundation;

    auto &xml = add_library("XML");
    xml.Public += foundation;
    xml.Public += "org.sw.demo.expat-2"_dep;
    xml -= "src/xml[[:lower:]].*"_r;

    auto &json = add_library("JSON");
    json.Public += foundation;
    if (s.Settings.Native.CompilerType == CompilerType::MSVC)
    {
        for (auto *f : json.gatherSourceFiles())
            f->BuildAs = NativeSourceFile::CPP;
    }

    auto &util = add_library("Util");
    util.Public += json, xml;
    if (s.Settings.TargetOS.is(OSType::Windows))
        util.Protected += "advapi32.lib"_slib;
    else
        util -= "src/Win.*"_rr;

    auto &crypto = add_library("Crypto");
    crypto.Public += util, "org.sw.demo.openssl.ssl-1"_dep;

    auto &net = add_library("Net");
    net.Public += util;
    if (s.Settings.TargetOS.is(OSType::Windows))
        net.Protected += "ws2_32.lib"_slib, "iphlpapi.lib"_slib;

    auto &net_ssl = add_library("NetSSL", "NetSSL_OpenSSL");
    net_ssl.Public += net, crypto;

    auto &net_win = add_library("NetSSL_Win");
    net_win.Public += net;
    if (s.Settings.TargetOS.is(OSType::Windows))
        net_win.Protected += "Crypt32.lib"_slib;
    else
        net_win.HeaderOnly = true; // exclude from building on *nix

    auto &sqlite = add_library("SQLite", "Data/SQLite");
    sqlite -= "src/sqlite3.c";
    sqlite.Public += data, "org.sw.demo.sqlite3-*"_dep;

    auto &odbc = add_library("ODBC", "Data/ODBC");
    odbc.Public += data;
    if (s.Settings.TargetOS.is(OSType::Windows))
        odbc.Protected += "odbc32.lib"_slib;

    auto &zip = add_library("Zip");
    zip.Public += util;

    auto &cppparser = add_library("CppParser");
    cppparser.Public += foundation;

    auto &cppunit = add_library("CppUnit");

    auto &enc = add_library("Encodings");
    enc.Public += foundation;
    enc += "src/.*_.*"_rr;

    auto &pc = add_library("PageCompiler");
    pc.Public += net;

    /*auto &pdf = add_library("PDF");
    pdf.Public += util, "org.sw.demo.libharu-2"_dep;
    pdf ^= "src/[[:lower:]].*"_rr;*/

    auto &doc = add_library("PocoDoc");
    doc.Public += util, cppparser;

    auto &_7zip = add_library("SevenZip");
    _7zip.Public += util;
}
