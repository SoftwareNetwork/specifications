#pragma sw require pub.egorpugin.primitives.filesystem

#include <regex>

void build(Solution &s)
{
    auto &p = s.addProject("wxWidgets", "3.2.1");
    p += Git("https://github.com/wxWidgets/wxWidgets", "v{v}");

    auto add_target = [&p](const String &name) -> decltype(auto)
    {
        auto &t = p.addTarget<Library>(name);
        t += sw::Shared, Definition("WXMAKINGDLL_" + boost::to_upper_copy(name));
        return t;
    };

    auto add_sources = [](auto &t, const String &n)
    {
        if (t.DryRun)
            return;

        const auto files_cmake_fn = "build/cmake/files.cmake";
        const auto files = read_file(t.SourceDir / files_cmake_fn);

        t += files_cmake_fn;

        auto read_files = [&t, &files](const String &n, const path &base = {}, bool is_header = false)
        {
            String f = "set(" + n;
            auto p = files.find(f);
            if (p == files.npos)
                return;

            auto str = files.substr(p, files.find(')', p) - p);
            str = str.substr(f.size());

            static std::regex r("#[^\\n]*");
            str = std::regex_replace(str, r, "");

            StringSet files2;
            boost::split(files2, str, boost::is_any_of(" \t\r\n"));

            for (auto &f : files2)
            {
                if (f.empty())
                    continue;
                if (is_header)
                    t -= base / f;
                else
                    t += base / f;
            }
        };

        read_files(n + "_SRC");
        read_files(n + "_HDR", "include", true);
    };

    auto &base = add_target("base");
    {
        base.setChecks("wx", true);
        base += "build/cmake/setup.h.in";

        base -= "src/common/.*"_rr;
        base -= "src/regex/.*"_rr;
        base += "src/regex/regexec.c";
        base += "src/regex/regcomp.c";
        base += "src/regex/regfree.c";
        base += "src/regex/regerror.c";
        base += "src/regex"_id;

        base += "include/.*"_rr;
        base += "interface/.*"_rr;
        base.Public += "include"_id;
        base.Public += "interface"_id;

        add_sources(base, "BASE_CMN");
        add_sources(base, "BASE_AND_GUI_CMN");

        if (base.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            add_sources(base, "BASE_WIN32");
            add_sources(base, "BASE_AND_GUI_WIN32");

            base += "advapi32.lib"_slib;
            base += "shell32.lib"_slib;
            base += "ole32.lib"_slib;
            base += "user32.lib"_slib;
            base += "uuid.lib"_slib;
            base += "Version.lib"_slib;
            base += "Shlwapi.lib"_slib;
        }
        else if (base.getBuildSettings().TargetOS.Type == OSType::IOS)
            add_sources(base, "BASE_AND_GUI_OSX_IPHONE");
        else if (base.getBuildSettings().TargetOS.Type == OSType::Macos)
            add_sources(base, "BASE_AND_GUI_OSX_COCOA");
        else
            add_sources(base, "BASE_UNIX");

        base.Interface += sw::Shared, "WXUSINGDLL"_def;
        base += "wxUSE_BASE=1"_def;
        base += "wxUSE_GUI=0"_def;
        base.Protected += "WXBUILDING"_def;

        if (base.getBuildSettings().TargetOS.Type == OSType::Windows)
            base.Public += "UNICODE"_def;
        else
            base.Public += "_UNICODE"_def;

        auto add_basic_settings = [&s](auto &base)
        {
            /*#cmakedefine WX_TIMEZONE @WX_TIMEZONE@
            #cmakedefine WX_SOCKLEN_T @WX_SOCKLEN_T@
            #cmakedefine SOCKOPTLEN_T @SOCKOPTLEN_T@
            #cmakedefine WX_STATFS_T @WX_STATFS_T@
            #define wxTYPE_SA_HANDLER @wxTYPE_SA_HANDLER@*/

            base.Variables["CMAKE_SIZEOF_VOID_P"] = base.Variables["SIZEOF_VOID_P"].toString();

            base.Variables["SIZEOF_WCHAR_T_CODE"] = "#define SIZEOF_WCHAR_T " + base.Variables["SIZEOF_WCHAR_T"].toString();
            base.Variables["SIZEOF_INT_CODE"] = "#define SIZEOF_INT " + base.Variables["SIZEOF_INT"].toString();
            base.Variables["SIZEOF_LONG_CODE"] = "#define SIZEOF_LONG " + base.Variables["SIZEOF_LONG"].toString();
            base.Variables["SIZEOF_LONG_LONG_CODE"] = "#define SIZEOF_LONG_LONG " + base.Variables["SIZEOF_LONG_LONG"].toString();
            base.Variables["SIZEOF_SHORT_CODE"] = "#define SIZEOF_SHORT " + base.Variables["SIZEOF_SHORT"].toString();
            base.Variables["SIZEOF_SIZE_T_CODE"] = "#define SIZEOF_SIZE_T " + base.Variables["SIZEOF_SIZE_T"].toString();

            base.Variables["ICONV_CONST"] = "const";

            base.Variables["WXGARBAGE_COLLECTION_ON"] = 0;
            base.Variables["WXWIN_COMPATIBILITY_2_8"] = 0;
            base.Variables["WXWIN_COMPATIBILITY_3_0"] = 0;
            base.Variables["wxDIALOG_UNIT_COMPATIBILITY"] = 0;
            base.Variables["wxHAVE_LIB_XPM"] = 1;
            base.Variables["wxUSE_ABOUTDLG"] = 1;
            base.Variables["wxUSE_ACCEL"] = 1;
            base.Variables["wxUSE_ACCESSIBILITY"] = 1;
            base.Variables["wxUSE_ACTIVITYINDICATOR"] = 1;
            base.Variables["wxUSE_ADDREMOVECTRL"] = 1;
            base.Variables["wxUSE_AFM_FOR_POSTSCRIPT"] = 1;
            base.Variables["wxUSE_ALL_THEMES"] = 1;
            base.Variables["wxUSE_ANIMATIONCTRL"] = 1;
            base.Variables["wxUSE_ANY"] = 1;
            base.Variables["wxUSE_APPLE_IEEE"] = 1;
            base.Variables["wxUSE_ARCHIVE_STREAMS"] = 1;
            base.Variables["wxUSE_ARTPROVIDER_STD"] = 1;
            base.Variables["wxUSE_ARTPROVIDER_TANGO"] = 1;
            base.Variables["wxUSE_AUI"] = 1;
            base.Variables["wxUSE_AUTOID_MANAGEMENT"] = 1;
            base.Variables["wxUSE_BANNERWINDOW"] = 1;
            base.Variables["wxUSE_BASE64"] = 1;
            base.Variables["wxUSE_BITMAPCOMBOBOX"] = 1;
            base.Variables["wxUSE_BMPBUTTON"] = 1;
            base.Variables["wxUSE_BUSYINFO"] = 1;
            base.Variables["wxUSE_BUTTON"] = 1;
            base.Variables["wxUSE_CALENDARCTRL"] = 1;
            base.Variables["wxUSE_CARET"] = 1;
            base.Variables["wxUSE_CHECKBOX"] = 1;
            base.Variables["wxUSE_CHECKLISTBOX"] = 1;
            base.Variables["wxUSE_CHOICE"] = 1;
            base.Variables["wxUSE_CHOICEBOOK"] = 1;
            base.Variables["wxUSE_CHOICEDLG"] = 1;
            base.Variables["wxUSE_CLIPBOARD"] = 1;
            base.Variables["wxUSE_CMDLINE_PARSER"] = 1;
            base.Variables["wxUSE_COLLPANE"] = 1;
            base.Variables["wxUSE_COLOURDLG"] = 1;
            base.Variables["wxUSE_COLOURPICKERCTRL"] = 1;
            base.Variables["wxUSE_COMBOBOX"] = 1;
            base.Variables["wxUSE_COMBOCTRL"] = 1;
            base.Variables["wxUSE_COMMANDLINKBUTTON"] = 1;
            base.Variables["wxUSE_COMMON_DIALOGS"] = 1;
            base.Variables["wxUSE_COMPILER_TLS"] = 1;
            base.Variables["wxUSE_CONFIG"] = 1;
            base.Variables["wxUSE_CONFIG_NATIVE"] = 1;
            base.Variables["wxUSE_CONSOLE_EVENTLOOP"] = 1;
            base.Variables["wxUSE_CONSTRAINTS"] = 1;
            base.Variables["wxUSE_CONTROLS"] = 1;
            base.Variables["wxUSE_CRASHREPORT"] = 1;
            base.Variables["wxUSE_DATAOBJ"] = 1;
            base.Variables["wxUSE_DATAVIEWCTRL"] = 1;
            base.Variables["wxUSE_DATEPICKCTRL"] = 1;
            base.Variables["wxUSE_DATEPICKCTRL_GENERIC"] = 1;
            base.Variables["wxUSE_DATETIME"] = 1;
            base.Variables["wxUSE_DBGHELP"] = 1;
            base.Variables["wxUSE_DC_CACHEING"] = 1;
            base.Variables["wxUSE_DC_TRANSFORM_MATRIX"] = 1;
            base.Variables["wxUSE_DEBUGREPORT"] = 1;
            base.Variables["wxUSE_DEBUG_CONTEXT"] = 0;
            base.Variables["wxUSE_DEBUG_NEW_ALWAYS"] = 0;
            base.Variables["wxUSE_DETECT_SM"] = 1;
            base.Variables["wxUSE_DIALUP_MANAGER"] = 1;
            base.Variables["wxUSE_DIRDLG"] = 1;
            base.Variables["wxUSE_DIRPICKERCTRL"] = 1;
            base.Variables["wxUSE_DISPLAY"] = 1;
            base.Variables["wxUSE_DOC_VIEW_ARCHITECTURE"] = 1;
            base.Variables["wxUSE_DRAGIMAGE"] = 1;
            base.Variables["wxUSE_DYNAMIC_LOADER"] = 1;
            base.Variables["wxUSE_DYNLIB_CLASS"] = 1;
            base.Variables["wxUSE_EDITABLELISTBOX"] = 1;
            if (base.getBuildSettings().TargetOS.Type == OSType::Linux)
                base.Variables["wxUSE_EPOLL_DISPATCHER"] = 1;
            base.Variables["wxUSE_EXCEPTIONS"] = 1;
            base.Variables["wxUSE_EXTENDED_RTTI"] = 0;
            base.Variables["wxUSE_FFILE"] = 1;
            base.Variables["wxUSE_FILE"] = 1;
            base.Variables["wxUSE_FILECTRL"] = 1;
            base.Variables["wxUSE_FILEDLG"] = 1;
            base.Variables["wxUSE_FILEPICKERCTRL"] = 1;
            base.Variables["wxUSE_FILESYSTEM"] = 1;
            base.Variables["wxUSE_FILE_HISTORY"] = 1;
            base.Variables["wxUSE_FINDREPLDLG"] = 1;
            base.Variables["wxUSE_FONTDLG"] = 1;
            base.Variables["wxUSE_FONTENUM"] = 1;
            base.Variables["wxUSE_FONTMAP"] = 1;
            base.Variables["wxUSE_FONTPICKERCTRL"] = 1;
            base.Variables["wxUSE_FSVOLUME"] = 1;
            base.Variables["wxUSE_FSWATCHER"] = 1;
            base.Variables["wxUSE_FS_ARCHIVE"] = 1;
            base.Variables["wxUSE_FS_INET"] = 1;
            base.Variables["wxUSE_FS_ZIP"] = 1;
            base.Variables["wxUSE_GAUGE"] = 1;
            base.Variables["wxUSE_GEOMETRY"] = 1;
            base.Variables["wxUSE_GIF"] = 1;
            base.Variables["wxUSE_GLCANVAS"] = 1;
            base.Variables["wxUSE_GLOBAL_MEMORY_OPERATORS"] = 1;
            base.Variables["wxUSE_GRAPHICS_CONTEXT"] = 1;
            base.Variables["wxUSE_GRAPHICS_DIRECT2D"] = 1;
            base.Variables["wxUSE_GRID"] = 1;
            base.Variables["wxUSE_GSTREAMER"] = 1;
            base.Variables["wxUSE_GSTREAMER_PLAYER"] = 1;
            base.Variables["wxUSE_GTKPRINT"] = 0;
            base.Variables["wxUSE_HEADERCTRL"] = 1;
            base.Variables["wxUSE_HELP"] = 1;
            base.Variables["wxUSE_HOTKEY"] = 1;
            base.Variables["wxUSE_HTML"] = 1;
            base.Variables["wxUSE_HYPERLINKCTRL"] = 1;
            base.Variables["wxUSE_ICO_CUR"] = 1;
            base.Variables["wxUSE_IFF"] = 1;
            base.Variables["wxUSE_IMAGE"] = 1;
            base.Variables["wxUSE_IMAGLIST"] = 1;
            base.Variables["wxUSE_INFOBAR"] = 1;
            base.Variables["wxUSE_INICONF"] = 1;
            base.Variables["wxUSE_INKEDIT"] = 1;
            base.Variables["wxUSE_INTL"] = 1;
            base.Variables["wxUSE_IOSTREAMH"] = 1;
            base.Variables["wxUSE_IPC"] = 1;
            base.Variables["wxUSE_IPV6"] = 1;
            base.Variables["wxUSE_JOYSTICK"] = 1;
            base.Variables["wxUSE_LIBGNOMEVFS"] = 1;
            base.Variables["wxUSE_LIBJPEG"] = 1;
            base.Variables["wxUSE_LIBLZMA"] = 1;
            base.Variables["wxUSE_LIBMSPACK"] = 1;
            base.Variables["wxUSE_LIBNOTIFY"] = 1;
            base.Variables["wxUSE_LIBNOTIFY_0_7"] = 1;
            base.Variables["wxUSE_LIBPNG"] = 1;
            base.Variables["wxUSE_LIBSDL"] = 1;
            base.Variables["wxUSE_LIBTIFF"] = 1;
            base.Variables["wxUSE_LISTBOOK"] = 1;
            base.Variables["wxUSE_LISTBOX"] = 1;
            base.Variables["wxUSE_LISTCTRL"] = 1;
            base.Variables["wxUSE_LOG"] = 1;
            base.Variables["wxUSE_LOGGUI"] = 1;
            base.Variables["wxUSE_LOGWINDOW"] = 1;
            base.Variables["wxUSE_LOG_DIALOG"] = 1;
            base.Variables["wxUSE_LONGLONG"] = 1;
            base.Variables["wxUSE_MARKUP"] = 1;
            base.Variables["wxUSE_MDI"] = 1;
            base.Variables["wxUSE_MDI_ARCHITECTURE"] = 1;
            base.Variables["wxUSE_MEDIACTRL"] = 1;
            base.Variables["wxUSE_MEMORY_TRACING"] = 1;
            base.Variables["wxUSE_MENUBAR"] = 1;
            base.Variables["wxUSE_MENUS"] = 1;
            base.Variables["wxUSE_MIMETYPE"] = 1;
            base.Variables["wxUSE_MINIFRAME"] = 1;
            base.Variables["wxUSE_MOUSEWHEEL"] = 1;
            base.Variables["wxUSE_MSGDLG"] = 1;
            base.Variables["wxUSE_MS_HTML_HELP"] = 1;
            base.Variables["wxUSE_NANOX"] = 1;
            base.Variables["wxUSE_NATIVE_PROGRESSDLG"] = 1;
            base.Variables["wxUSE_NATIVE_STATUSBAR"] = 1;
            base.Variables["wxUSE_NOTEBOOK"] = 1;
            base.Variables["wxUSE_NOTIFICATION_MESSAGE"] = 1;
            base.Variables["wxUSE_NUMBERDLG"] = 1;
            base.Variables["wxUSE_ODCOMBOBOX"] = 1;
            base.Variables["wxUSE_ON_FATAL_EXCEPTION"] = 1;
            base.Variables["wxUSE_OPENGL"] = 1;
            base.Variables["wxUSE_OWNER_DRAWN"] = 1;
            base.Variables["wxUSE_PALETTE"] = 1;
            base.Variables["wxUSE_PCX"] = 1;
            base.Variables["wxUSE_PLUGINS"] = 1;
            base.Variables["wxUSE_PNM"] = 1;
            base.Variables["wxUSE_POPUPWIN"] = 1;
            base.Variables["wxUSE_POSTSCRIPT"] = 1;
            base.Variables["wxUSE_POSTSCRIPT_ARCHITECTURE_IN_MSW"] = 1;
            base.Variables["wxUSE_PREFERENCES_EDITOR"] = 1;
            base.Variables["wxUSE_PRINTF_POS_PARAMS"] = 1;
            base.Variables["wxUSE_PRINTING_ARCHITECTURE"] = 1;
            base.Variables["wxUSE_PRIVATE_FONTS"] = 1;
            base.Variables["wxUSE_PROGRESSDLG"] = 1;
            base.Variables["wxUSE_PROPGRID"] = 1;
            base.Variables["wxUSE_PROTOCOL"] = 1;
            base.Variables["wxUSE_PROTOCOL_FILE"] = 1;
            base.Variables["wxUSE_PROTOCOL_FTP"] = 1;
            base.Variables["wxUSE_PROTOCOL_HTTP"] = 1;
            base.Variables["wxUSE_RADIOBOX"] = 1;
            base.Variables["wxUSE_RADIOBTN"] = 1;
            base.Variables["wxUSE_REARRANGECTRL"] = 1;
            base.Variables["wxUSE_REGEX"] = 1;
            base.Variables["wxUSE_REGKEY"] = 1;
            base.Variables["wxUSE_REPRODUCIBLE_BUILD"] = 1;
            base.Variables["wxUSE_RIBBON"] = 1;
            base.Variables["wxUSE_RICHEDIT"] = 1;
            base.Variables["wxUSE_RICHEDIT2"] = 1;
            base.Variables["wxUSE_RICHMSGDLG"] = 1;
            base.Variables["wxUSE_RICHTEXT"] = 1;
            base.Variables["wxUSE_RICHTOOLTIP"] = 1;
            base.Variables["wxUSE_SASH"] = 1;
            base.Variables["wxUSE_SCROLLBAR"] = 1;
            base.Variables["wxUSE_SEARCHCTRL"] = 1;
            base.Variables["wxUSE_SECRETSTORE"] = 1;
            base.Variables["wxUSE_SELECT_DISPATCHER"] = 0;
            base.Variables["wxUSE_SLIDER"] = 1;
            base.Variables["wxUSE_SNGLINST_CHECKER"] = 1;
            base.Variables["wxUSE_SOCKETS"] = 1;
            base.Variables["wxUSE_SOUND"] = 1;
            base.Variables["wxUSE_SPINBTN"] = 1;
            base.Variables["wxUSE_SPINCTRL"] = 1;
            base.Variables["wxUSE_SPLASH"] = 1;
            base.Variables["wxUSE_SPLINES"] = 1;
            base.Variables["wxUSE_SPLITTER"] = 1;
            base.Variables["wxUSE_STACKWALKER"] = 1;
            base.Variables["wxUSE_STARTUP_TIPS"] = 1;
            base.Variables["wxUSE_STATBMP"] = 1;
            base.Variables["wxUSE_STATBOX"] = 1;
            base.Variables["wxUSE_STATLINE"] = 1;
            base.Variables["wxUSE_STATTEXT"] = 1;
            base.Variables["wxUSE_STATUSBAR"] = 1;
            base.Variables["wxUSE_STC"] = 1;
            base.Variables["wxUSE_STDPATHS"] = 1;
            base.Variables["wxUSE_STD_CONTAINERS"] = 1;
            base.Variables["wxUSE_STD_DEFAULT"] = 1;
            base.Variables["wxUSE_STL"] = 0;
            base.Variables["wxUSE_STOPWATCH"] = 1;
            base.Variables["wxUSE_STREAMS"] = 1;
            base.Variables["wxUSE_SVG"] = 1;
            base.Variables["wxUSE_SYSTEM_OPTIONS"] = 1;
            base.Variables["wxUSE_TARSTREAM"] = 1;
            base.Variables["wxUSE_TASKBARBUTTON"] = 1;
            base.Variables["wxUSE_TASKBARICON"] = 1;
            base.Variables["wxUSE_TASKBARICON_BALLOONS"] = 1;
            base.Variables["wxUSE_TEXTBUFFER"] = 1;
            base.Variables["wxUSE_TEXTCTRL"] = 1;
            base.Variables["wxUSE_TEXTDLG"] = 1;
            base.Variables["wxUSE_TEXTFILE"] = 1;
            base.Variables["wxUSE_TGA"] = 1;
            base.Variables["wxUSE_THEME_GTK"] = 1;
            base.Variables["wxUSE_THEME_METAL"] = 1;
            base.Variables["wxUSE_THEME_MONO"] = 1;
            base.Variables["wxUSE_THEME_WIN32"] = 1;
            base.Variables["wxUSE_THREADS"] = 1;
            base.Variables["wxUSE_TIMEPICKCTRL"] = 1;
            base.Variables["wxUSE_TIMEPICKCTRL_GENERIC"] = 1;
            base.Variables["wxUSE_TIMER"] = 1;
            base.Variables["wxUSE_TIPWINDOW"] = 1;
            base.Variables["wxUSE_TOGGLEBTN"] = 1;
            base.Variables["wxUSE_TOOLBAR"] = 1;
            base.Variables["wxUSE_TOOLBAR_NATIVE"] = 1;
            base.Variables["wxUSE_TOOLBOOK"] = 1;
            base.Variables["wxUSE_TOOLTIPS"] = 1;
            base.Variables["wxUSE_TREEBOOK"] = 1;
            base.Variables["wxUSE_TREECTRL"] = 1;
            base.Variables["wxUSE_TREELISTCTRL"] = 1;
            base.Variables["wxUSE_UIACTIONSIMULATOR"] = 1;
            base.Variables["wxUSE_UNICODE"] = 1;
            base.Variables["wxUSE_UNICODE_UTF8"] = 1;
            base.Variables["wxUSE_UNIX"] = int(base.getBuildSettings().TargetOS.Type != OSType::Windows);
            base.Variables["wxUSE_UNSAFE_WXSTRING_CONV"] = 1;
            base.Variables["wxUSE_URL"] = 1;
            base.Variables["wxUSE_URL_NATIVE"] = 0;
            base.Variables["wxUSE_UTF8_LOCALE_ONLY"] = 1;
            base.Variables["wxUSE_UXTHEME"] = 1;
            base.Variables["wxUSE_VALIDATORS"] = 1;
            base.Variables["wxUSE_VARIANT"] = 1;
            base.Variables["wxUSE_WCHAR_T"] = 1;
            base.Variables["wxUSE_WEBKIT"] = 1;
            base.Variables["wxUSE_WEBVIEW"] = 1;
            base.Variables["wxUSE_WEBVIEW_IE"] = 1;
            base.Variables["wxUSE_WEBVIEW_WEBKIT"] = 1;
            base.Variables["wxUSE_WEBVIEW_WEBKIT2"] = 1;
            base.Variables["wxUSE_WINRT"] = 1;
            base.Variables["wxUSE_WIZARDDLG"] = 1;
            base.Variables["wxUSE_WXDIB"] = 1;
            base.Variables["wxUSE_WXHTML_HELP"] = 1;
            base.Variables["wxUSE_XLOCALE"] = 1;
            base.Variables["wxUSE_XML"] = 1;
            base.Variables["wxUSE_XPM"] = 1;
            base.Variables["wxUSE_XRC"] = 1;
            base.Variables["wxUSE_XTEST"] = 1;
            base.Variables["wxUSE_ZIPSTREAM"] = 1;

            base.Variables["wxUSE_ACTIVEX"] = 1;
            base.Variables["wxUSE_DRAG_AND_DROP"] = 1;
            base.Variables["wxUSE_OLE"] = 1;
            base.Variables["wxUSE_OLE_AUTOMATION"] = 1;

            base.Variables["wxUSE_METAFILE"] = 1;
            base.Variables["wxUSE_ENH_METAFILE"] = 1;
            base.Variables["wxUSE_WIN_METAFILES_ALWAYS"] = 0;

            if (base.getBuildSettings().TargetOS.Type == OSType::Windows || base.getBuildSettings().TargetOS.Type == OSType::Macos)
                base.Variables["wxUSE_CAIRO"] = 0;
            else
                base.Variables["wxUSE_CAIRO"] = 1;
            base.Variables["wxUSE_ZLIB"] = 1;
        };

        add_basic_settings(base);
        base.configureFile("build/cmake/setup.h.in", "wx/setup.h");

        base += "org.sw.demo.madler.zlib"_dep;
        base += "org.sw.demo.xz_utils.lzma"_dep;
        base += "org.sw.demo.pcre.pcre8"_dep;
    }

    auto &xml = add_target("xml");
    {
        add_sources(xml, "XML");
        xml.Public += base;
        xml.Public += "org.sw.demo.expat"_dep;
    }

    auto &net = add_target("net");
    {
        add_sources(net, "NET_CMN");

        if (net.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            add_sources(net, "NET_WIN32");
            net += "ws2_32.lib"_slib;
            net += "User32.lib"_slib;
        }
        else if (net.getBuildSettings().TargetOS.Type == OSType::Macos) /// apple
            add_sources(net, "NET_OSX");
        if (net.getBuildSettings().TargetOS.Type != OSType::Windows)
            add_sources(net, "NET_UNIX");

        net.Public += base;
    }

    auto &core = add_target("core");
    {
        core -= "art/.*"_rr;
        add_sources(core, "GUI_CMN");
        add_sources(core, "BASE_AND_GUI_CMN");

        if (core.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            add_sources(core, "BASE_AND_GUI_WIN32");

            add_sources(core, "MSW_LOWLEVEL");
            add_sources(core, "MSW_DESKTOP_LOWLEVEL");
            add_sources(core, "MSW");
            add_sources(core, "MSW_DESKTOP");

            core += "com.Microsoft.Windows.SDK.winrt"_dep;

            core += "Shell32.lib"_slib;
            core += "Comctl32.lib"_slib;
            core += "Comdlg32.lib"_slib;
            core += "Winmm.lib"_slib;
            core += "Winspool.lib"_slib;
            core += "Rpcrt4.lib"_slib;
            core += "OleAut32.lib"_slib;
            core += "uuid.lib"_slib;
            core += "ole32.lib"_slib;
            core += "uxtheme.lib"_slib;
            core += "Shlwapi.lib"_slib;
            core += "Oleacc.lib"_slib;
        }
        else
            add_sources(core, "UNIX");

        core.Protected += "wxUSE_BASE=0"_def;
        core.Public += "wxUSE_GUI=1"_def;

        core.Public += base;

        core -= "org.sw.demo.cairographics.cairo"_dep;
        if (core.getBuildSettings().TargetOS.Type == OSType::Windows ||
            core.getBuildSettings().TargetOS.Type == OSType::Macos)
            core += "org.sw.demo.cairographics.cairo"_dep;
        core += "org.sw.demo.tiff"_dep;
        core += "org.sw.demo.glennrp.png"_dep;
        core += "org.sw.demo.jpeg"_dep;
    }

    auto add_core_target = [&add_target, &core](const String &name) -> decltype(auto)
    {
        auto &t = add_target(name);
        t.Public += core;
        return t;
    };

    auto &aui = add_core_target("aui");
    {
        add_sources(aui, "AUI_CMN");
        if (aui.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            add_sources(aui, "AUI_MSW");
            aui += "Gdi32.lib"_slib;
            aui += "User32.lib"_slib;
            aui += "uxtheme.lib"_slib;
        }
        //else
        //AUI_GTK // gtk2
    }

    auto &html = add_core_target("html");
    {
        add_sources(html, "HTML_CMN");
        if (html.getBuildSettings().TargetOS.Type == OSType::Windows)
            add_sources(html, "HTML_MSW");
        html += "org.sw.demo.kyz.libmspack"_dep;
    }

    auto &xrc = add_core_target("xrc");
    {
        add_sources(xrc, "XRC");
        xrc += html, xml;
    }

    auto &gl = add_core_target("gl");
    {
        add_sources(gl, "OPENGL_CMN");
        gl -= "org.sw.demo.find.opengl-master"_dep;
        if (gl.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            add_sources(gl, "OPENGL_MSW");
            gl += "gdi32.lib"_slib;
            gl += "user32.lib"_slib;
            gl += "org.sw.demo.find.opengl-master"_dep;
        }
    }

    auto &media = add_core_target("media");
    {
        add_sources(media, "MEDIA_CMN");
        if (media.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            add_sources(media, "MEDIA_MSW");
            media += "gdi32.lib"_slib;
            media += "User32.lib"_slib;
            media += "ole32.lib"_slib;
        }
    }

    auto &propgrid = add_core_target("propgrid");
    {
        add_sources(propgrid, "PROPGRID");
    }

    auto &qa = add_core_target("qa");
    {
        add_sources(qa, "QA");
        qa += xml;
    }

    auto &ribbon = add_core_target("ribbon");
    {
        add_sources(ribbon, "RIBBON");
    }

    auto &richtext = add_core_target("richtext");
    {
        richtext -= "src/richtext/.*"_rr;
        add_sources(richtext, "RICHTEXT");
        richtext += html, xml;
    }

    auto &webview = add_core_target("webview");
    {
        add_sources(webview, "WEBVIEW_CMN");
        if (webview.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            add_sources(webview, "WEBVIEW_MSW");
            webview += "ole32.lib"_slib;
            webview += "OleAut32.lib"_slib;
            webview += "gdi32.lib"_slib;
            webview += "User32.lib"_slib;
            webview += "uuid.lib"_slib;
        }
    }

    auto &stc = add_core_target("stc");
    {
        stc -= "src/stc/.*"_rr;
        stc += "src/stc/scintilla/.*"_rr;
        stc += "src/stc/scintilla/include"_id;
        stc += "src/stc/scintilla/lexlib"_id;
        stc += "src/stc/scintilla/src"_id;
        stc += "NO_CXX11_REGEX"_def;
        stc += "__WX__"_def;

        add_sources(stc, "STC_CMN");

        stc += "SCI_LEXER"_def;
        stc += "LINK_LEXERS"_def;

        if (stc.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            stc += "ole32.lib"_slib;
            stc += "gdi32.lib"_slib;
            stc += "User32.lib"_slib;
            stc += "Imm32.lib"_slib;
        }
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("wx");
    s.checkFunctionExists("alloca");
    s.checkFunctionExists("clock_gettime");
    s.checkFunctionExists("dlerror");
    s.checkFunctionExists("dlopen");
    s.checkFunctionExists("dlsym");
    s.checkFunctionExists("endmntent");
    s.checkFunctionExists("endservent");
    s.checkFunctionExists("fallocate");
    s.checkFunctionExists("fchmod");
    s.checkFunctionExists("fchown");
    s.checkFunctionExists("fdwalk");
    s.checkFunctionExists("fsync");
    s.checkFunctionExists("getc_unlocked");
    s.checkFunctionExists("getfsstat");
    s.checkFunctionExists("getgrgid_r");
    s.checkFunctionExists("getmntent_r");
    s.checkFunctionExists("getprotobyname_r");
    s.checkFunctionExists("getpwuid_r");
    s.checkFunctionExists("getresuid");
    s.checkFunctionExists("getvfsstat");
    s.checkFunctionExists("gmtime_r");
    s.checkFunctionExists("hasmntopt");
    s.checkFunctionExists("iconv_open");
    s.checkFunctionExists("if_indextoname");
    s.checkFunctionExists("if_nametoindex");
    s.checkFunctionExists("inotify_init1");
    s.checkFunctionExists("issetugid");
    s.checkFunctionExists("kevent");
    s.checkFunctionExists("kqueue");
    s.checkFunctionExists("lchmod");
    s.checkFunctionExists("lchown");
    s.checkFunctionExists("localtime_r");
    s.checkFunctionExists("lstat");
    s.checkFunctionExists("mbrtowc");
    s.checkFunctionExists("memalign");
    s.checkFunctionExists("memmem");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("newlocale");
    s.checkFunctionExists("NSLinkModule");
    s.checkFunctionExists("pipe2");
    s.checkFunctionExists("poll");
    s.checkFunctionExists("posix_memalign");
    s.checkFunctionExists("printf_unix98");
    s.checkFunctionExists("prlimit");
    s.checkFunctionExists("readlink");
    s.checkFunctionExists("recvmmsg");
    s.checkFunctionExists("sendmmsg");
    s.checkFunctionExists("setenv");
    s.checkFunctionExists("setmntent");
    s.checkFunctionExists("snprintf_c99");
    s.checkFunctionExists("socket");
    s.checkFunctionExists("splice");
    s.checkFunctionExists("statfs");
    s.checkFunctionExists("statvfs");
    s.checkFunctionExists("stpcpy");
    s.checkFunctionExists("strcasecmp");
    s.checkFunctionExists("strerror_r");
    s.checkFunctionExists("strncasecmp");
    s.checkFunctionExists("strnlen");
    s.checkFunctionExists("strsignal");
    s.checkFunctionExists("strtod_l");
    s.checkFunctionExists("strtoll_l");
    s.checkFunctionExists("strtoull_l");
    s.checkFunctionExists("symlink");
    s.checkFunctionExists("sysctlbyname");
    s.checkFunctionExists("timegm");
    s.checkFunctionExists("unsetenv");
    s.checkFunctionExists("uselocale");
    s.checkFunctionExists("utimes");
    s.checkFunctionExists("valloc");
    s.checkFunctionExists("vasprintf");
    s.checkFunctionExists("vsnprintf");
    s.checkFunctionExists("vsnprintf_c99");
    s.checkFunctionExists("wcrtomb");
    s.checkFunctionExists("wcslen");
    s.checkFunctionExists("wcsnlen");
    s.checkFunctionExists("_NSGetEnviron");
    s.checkIncludeExists("attr/xattr.h");
    s.checkIncludeExists("crt_externs.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("fam.h");
    s.checkIncludeExists("fstab.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("linux/magic.h");
    s.checkIncludeExists("mach/mach_time.h");
    s.checkIncludeExists("malloc.h");
    s.checkIncludeExists("mntent.h");
    s.checkIncludeExists("linux/netlink.h");
    s.checkIncludeExists("sched.h");
    s.checkIncludeExists("selinux/selinux.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/event.h");
    s.checkIncludeExists("sys/filio.h");
    s.checkIncludeExists("sys/inotify.h");
    s.checkIncludeExists("sys/mntctl.h");
    s.checkIncludeExists("sys/mnttab.h");
    s.checkIncludeExists("sys/mount.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/statfs.h");
    s.checkIncludeExists("sys/statvfs.h");
    s.checkIncludeExists("sys/sysctl.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/vfstab.h");
    s.checkIncludeExists("sys/vfs.h");
    s.checkIncludeExists("sys/vmount.h");
    s.checkIncludeExists("sys/xattr.h");
    s.checkIncludeExists("termios.h");
    s.checkIncludeExists("values.h");
    s.checkIncludeExists("xlocale.h");
    s.checkIncludeExists("type_traits").setCpp();
    s.checkTypeSize("char");
    s.checkTypeSize("int");
    s.checkTypeSize("intmax_t");
    s.checkTypeSize("ip_mreqn");
    s.checkTypeSize("long");
    s.checkTypeSize("long long");
    s.checkTypeSize("ptrdiff_t");
    s.checkTypeSize("short");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("struct");
    s.checkTypeSize("unsigned");
    s.checkTypeSize("void *");
    s.checkTypeSize("__int64");
    s.checkTypeSize("wchar_t");
    s.checkTypeAlignment("uint32_t");
    s.checkTypeAlignment("uint64_t");
    s.checkTypeAlignment("unsigned long");
}
