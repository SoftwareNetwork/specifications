#pragma once

struct QtFeature
{
    String name;
    bool enabled = false;
    Strings ifdefs; // or!

    bool operator==(const QtFeature &rhs) const { return name == rhs.name; }

    bool operator<(const QtFeature &rhs) const { return name < rhs.name; }

    String print() const
    {
        String s;
        if (!ifdefs.empty()) {
            s += "#if ";
            for (auto &d : ifdefs)
                s += "defined(" + d + ") || ";
            s.resize(s.size() - 4);
            s += "\n";
        }
        s += "#define QT_FEATURE_" + name + " " + (enabled ? "1" : "-1") + "\n";
        if (!ifdefs.empty())
            s += "#endif\n";
        return s;
    }
};

struct QtFeatures : std::set<QtFeature>
{
    using std::set<QtFeature>::set;

    QtFeature &operator[](const String &k)
    {
        auto i = find({ k });
        if (i != end())
            return (QtFeature &)*i;
        return (QtFeature &)*insert({ k }).first;
    }
};

struct QtConfig
{
    QtFeatures features;
    StringMap<String> definitions;

    String print() const
    {
        String s;
        for (auto &[k, v] : definitions)
            s += "#define " + k + " " + v + "\n";
        s += "\n";
        for (auto &f : features)
            s += f.print();
        return s;
    }
};

struct QtLibraryConfig
{
    QtConfig public_;
    QtConfig private_;
};

struct QtLibrary
{
    String name;
    QtLibraryConfig config;
    Strings dependencies;

    void print(NativeExecutedTarget &t)
    {
        if (t.getBuildSettings().Native.LibrariesType == LibraryType::Static)
            config.public_.features["static"].enabled = true;
        config.public_.features["shared"].enabled = !config.public_.features["static"].enabled;

        auto name_lower = boost::to_lower_copy(name);
        auto actual_name = name;

        auto config_fn = name_lower;
        auto private_config_fn = name_lower;

        if (name == "q") {
            actual_name = "QtCore";
            t.writeFileOnce(actual_name + "/QtConfig", "#include \"" + config_fn + "\"");
        } else {
            config_fn += "-";
            private_config_fn += "-";
        }

        config_fn += "config.h";
        private_config_fn += "config_p.h";

        t.writeFileOnce(actual_name + "/" + config_fn, config.public_.print());
        t.writeFileOnce(actual_name + "/private/" + private_config_fn, config.private_.print());

        t.writeFileOnce(actual_name + "/" + actual_name + "Depends", printDependencies());
    }

    String printDependencies() const
    {
        String s;
        s += "#ifdef __cplusplus\n";
        for (auto &d : dependencies)
            s += "#include <" + d + "/" + d + ">\n";
        s += "#endif\n";
        return s;
    }
};

static QtLibrary qt_desc {
    "q",
    // config
    {
            // public
            {
                    // features
                    {
                            { "accessibility", true },
                            { "build_all", true },
                            { "c__11", true },
                            { "c__14", true },
                            { "c__1z", false },
                            { "concurrent", true },
                            { "cxx11_future", true },
                            { "datetimeparser", true },
                            { "debug_and_release", true },
                            { "force_asserts", false },
                            { "framework", false },
                            { "future", true },
                            { "getentropy", false },
                            { "journald", false },
                            { "pkg_config", false },
                            { "processenvironment", true },
                            { "rpath", false },
                            { "separate_debug_info", false },
                            { "shared", false },
                            { "simulator_and_device", false },
                            { "slog2", false },
                            { "static", false },
                            { "std_atomic64", true },
                            { "syslog", false },
                            { "thread", true },
                    },
                    // defs
                    {
                            { "QT_COMPILER_SUPPORTS_SSE2", "1" },
                            { "QT_COMPILER_SUPPORTS_SSE3", "1" },
                            { "QT_COMPILER_SUPPORTS_SSSE3", "1" },
                            { "QT_COMPILER_SUPPORTS_SSE4_1", "1" },
                            { "QT_COMPILER_SUPPORTS_SSE4_2", "1" },
                            { "QT_COMPILER_SUPPORTS_AVX", "1" },
                            { "QT_COMPILER_SUPPORTS_AVX2", "1" },
                            { "QT_LARGEFILE_SUPPORT", "64" },
                    },
            },
            // private
            {
                    // features
                    {
                            { "dbus_linked", false },
                            { "dbus", true },
                            { "gui", true },
                            { "libudev", false },
                            { "posix_fallocate", false },
                            { "private_tests", false },
                            { "reduce_exports", false },
                            { "reduce_relocations", false },
                            { "release_tools", false },
                            { "sse2", true },
                            { "system_zlib", false },
                            { "widgets", true },
                    },
            },
    },
};

