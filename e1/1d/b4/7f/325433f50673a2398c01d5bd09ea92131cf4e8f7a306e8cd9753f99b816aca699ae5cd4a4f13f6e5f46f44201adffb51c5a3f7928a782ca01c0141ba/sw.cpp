#pragma sw require pub.egorpugin.primitives.filesystem-master

static int wt_preprocess_file(path i, path o, String name)
{
    auto preprocess_file = [](const String &s)
    {
        String o;
        int i = 0;
        for (auto &c : s)
        {
            String h(2, 0);
            sprintf(&h[0], "%02x", c);
            o += "0x" + h + ",";
            if (++i % 25 == 0)
                o += "\n";
            else
                o += " ";
        }
        o += "0x00,";
        if (++i % 25 == 0)
            o += "\n";
        return o;
    };

    auto s = read_file(i);
    std::string out;
    out += "#include <string>\n#include <vector>\nnamespace skeletons {\nstatic const char s[] {";
    out += preprocess_file(s);
    out += "};\nconst char *" + name + "1 = s;\n";
    out += "std::vector<const char *> " + name + "()\n{\nreturn { " + name + "1 };\n}\n}";
    write_file(o, out);

    return 0;
}

SW_DEFINE_VISIBLE_FUNCTION_JUMPPAD(wt_preprocess_file, wt_preprocess_file)

