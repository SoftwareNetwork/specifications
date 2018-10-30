#pragma sw require header pub.egorpugin.primitives.tools.syncqt-master
#pragma sw require header org.sw.demo.qtproject.qt.base.widgets-5

void build(Solution &s)
{
    auto &qt = s.addProject("qtproject.qt", "5.11.2");

    auto &svg = qt.addLibrary("svg");
    svg += Git("git://code.qt.io/qt/qtsvg.git", "v{v}");

    {
        String module = "QtSvg";
        syncqt(svg, { module });
        auto &t = svg;

        svg += "src/svg/[^/]*"_rr;
        svg.Private += "QT_BUILD_SVG_LIB"_d;
        svg.Public += "org.sw.demo.qtproject.qt.base.widgets-5"_dep;
        if (s.Settings.TargetOS.Type == OSType::Windows)
        {
            svg.Private += "UNICODE"_d;
        }
        automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, svg);
        t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
            #ifdef __cplusplus
            #include <QtCore/QtCore>
            #include <QtGui/QtGui>
            #include <QtWidgets/QtWidgets>
            #endif
)xxx");
    }
}
