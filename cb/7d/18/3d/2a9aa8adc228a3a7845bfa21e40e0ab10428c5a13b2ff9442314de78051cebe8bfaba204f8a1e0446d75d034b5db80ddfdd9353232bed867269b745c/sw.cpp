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
    out += "};\n";
    out += "const char *" + name + " = s;\n";
    out += "}\n";
    write_file(o, out);

    return 0;
}
SW_DEFINE_VISIBLE_FUNCTION_JUMPPAD2(wt_preprocess_file)

void build(Solution &s)
{
    auto &p = s.addProject("emweb.wt", "4.11.4");
    p += Git("https://github.com/emweb/wt");

    constexpr auto cppstd = cpp20;

    auto &wt = p.addTarget<LibraryTarget>("wt");
    {
        wt += cppstd;

        wt +=
            "WConfig.h.in",
            "src/thirdparty/rapidxml/.*\\.hpp"_rr,
            "src/thirdparty/qrcodegen/.*\\.[ch]pp"_rr,
            "src/Wt/.*"_rr,
            "src/Wt/Dbo/.*"_rr,
            "src/js/.*"_rr,
            "src/web/.*"_rr,
            "src/xml/.*"_rr;

        wt -=
            "src/Wt/Chart/scripts/.*"_rr,
            "src/Wt/Auth/Saml/.*"_rr,
            "src/Wt/Dbo/.*"_rr,
            "src/Wt/Date/.*"_rr,
            "src/Wt/Test/.*"_rr,
            "src/Wt/FontSupport.*"_rr,
            "src/Wt/WRasterImage-d2d1.C"
            ;

        //wt += "src/Wt/Date/include"_idir;
        wt.Public +=
            "src/web"_id,
            "src"_id;

        //wt.Public += "_WIN32_WINNT=0x0601"_d;

        wt += "src/Wt/FontSupportPango.C";
        /*wt -= "src/Wt/WGLWidget.C";
        wt -= "src/Wt/WServerGLWidget.C";
        if (wt.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            wt += "src/Wt/WGLWidget.C";
            wt += "src/Wt/WServerGLWidget.C";
        }*/


        wt.Private += "WT_BUILDING"_d;
        wt.Public += "WT_FONTSUPPORT_PANGO"_d;
        wt.Private += sw::Shared, "wt_EXPORTS"_d;
        wt.Public += sw::Static, "WT_STATIC"_d;
        // turn on when gcc catch up
        //wt.Public += "WT_CPP20_DATE_TZ_IMPLEMENTATION=std"_d;

        //wt += "OpenGL"_framework;

        wt.Variables["WT_ASIO_IS_BOOST_ASIO"] = 1;
        wt.Public += "org.sw.demo.boost.asio"_dep;

        wt.Public += "org.sw.demo.boost.math"_dep;
        wt.Public += "org.sw.demo.boost.multi_index"_dep;
        wt.Public += "org.sw.demo.boost.date_time"_dep;
        wt.Public += "org.sw.demo.boost.filesystem"_dep;
        wt.Public += "org.sw.demo.boost.interprocess"_dep;
        wt.Public += "org.sw.demo.boost.ublas"_dep;
        wt.Public += "org.sw.demo.boost.numeric"_dep;
        wt.Public += "org.sw.demo.boost.thread"_dep;
        wt.Public += "org.sw.demo.gnome.pango.pangoft2"_dep;
        //wt.Public -= "org.sw.demo.glew"_dep;
        //if (wt.getBuildSettings().TargetOS.Type == OSType::Windows)
            wt.Public += "org.sw.demo.glew"_dep;
        wt.Public += "org.sw.demo.graphicsmagick.magick"_dep;
        wt.Public += "org.sw.demo.libharu"_dep;
        wt.Public += "org.sw.demo.openssl.ssl"_dep;
        wt.Public += "org.sw.demo.howardhinnant.date.date_full"_dep;

        if (wt.getBuildSettings().TargetOS.Type == OSType::Windows) {
            wt.Private += "WIN32_LEAN_AND_MEAN"_d;
            wt.Public += "Ole32.lib"_slib, "Shell32.lib"_slib;
        }

        auto file2string = [&wt](path i, const std::string &name)
        {
            auto c = wt.addCommand(SW_VISIBLE_FUNCTION(wt_preprocess_file));
            c << cmd::in(i);
            c << cmd::out(name + ".C");
            c << name;
        };

        file2string("src/web/skeleton/Plain.html", "Plain_html");
        file2string("src/web/skeleton/Hybrid.html", "Hybrid_html");
        file2string("src/web/skeleton/Boot.html", "Boot_html");

        if (wt.getBuildSettings().Native.ConfigurationType == ConfigurationType::Debug)
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

        file2string("src/xml/wt.xml", "Wt_xml");
        file2string("src/xml/auth_strings.xml", "AuthStrings_xml");
        file2string("src/xml/bootstrap_theme.xml", "BootstrapTheme_xml");
        file2string("src/xml/bootstrap3_theme.xml", "Bootstrap3Theme_xml");
        file2string("src/xml/bootstrap5_theme.xml", "Bootstrap5Theme_xml");
        file2string("src/xml/auth_css_theme.xml", "AuthCssTheme_xml");

        wt.Variables["WT_HAS_WRASTERIMAGE"] = "1";
        wt.Variables["WT_HAS_WPDFIMAGE"] = "1";
        wt.Variables["WT_WITH_SSL"] = "1";

        //if (wt.getBuildSettings().TargetOS.Type == OSType::Windows)
            wt.Variables["WT_USE_OPENGL"] = "1";
        wt.Variables["WT_DEBUG_ENABLED"] = "0";
        wt.Variables["WT_THREADED"] = "1";

        wt.Variables["WT_ANY_IS_STD_ANY"] = 1;
        wt.Variables["WT_DATE_TZ_USE_DATE"] = 1;

        wt.Variables["VERSION_SERIES"] = wt.Variables["PACKAGE_VERSION_MAJOR"];
        wt.Variables["VERSION_MAJOR"] = wt.Variables["PACKAGE_VERSION_MINOR"];
        wt.Variables["VERSION_MINOR"] = wt.Variables["PACKAGE_VERSION_PATCH"];

        wt.Variables["VERSION_MINOR"] = wt.Variables["PACKAGE_VERSION_PATCH"];

        std::string cfg = ".";
        if (wt.getBuildSettings().TargetOS.Type != OSType::Windows)
            cfg = "/etc/wt";

        wt.Variables["CONFIGURATION"] = cfg + "/wt_config.xml";
        wt.Variables["CONFIGURATION"] = cfg + "/wthttpd";

        wt.configureFile("WConfig.h.in", "Wt/WConfig.h");

        wt.patch("src/web/WebUtils.h", "#include \"thirdparty/rapidxml/rapidxml.hpp\"", "//# include \"thirdparty/rapidxml/rapidxml.hpp\"");
        wt.patch("src/web/WebUtils.h", "namespace Wt {", "namespace Wt  { namespace rapidxml { template<class Ch> class xml_node; }");

        wt.patch("src/Wt/WServerGLWidget.C", "#include <GL/gl.h>", R"(
            #ifndef APPLE_GL
            #include <GL/gl.h>
            #endif
        )");

        wt.patch("src/web/FileUtils.C",
            "#include <fstream>",
            "#include <filesystem>\n#include <fstream>"
        );
        wt.patch("src/web/FileUtils.C",
            "boost::filesystem::directory_iterator end_itr;",
            ""
        );
        wt.patch("src/web/FileUtils.C",
            "for (boost::filesystem::directory_iterator i(path); i != end_itr; ++i) {",
            "for (auto &&p : std::filesystem::directory_iterator{directory}) {"
        );
        wt.patch("src/web/FileUtils.C",
            "std::string f = (*i).path().string();",
            "std::string f = p.path().string();"
        );
        wt.patch("src/Wt/WRasterImage-gm.C",
            "strncpy(info.filename, imagePath.c_str(), 2048);",
            "strncpy(info.filename, imagePath.string().c_str(), 2048);"
        );
    }

    auto &http = p.addTarget<LibraryTarget>("http");
    {
        http += cppstd;

        http +=
            "src/http/.*"_rr;

        http.Public +=
            "src"_id;

        http.Private += "HTTP_WITH_SSL"_d;
        http.Private += "WT_BUILDING"_d;
        http.Public += "WTHTTP_WITH_ZLIB"_d;
        http.Private += sw::Shared, "wthttp_EXPORTS"_d;
        http.Public += sw::Static, "WTHTTP_STATIC"_d;

        if (http.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            //http.Private += "_WIN32_WINNT=0x0601"_d;
            http.Private += "WIN32_LEAN_AND_MEAN"_d;
            http.Private += "NOMINMAX"_d;
        }

        http.Public += "org.sw.demo.boost.program_options"_dep;
        http.Public += "org.sw.demo.boost.spirit"_dep;
        http.Public += "org.sw.demo.madler.zlib"_dep;
        http.Public += wt;

        http.patch("src/http/Server.C",
            "query = Wt::AsioWrapper::asio::ip::tcp::resolver::query(Wt::AsioWrapper::asio::ip::tcp::v6(), address, \"http\");",
            "auto query2 = Wt::AsioWrapper::asio::ip::tcp::resolver::query(Wt::AsioWrapper::asio::ip::tcp::v6(), address, \"http\");"
        );
        http.patch("src/http/Server.C",
            "for (Wt::AsioWrapper::asio::ip::tcp::resolver::iterator it = resolver.resolve(query, errc);",
            "for (Wt::AsioWrapper::asio::ip::tcp::resolver::iterator it = resolver.resolve(query2, errc);"
        );
    }

    auto &dbo = p.addTarget<LibraryTarget>("dbo");
    {
        dbo += cppstd;

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
    }

    auto &sqlite3 = dbo.addTarget<LibraryTarget>("backend.sqlite3");
    {
        sqlite3 += cppstd;

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
        sqlite3.Public += "org.sw.demo.sqlite3"_dep;
    }

    auto &resources = p.addTarget<FilesTarget>("resources");
    resources += "resources/.*"_rr;
}
