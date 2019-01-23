#pragma sw require header org.sw.demo.qtproject.qt.base.widgets-5

void build(Solution &s)
{
    auto &qt = s.addProject("qtproject.qt", "5.12.0");

    auto &plugins = qt.addDirectory("plugins");

    auto &imageformats = plugins.addProject("imageformats");
    imageformats += Git("git://code.qt.io/qt/qtimageformats.git", "v{v}");

    {
        auto add_imageformat1 = [&s](auto &t, const String &name)
        {
            t.setOutputDir("plugins/imageformats");
            t.SourceDir /= "src/plugins/imageformats";
            t.SourceDir /= name;
            t += ".*\\.cpp"_rr, ".*\\.h"_rr, ".*\\.json"_rr;
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                t.Private += "UNICODE"_d;
            }
            t.Private += sw::Static, "QT_STATICPLUGIN"_d;

            automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, t);

            t.Public += "org.sw.demo.qtproject.qt.base.gui-5"_dep;
        };

        auto add_imageformat = [&s, &imageformats, &add_imageformat1](const String &name) -> decltype(auto)
        {
            auto &t = imageformats.addLibrary("q" + name);
            add_imageformat1(t, name);
            return t;
        };

        auto add_imageformat_base = [&imageformats, &add_imageformat1](const String &name) -> decltype(auto)
        {
            auto &t = imageformats.addLibrary("q" + name);
            t += Git("git://code.qt.io/qt/qtbase.git", "v{v}");
            add_imageformat1(t, name);
            return t;
        };

        add_imageformat_base("gif");
        add_imageformat_base("ico");
        add_imageformat_base("jpeg") += "org.sw.demo.jpeg-*"_dep;

        add_imageformat("dds");
        add_imageformat("icns");
        add_imageformat("tga");
        add_imageformat("tiff") += "org.sw.demo.tiff-*"_dep;
        add_imageformat("wbmp");
        add_imageformat("webp") += "org.sw.demo.webmproject.webp-*"_dep;
        add_imageformat("jp2") += "org.sw.demo.mdadams.jasper-*"_dep;
        add_imageformat("mng") += "org.sw.demo.mng-*"_dep;
    }
}