void build(Solution &s)
{
    auto &p = s.addProject("emweb.wt", "4.0.5");
    p += Git("https://github.com/emweb/wt", "{v}");
    auto &wt = p.addTarget<LibraryTarget>("wt");

    wt +=
        "WConfig.h.in",
        "src/3rdparty/rapidxml/.*\\.hpp"_rr,
        "src/Wt/.*"_rr,
        "src/Wt/Dbo/.*"_rr,
        "src/filetostring.cmake",
        "src/js/.*"_rr,
        "src/web/.*"_rr,
        "src/xml/.*"_rr;

    wt -=
        "src/Wt/Chart/scripts/.*"_rr,
        "src/Wt/Dbo/.*"_rr,
        "src/Wt/Date/.*"_rr,
        "src/Wt/FontSupport.*"_rr,
        "src/Wt/WRasterImage-d2d1.C",
        "src/Wt/WRasterImage-skia.C";

    //wt += "src/Wt/Date/include"_idir;
    wt.Public +=
        "src/web"_id,
        "src"_id;

    //wt.Public += "io_service=io_context"_d;
    wt.Public += "_WIN32_WINNT=0x0601"_d;

    wt += "src/Wt/FontSupportPango.C";

    wt.Private += "WT_BUILDING"_d;
    wt.Public += "WT_FONTSUPPORT_PANGO"_d;
    wt.Private += sw::Shared, "wt_EXPORTS"_d;
    wt.Private += sw::Shared, "wttest_EXPORTS"_d;
    wt.Public += sw::Static, "WT_STATIC"_d;

    wt.Public += "org.sw.demo.boost.multi_index-1"_dep;
    wt.Public += "org.sw.demo.boost.asio-1"_dep;
    wt.Public += "org.sw.demo.boost.date_time-1"_dep;
    wt.Public += "org.sw.demo.boost.filesystem-1"_dep;
    wt.Public += "org.sw.demo.boost.interprocess-1"_dep;
    wt.Public += "org.sw.demo.boost.ublas-1"_dep;
    wt.Public += "org.sw.demo.boost.numeric-1"_dep;
    wt.Public += "org.sw.demo.boost.thread-1"_dep;
    wt.Public += "org.sw.demo.gnome.pango.pangoft2-1"_dep;
    wt.Public += "org.sw.demo.glew-2"_dep;
    wt.Public += "org.sw.demo.graphicsmagick.magick-1"_dep;
    wt.Public += "org.sw.demo.libharu-2"_dep;
    wt.Public += "org.sw.demo.openssl.ssl-1.*.*.*"_dep;
    wt.Public += "org.sw.demo.howardhinnant.date.date_full-2"_dep;

    if (wt.getSettings().TargetOS.Type == OSType::Windows)
        wt.Public += "Ole32.lib"_lib, "Shell32.lib"_lib;

    auto file2string = [&wt](path i, const std::string &name)
    {
        i = wt.SourceDir / i;
        const auto o = wt.BinaryDir / (name + ".C");
        SW_MAKE_EXECUTE_BUILTIN_COMMAND_AND_ADD(c, wt, "wt_preprocess_file", &wt_preprocess_file);
        c->args.push_back(i.u8string());
        c->args.push_back(o.u8string());
        c->args.push_back(name);
        c->addInput(i);
        c->addOutput(o);
        wt += o;
    };

    file2string("src/web/skeleton/Plain.html", "Plain_html");
    file2string("src/web/skeleton/Hybrid.html", "Hybrid_html");
    file2string("src/web/skeleton/Boot.html", "Boot_html");

    if (wt.getSettings().Native.ConfigurationType == ConfigurationType::Debug)
    {
        file2string("src/web/skeleton/Wt.js", "Wt_js");
        file2string("src/web/skeleton/Boot.js", "Boot_js");
        file2string("src/js/WCartesianChart.js", "WCartesianChart_js");
    }
    else
    {
        file2string("src/web/skeleton/Wt.min.js", "Wt_js");
        file2string("src/web/skeleton/Boot.min.js", "Boot_js");
        file2string("src/js/WCartesianChart.min.js", "WCartesianChart_js");
    }

    file2string("src/web/skeleton/jquery.min.js", "JQuery_js");

    file2string("src/xml/wt.xml", "Wt_xml");
    file2string("src/xml/auth_strings.xml", "AuthStrings_xml");
    file2string("src/xml/bootstrap_theme.xml", "BootstrapTheme_xml");
    file2string("src/xml/bootstrap3_theme.xml", "Bootstrap3Theme_xml");
    file2string("src/xml/auth_css_theme.xml", "AuthCssTheme_xml");

    wt.Variables["WT_HAS_WRASTERIMAGE"] = "1";
    wt.Variables["WT_HAS_WPDFIMAGE"] = "1";
    wt.Variables["WT_WITH_SSL"] = "1";

    wt.Variables["WT_USE_OPENGL"] = "1";
    wt.Variables["WT_DEBUG_ENABLED"] = "0";
    wt.Variables["WT_THREADED"] = "1";

    wt.Variables["WT_ASIO_IS_BOOST_ASIO"] = "1";

    wt.Variables["VERSION_SERIES"] = wt.Variables["PACKAGE_VERSION_MAJOR"];
    wt.Variables["VERSION_MAJOR"] = wt.Variables["PACKAGE_VERSION_MINOR"];
    wt.Variables["VERSION_MINOR"] = wt.Variables["PACKAGE_VERSION_PATCH"];

    wt.Variables["VERSION_MINOR"] = wt.Variables["PACKAGE_VERSION_PATCH"];

    std::string cfg = ".";
    if (wt.getSettings().TargetOS.Type != OSType::Windows)
        cfg = "/etc/wt";

    wt.Variables["CONFIGURATION"] = cfg + "/wt_config.xml";
    wt.Variables["CONFIGURATION"] = cfg + "/wthttpd";

    wt.configureFile("WConfig.h.in", "Wt/WConfig.h");

    //
    auto &http = p.addTarget<LibraryTarget>("http");

    http +=
        "src/http/.*"_rr;

    http.Public +=
        "src"_id;

    http.Private += "HTTP_WITH_SSL"_d;
    http.Private += "WT_BUILDING"_d;
    http.Private += "_WIN32_WINNT=0x0601"_d;
    http.Public += "WTHTTP_WITH_ZLIB"_d;
    http.Private += sw::Shared, "wthttp_EXPORTS"_d;
    http.Public += sw::Static, "WTHTTP_STATIC"_d;

    http.Public += "org.sw.demo.boost.program_options-1"_dep;
    http.Public += "org.sw.demo.boost.spirit-1"_dep;
    http.Public += "org.sw.demo.madler.zlib-1"_dep;
    http.Public += wt;

    //
    auto &dbo = p.addTarget<LibraryTarget>("dbo");

    dbo +=
        "src/Wt/Dbo/.*"_rr,
        "src/Wt/WStringStream.*"_rr,
        "src/web/EscapeOStream.*"_rr;

    dbo -=
        "src/Wt/Dbo/backend/.*"_rr,
        "src/Wt/WStringStream.*"_rr,
        "src/web/.*"_rr;

    dbo.Public +=
        "src"_id;

    dbo.Private += "WT_BUILDING"_d;
    dbo.Private += sw::Shared, "wtdbo_EXPORTS"_d;
    dbo.Public += sw::Static, "WTDBO_STATIC"_d;

    dbo.Public += wt;

    //
    auto &sqlite3 = dbo.addTarget<LibraryTarget>("backend.sqlite3");

    sqlite3 +=
        "src/Wt/Dbo/backend/.*"_rr;
    sqlite3 -=
        "src/Wt/Dbo/backend/.*"_rr;
    sqlite3 += "src/Wt/Dbo/backend/Sqlite3.C";

    sqlite3.Public +=
        "src"_id;

    sqlite3.Private += "WT_BUILDING"_d;
    sqlite3.Private += sw::Shared, "wtdbosqlite3_EXPORTS"_d;
    sqlite3.Public += sw::Static, "WTDBOSQLITE3_STATIC"_d;

    sqlite3.Public += dbo;
    sqlite3.Public += "org.sw.demo.sqlite3-3"_dep;
}
