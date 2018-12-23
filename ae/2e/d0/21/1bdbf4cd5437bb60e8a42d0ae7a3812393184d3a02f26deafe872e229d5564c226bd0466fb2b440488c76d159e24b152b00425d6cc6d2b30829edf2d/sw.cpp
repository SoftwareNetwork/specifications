//#pragma sw require header pub.egorpugin.primitives.tools.syncqt-master
#pragma sw require header org.sw.demo.qtproject.qt.base.widgets-5

void build(Solution &s)
{
    auto &qt = s.addProject("qtproject.qt", "5.12.0");

    auto &quickcontrols = qt.addProject("quickcontrols");
    quickcontrols += Git("https://github.com/qt/qtquickcontrols", "v{v}");

    auto setup_output = [](auto &t, const String &out_dir, const String &proj_dir)
    {
        t.setOutputDir("qml/QtQuick/" + out_dir);

        auto qmldir = path(proj_dir) / "qmldir";
        t += qmldir;
        t.patch(qmldir, "plugin ", "# plugi n");

        auto settings = path(proj_dir) / (t.pkg.ppath.back() + ".pro");
        t += settings;

        if (t.DryRun)
            return;

        t.configureFile(qmldir, t.BinaryDir / qmldir, ConfigureFlags::CopyOnly);
        t.pushBackToFileOnce(t.BinaryDir / qmldir, "plugin " + t.getOutputFile().filename().stem().string());
        t += t.BinaryDir / qmldir;
        t[t.BinaryDir / qmldir].install_dir = ".";

        auto s = read_file(t.SourceDir / settings);
        static std::regex r("TARGETPATH\\s*=\\s*(\\S+)");
        std::smatch m;
        if (!std::regex_search(s, m, r))
            throw std::runtime_error("No .pro file found");
        auto mod = m[1].str();
        t.setOutputDir("qml/" + mod);
    };

    auto &controls = quickcontrols.addLibrary("controls");
    {
        controls += "src/controls/[^/]*\\.cpp"_rr;
        controls += "src/controls/[^/]*\\.h"_rr;
        controls += "src/controls/[^/]*\\.qml"_rr;
        controls += "src/controls/Private/.*"_rr;
        controls += "src/controls/Shaders/.*"_rr;
        controls += "src/controls/Styles/.*"_rr;
        controls.Public += "org.sw.demo.qtproject.qt.base.widgets-*"_dep;
        controls.Public += "org.sw.demo.qtproject.qt.declarative.quick-*"_dep;
        controls.Public += "org.sw.demo.qtproject.qt.declarative.qml.imports.layouts-*"_dep;
        controls.Public += "org.sw.demo.qtproject.qt.declarative.qml.imports.window-*"_dep;
        automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, controls);

        setup_output(controls, "Controls", "src/controls");
    }

    auto &dialogs = quickcontrols.addLibrary("dialogs");
    auto &dialogs_pvt = dialogs.addLibrary("Private");
    {
        dialogs_pvt += "src/dialogs/Private/.*"_rr;
        dialogs_pvt.Public += controls;
        automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, dialogs_pvt);

        setup_output(dialogs_pvt, "Dialogs/Private", "src/dialogs/Private");
    }

    {
        dialogs += "src/dialogs/[^/]*\\.cpp"_rr;
        dialogs += "src/dialogs/[^/]*\\.h"_rr;
        dialogs += "src/dialogs/[^/]*\\.qml"_rr;
        dialogs += "src/dialogs/images/.*"_rr;
        dialogs += "src/dialogs/qml/.*"_rr;
        dialogs.Public += dialogs_pvt;
        automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, dialogs);

        setup_output(dialogs, "Dialogs", "src/dialogs");
    }

    auto &widgets = quickcontrols.addLibrary("widgets");
    {
        widgets += "src/widgets/[^/]*\\.cpp"_rr;
        widgets += "src/widgets/[^/]*\\.h"_rr;
        widgets += "src/widgets/[^/]*\\.qml"_rr;
        widgets +=
            "src/dialogs/qquickabstractdialog.cpp",
            "src/dialogs/qquickabstractdialog_p.h",
            "src/dialogs/qquickabstractmessagedialog_p.h",
            "src/dialogs/qquickabstractmessagedialog.cpp",
            "src/dialogs/qquickabstractfiledialog.cpp",
            "src/dialogs/qquickabstractfiledialog_p.h",
            "src/dialogs/qquickabstractcolordialog.cpp",
            "src/dialogs/qquickabstractcolordialog_p.h",
            "src/dialogs/qquickabstractfontdialog.cpp",
            "src/dialogs/qquickabstractfontdialog_p.h";

        widgets.Public += dialogs;
        automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, widgets);

        setup_output(widgets, "PrivateWidgets", "src/widgets");
    }
}