static QtLibrary qt_core_desc {
    "QtCore",
    // config
    {
            // public
            {
                    // features
                    {
                            { "animation", true },
                            { "big_codecs", true },
                            { "codecs", true },
                            { "commandlineparser", true },
                            { "datestring", true },
                            { "easingcurve", true },
                            { "filesystemiterator", true },
                            { "filesystemwatcher", true },
                            { "gestures", true },
                            { "library", true },
                            { "mimetype", true },
                            { "mimetype_database", true },
                            { "process", true },
                            { "properties", true },
                            { "qeventtransition", true },
                            { "regularexpression", true },
                            { "settings", true },
                            { "sharedmemory", true },
                            { "signaling_nan", true },
                            { "statemachine", true },
                            { "systemsemaphore", true },
                            { "temporaryfile", true },
                            { "textcodec", true },
                            { "textdate", true },
                            { "timezone", true },
                            { "topleveldomain", true },
                            { "translation", true },
                            { "xmlstreamreader", true },
                            { "xmlstream", true },
                            { "xmlstreamwriter", true },
                            { "zstd", true },

                            { "hijricalendar", true },
                            { "jalalicalendar", true },
                            { "islamiccivilcalendar", true },

                            { "concatenatetablesproxymodel", true },
                            { "itemmodel", true },
                            { "identityproxymodel", true },
                            { "proxymodel", true },
                            { "sortfilterproxymodel", true },
                            { "stringlistmodel", true },
                            { "transposeproxymodel", true },

                            { "cborstreamreader", true },
                            { "cborstreamwriter", true },
                            { "binaryjson", true },
                    },
                    // defs
                    {
                            { "QT_NO_INOTIFY", "" },
                            { "QT_NO_GLIB", "" },
                            { "QT_NO_ICONV", "" },
                            { "QT_NO_EVENTFD", "" },
                            { "QT_NO_CLOCK_MONOTONIC", "" },
                            { "QT_CONFIGURE_LIBLOCATION_TO_PREFIX_PATH", "\".\"" },
                    },
            },
            // private
            {
                    // features
                    {
                            { "clock_gettime", false },
                            { "doubleconversion", true },
                            { "glib", false },
                            { "gnu_libiconv", false },
                            { "iconv", false },
                            { "icu", false },
                            { "inotify", false },
                            { "libatomic", false },
                            { "posix_libiconv", false },
                            { "sha3_fast", true },
                            { "system_doubleconversion", false },
                            { "system_pcre", false },
                            { "relocatable", true },
                            { "dlopen", false }, // win only
                    },
            },
    },
};

