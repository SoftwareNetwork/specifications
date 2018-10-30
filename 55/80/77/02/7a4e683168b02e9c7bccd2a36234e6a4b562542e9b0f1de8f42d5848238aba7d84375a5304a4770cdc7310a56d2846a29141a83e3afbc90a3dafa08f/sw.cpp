#pragma sw require header org.sw.demo.qtproject.qt.base.widgets-5

void build(Solution &s)
{
    auto &qt = s.addProject("qtproject.qt", "5.11.2");

    auto &base = qt.addProject("base");
    base += Git("git://code.qt.io/qt/qtbase.git", "v{v}");

    auto &plugins = qt.addDirectory("plugins");

    auto &imageformats = plugins.addProject("imageformats");
    imageformats += Git("git://code.qt.io/qt/qtimageformats.git", "v{v}");

    {
        auto add_imageformat = [&s](auto &parent, const String &name) -> decltype(auto)
        {
            auto &t = parent.addLibrary(name);
            t.setOutputDir("plugins/imageformats");
            t.SourceDir /= "src/plugins/imageformats";
            t.SourceDir /= name;
            t += ".*\\.cpp"_rr, ".*\\.h"_rr;
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                t.Private += "UNICODE"_d;
            }
            t.Private += sw::Static, "QT_STATICPLUGIN"_d;

            automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, t);

            t.Public += "org.sw.demo.qtproject.qt.base.gui-5"_dep;
            return t;
        };

        add_imageformat(base, "gif");
        add_imageformat(base, "ico");
        add_imageformat(base, "jpeg") += "org.sw.demo.jpeg-*"_dep;

        add_imageformat(imageformats, "dds");
        add_imageformat(imageformats, "icns");
        add_imageformat(imageformats, "tga");
        add_imageformat(imageformats, "tiff") += "org.sw.demo.tiff-*"_dep;
        add_imageformat(imageformats, "wbmp");
        add_imageformat(imageformats, "webp") += "org.sw.demo.webmproject.webp-*"_dep;
        add_imageformat(imageformats, "jp2") += "org.sw.demo.mdadams.jasper-*"_dep;
        add_imageformat(imageformats, "mng") += "org.sw.demo.mng-*"_dep;
    }
}
