#pragma sw require header pub.egorpugin.primitives.tools.syncqt-master
#pragma sw require header org.sw.demo.qtproject.qt.base.widgets-5

void build(Solution &s)
{
    auto &qt = s.addProject("qtproject.qt", "5.12.0");

    auto &winextras = qt.addLibrary("winextras");
    winextras += Git("https://github.com/qt/qtwinextras.git", "v{v}");

    {
        String module = "QtWinExtras";
        syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, winextras, { module });
        auto &t = winextras;

        winextras.SourceDir /= "src/winextras";
        winextras += "[^/]*"_rr;
        winextras.Private += "QT_BUILD_WINEXTRAS_LIB"_d;
        winextras.Public += "org.sw.demo.qtproject.qt.base.gui-5"_dep;
        if (s.Settings.TargetOS.Type == OSType::Windows)
        {
            winextras.Private += "UNICODE"_d;
            winextras += "dwmapi.lib"_lib;
        }
        automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, winextras);
        t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
            #ifdef __cplusplus
            #include <QtCore/QtCore>
            #include <QtGui/QtGui>
            #endif
)xxx");
    }
}