static QtLibrary qt_gui_desc {
    "QtGui",
    // config
    {
            // public
            {
                    // features
                    {
                            { "accessibility", true },
                            { "action", true },
                            { "angle", true },
                            { "clipboard", true },
                            { "colornames", true },
                            { "cssparser", true },
                            { "cursor", true },
                            { "desktopservices", true },
                            { "draganddrop", true },
                            { "dynamicgl", true },
                            { "highdpiscaling", true },
                            { "imageformat_bmp", true },
                            { "imageformat_jpeg", true },
                            { "imageformatplugin", true },
                            { "imageformat_png", true },
                            { "imageformat_ppm", true },
                            { "imageformat_xbm", true },
                            { "imageformat_xpm", true },
                            { "image_heuristic_mask", true },
                            { "image_text", true },
                            { "im", true },
                            { "movie", true },
                            { "opengles2", false },
                            { "opengles31", false },
                            { "opengles3", false },
                            { "opengl", true },
                            { "pdf", true },
                            { "picture", true },
                            { "sessionmanager", true },
                            { "shortcut", true },
                            { "standarditemmodel", true },
                            { "systemtrayicon", true },
                            { "tabletevent", true },
                            { "texthtmlparser", true },
                            { "textodfwriter", true },
                            { "validator", true },
                            { "vulkan", false },
                            { "whatsthis", true },
                            { "wheelevent", true },

                            { "textmarkdownreader", true },
                            { "textmarkdownwriter", true },
                    },
                    // defs
                    {
                            { "QT_NO_XRENDER", "" },
                            { "QT_QPA_DEFAULT_PLATFORM_NAME", "\"windows\"" },
                            { "QT_NO_ACCESSIBILITY_ATSPI_BRIDGE", "" },
                            { "QT_OPENGL_DYNAMIC", "true" },
                            { "QT_OPENGL_ES_2_ANGLE", "true" },
                            { "QT_NO_FONTCONFIG", "" },
                    },
            },
            // private
            {
                    // features
                    {
                            { "accessibility_atspi_bridge", false },
                            { "android_style_assets", false },
                            { "direct2d", true },
                            { "directfb", false },
                            { "directwrite2", true },
                            { "directwrite", true },
                            { "eglfs_brcm", false },
                            { "eglfs_egldevice", false },
                            { "eglfs", false },
                            { "eglfs_gbm", false },
                            { "eglfs_mali", false },
                            { "eglfs_viv", false },
                            { "eglfs_viv_wl", false },
                            { "egl", true },
                            { "egl_x11", false },
                            { "evdev", false },
                            { "fontconfig", false },
                            { "freetype", true },
                            { "gif", true },
                            { "harfbuzz", true },
                            { "ico", true },
                            { "integrityfb", false },
                            { "jpeg", true },
                            { "libinput_axis_api", false },
                            { "libinput", false },
                            { "linuxfb", false },
                            { "mirclient", false },
                            { "mtdev", false },
                            { "multiprocess", true },
                            { "png", true },
                            { "system_freetype", false },
                            { "system_harfbuzz", false },
                            { "system_jpeg", false },
                            { "system_png", false },
                            { "system_xcb", false },
                            { "tslib", false },
                            { "xcb", false },
                            { "xinput2", false },
                            { "xkbcommon_evdev", false },
                            { "xkb", false },
                            { "xlib", false },
                            { "xrender", false },

                            { "raster_64bit", true },
                    },
            },
    },
    // deps
    { "QtCore" },
};

static QtLibrary qt_widgets_desc {
    "QtWidgets",
    // config
    {
            // public
            {
                    // features
                    {
                            { "abstractbutton", true },
                            { "abstractslider", true },
                            { "buttongroup", true },
                            { "calendarwidget", true },
                            { "checkbox", true },
                            { "colordialog", true },
                            { "columnview", true },
                            { "combobox", true },
                            { "commandlinkbutton", true },
                            { "completer", true },
                            { "contextmenu", true },
                            { "datawidgetmapper", true },
                            { "datetimeedit", true },
                            { "dialogbuttonbox", true },
                            { "dialog", true },
                            { "dial", true },
                            { "dirmodel", true },
                            { "dockwidget", true },
                            { "effects", true },
                            { "errormessage", true },
                            { "filedialog", true },
                            { "filesystemmodel", true },
                            { "fontcombobox", true },
                            { "fontdialog", true },
                            { "formlayout", true },
                            { "fscompleter", true },
                            { "gestures", true },
                            { "graphicseffect", true },
                            { "graphicsview", true },
                            { "groupbox", true },
                            { "inputdialog", true },
                            { "itemviews", true },
                            { "keysequenceedit", true },
                            { "label", true },
                            { "lcdnumber", true },
                            { "lineedit", true },
                            { "listview", true },
                            { "listwidget", true },
                            { "mainwindow", true },
                            { "mdiarea", true },
                            { "menubar", true },
                            { "menu", true },
                            { "messagebox", true },
                            { "paint_debug", true },
                            { "progressbar", true },
                            { "progressdialog", true },
                            { "pushbutton", true },
                            { "radiobutton", true },
                            { "resizehandler", true },
                            { "rubberband", true },
                            { "scrollarea", true },
                            { "scrollbar", true },
                            { "scroller", true },
                            { "sizegrip", true },
                            { "slider", true },
                            { "spinbox", true },
                            { "splashscreen", true },
                            { "splitter", true },
                            { "stackedwidget", true },
                            { "statusbar", true },
                            { "statustip", true },
                            { "style_android", false },
                            { "style_fusion", true },
                            { "style_mac", false },
                            { "style_stylesheet", true },
                            // WIN32 is for moc, it does not understand and use _WIN32
                            { "style_windows", true, { "_WIN32", "WIN32" } },
                            { "style_windowsvista", true, { "_WIN32", "WIN32" } },
                            { "style_windowsxp", true, { "_WIN32", "WIN32" } },
                            { "syntaxhighlighter", true },
                            { "tabbar", true },
                            { "tableview", true },
                            { "tablewidget", true },
                            { "tabwidget", true },
                            { "textbrowser", true },
                            { "textedit", true },
                            { "toolbar", true },
                            { "toolbox", true },
                            { "toolbutton", true },
                            { "tooltip", true },
                            { "treeview", true },
                            { "treewidget", true },
                            { "undocommand", true },
                            { "undogroup", true },
                            { "undostack", true },
                            { "undoview", true },
                            { "whatsthis", true },
                            { "widgettextcontrol", true },
                            { "wizard", true },
                    },
            },
            // private
            {
                    // features
                    {
                            { "gtk3", false },
                    },
            },
    },
    // deps
    {
            "QtCore",
            "QtGui",
    },
};

static QtLibrary qt_network_desc {
    "QtNetwork",
    // config
    {
            // public
            {
                    // features
                    {
                            { "bearermanagement", true },
                            { "dnslookup", true },
                            { "dtls", true },
                            { "ftp", true },
                            { "http", true },
                            { "localserver", true },
                            { "networkdiskcache", true },
                            { "networkinterface", true },
                            { "networkproxy", true },
                            { "ocsp", true },
                            { "opensslv11", true },
                            { "schannel", false },
                            { "sctp", false },
                            { "socks5", true },
                            { "ssl", true },
                            { "udpsocket", true },
                    },
                    // defs
                    {
                            { "QT_NO_SCTP", "" },
                            { "QT_NO_IPV6IFNAME", "" },
                            { "QT_NO_GETADDRINFO", "" },
                            { "QT_NO_GETIFADDRS", "" },
                    },
            },
            // private
            {
                    // features
                    {
                            { "libproxy", false },
                            { "openssl_linked", false }, // true?
                            { "openssl", true },
                            { "securetransport", false },
                            { "system_proxies", true },

                            //{"gssapi", true},
                            { "sspi", true }, // win only!
                    },
            },
    },
    // deps
    { "QtCore" },
};

static QtLibrary qt_xml_desc {
    "QtXml",
    // config
    {
            // public
            {
                    // features
                    {
                            { "dom", true },
                    },
            },
    },
    // deps
    { "QtCore" },
};

static QtLibrary qt_sql_desc {
    "QtSql",
    // config
    {
            // public
            {
                    // features
                    {
                            { "sqlmodel", true },
                    },
            },
    },
    // deps
    { "QtCore" },
};

static QtLibrary qt_printsupport_desc {
    "QtPrintSupport",
    // config
    {
            // public
            {
                    // features
                    {
                            { "printer", true },
                            { "printdialog", true },
                            { "printpreviewwidget", true },
                            { "printpreviewdialog", true },
                    },
                    {
                            { "QT_NO_CUPS", "" },
                            { "QT_NO_CUPSJOBWIDGET", "" },
                    },
            },
            // private
            {
                    // features
                    {
                            { "cups", false },
                            { "cupsjobwidget", false },
                    },
            },
    },
    // deps
    {
            "QtCore",
            "QtGui",
            "QtWidgets",
    },
};

static QtLibrary qt_svg_desc {
    "QtSvg",
    // config
    {},
    // deps
    {
            "QtCore",
            "QtGui",
            "QtWidgets",
    },
};

static QtLibrary qt_winextras_desc {
    "QtWinExtras",
    // config
    {},
    // deps
    {
            "QtCore",
            "QtGui",
    },
};
