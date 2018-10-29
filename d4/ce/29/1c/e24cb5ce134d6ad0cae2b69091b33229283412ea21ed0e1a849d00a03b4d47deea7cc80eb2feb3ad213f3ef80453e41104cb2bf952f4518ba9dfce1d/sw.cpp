#pragma sw require header pub.egorpugin.primitives.tools.syncqt-master

#pragma sw header on

#pragma sw require pub.egorpugin.primitives.hash-master

//#include <iostream>
#include <regex>

// http://doc.qt.io/qt-5/moc.html
struct moc_file
{
    path p;
    bool build = false;
    path from_file;
    bool from_cpp = false;
};

struct MocCommand : Command
{
    NativeExecutedTarget &t;

    MocCommand(NativeExecutedTarget &t)
        : t(t)
    {
        use_response_files = true;
        protect_args_with_quotes = false;
    }

    void prepare() override
    {
        if (prepared)
            return;
        t.addDefinitionsAndIncludeDirectories(*this);
        Command::prepare();
    }
};

void automoc(const DependencyPtr &moc, NativeExecutedTarget &t, const std::vector<moc_file> &additions = {})
{
    static Strings HeaderFileExtensions = []()
    {
        Strings HeaderFileExtensions;
        HeaderFileExtensions.push_back("h");
        HeaderFileExtensions.push_back("hh");
        HeaderFileExtensions.push_back("h++");
        HeaderFileExtensions.push_back("hm");
        HeaderFileExtensions.push_back("hpp");
        HeaderFileExtensions.push_back("hxx");
        HeaderFileExtensions.push_back("in");
        HeaderFileExtensions.push_back("txx");
        return HeaderFileExtensions;
    }();
    static StringSet HeaderFileExtensionsSet(HeaderFileExtensions.begin(), HeaderFileExtensions.end());

    if (t.PostponeFileResolving)
        return;

    // get current idirs
    // we do not need them as args, because args will get all target idirs and defs
    FilesOrdered idirs;
    //t.IncludeDirectories.insert(t.BinaryDir); // add bdir early
    t.TargetOptionsGroup::iterate<WithoutSourceFileStorage, WithNativeOptions>(
        [&idirs](auto &v, auto &s)
    {
        if (s.Inheritance == InheritanceType::Interface)
            return;
        for (auto &i : v.IncludeDirectories)
            idirs.push_back(i);
    });

    auto find_header = [](const auto &dir, const auto &base, path &o)
    {
        for (auto &e : HeaderFileExtensions)
        {
            auto p = dir / base;
            p += "." + e;
            if (fs::exists(p))
            {
                o = p;
                return true;
            }
        }
        return false;
    };

    std::vector<moc_file> files;
    for (auto &[p, f] : t)
    {
        if (f->skip)
            continue;
        auto ext = p.extension().u8string();
        if (ext == ".cpp")
        {
            static std::regex r("(^|\n)[ \t]*#[ \t]*include[ \t]+"
                "[\"<](([^ \">]+/)?moc_[^ \">/]+\\.cpp|[^ \">]+\\.moc)[\">]");
            std::smatch m;
            auto s = read_file(p);
            while (std::regex_search(s, m, r))
            {
                moc_file f;
                auto inc_string = m[2].str();
                if (inc_string.find("moc_") == 0)
                {
                    f.p = inc_string.substr(4);
                }
                else
                {
                    f.p = inc_string;
                    f.from_cpp = true;
                }
                f.p = p.parent_path() / f.p.stem();
                f.from_file = p;
                files.push_back(f);
                s = m.suffix().str();
            }
        }
    }

    files.insert(files.end(), additions.begin(), additions.end());

    for (auto &[p, f] : t)
    {
        if (f->skip)
            continue;
        auto ext = p.extension().u8string();
        if (ext == ".cpp" ||
            (ext.size() > 1 && HeaderFileExtensionsSet.find(ext.substr(1)) != HeaderFileExtensionsSet.end()))
        {
            auto s = read_file(p);
            if (s.find("Q_OBJECT") != s.npos ||
                s.find("Q_GADGET") != s.npos ||
                s.find("Q_NAMESPACE") != s.npos)
            {
                moc_file f;
                f.p = p.parent_path() / p.stem();
                f.build = true;
                files.push_back(f);
            }
        }
    }

    auto moc_dir = t.BinaryDir / "moc";
    Files used_files;
    for (auto &f : files)
    {
#ifdef _WIN32
        f.p = normalize_path_windows(f.p);
#endif

        auto dir = sha256(f.p.parent_path().u8string()).substr(0, 8);
        String fn;

        if (f.from_cpp)
            f.p += ".cpp";
        else if (!find_header(f.p.parent_path(), f.p.stem(), f.p))
        {
            if (find_header(f.p.parent_path(), f.p.stem() += "_p", f.p))
            {
                f.build = true;
            }
            else if (
                !std::any_of(idirs.begin(), idirs.end(), [&f, &find_header](auto &i)
            {
                return find_header(i, f.p.stem(), f.p);
            }))
            {
                f.p += ".cpp";
                //throw std::runtime_error("moc: nothing found for: " + f.p.u8string());
            }
        }

        if (!used_files.insert(f.p).second)
            continue;

        if (!f.from_cpp)
        {
            auto f2 = f.p.filename().stem().string();
            fn += "moc_" + f2 + ".cpp";
        }
        else
        {
            fn += f.p.filename().stem().string() + ".moc";
        }

        auto o = moc_dir / dir / fn;

        SW_MAKE_CUSTOM_COMMAND_AND_ADD(MocCommand, c, t, t);
        c->setProgram(moc);
        c->args.push_back(f.p.u8string());
        if (!f.from_file.empty())
        {
            c->args.push_back("-i");
            //File(f.from_file, *c->fs).addExplicitDependency(o);
        }
        c->args.push_back("-o");
        c->args.push_back(o.u8string());
        c->addInput(f.p);
        c->addOutput(o);

        if (f.build)
            t += o;
        else
            t -= o;

        (t + moc)->Dummy = true;
    }

    for (auto &f : files)
    {
        auto dir = sha256(f.p.parent_path().u8string()).substr(0, 8);
        t.IncludeDirectories.insert(moc_dir / dir);
    }
};

// http://doc.qt.io/qt-5/rcc.html
void rcc(const DependencyPtr &rcc, NativeExecutedTarget &t, const path &fn)
{
    static std::regex r("<file([^<]+)");

    if (t.PostponeFileResolving)
        return;

    std::smatch m;
    auto s = read_file(fn);
    Files files;
    while (std::regex_search(s, m, r))
    {
        auto f = m[1].str();
        files.insert(fn.parent_path() / f.substr(f.find('>') + 1));
        s = m.suffix().str();
    }

    auto outfilename = fn.filename().stem();
    auto outfile = t.BinaryDir / ("qrc_" + outfilename.u8string() + ".cpp");
    auto c = t.addCommand();
    c << cmd::prog(rcc)
        << "--name" << outfilename.u8string()
        << "--output" << cmd::out(outfile)
        << cmd::in(fn)
        << cmd::end()
        << cmd::in(files);
}

void rcc(const DependencyPtr &rcc, NativeExecutedTarget &t, const Files &files)
{
    for (auto &f : files)
        ::rcc(rcc, t, f);
}

// http://doc.qt.io/qt-5/uic.html
void uic(const DependencyPtr &uic, NativeExecutedTarget &t, const path &fn)
{
    auto outfilename = fn.filename().stem();
    auto outfile = t.BinaryDir / ("ui_" + outfilename.u8string() + ".h");
    auto c = t.addCommand();
    c << cmd::prog(uic)
        << "-o" << cmd::out(outfile)
        << cmd::in(fn);
    t += outfile;
}

void uic(const DependencyPtr &uic, NativeExecutedTarget &t, const Files &files)
{
    for (auto &f : files)
        ::uic(uic, t, f);
}

void platform_files(NativeExecutedTarget &t)
{
    // remove platform
    t -= ".*.mm"_rr;
    t -= ".*_mac.*"_rr;
    t -= ".*_unix.*"_rr;
    t -= ".*_android.*"_rr;
    t -= ".*_systemv.*"_rr;
    t -= ".*_haiku.*"_rr;
    t -= ".*_nacl.*"_rr;
    t -= ".*_winrt.*"_rr;
    t -= ".*_glib.*"_rr;
    t -= ".*_inotify.*"_rr;
    t -= ".*_kqueue.*"_rr;
    t -= ".*_linux.*"_rr;
    t -= ".*_macx.*"_rr;
    t -= ".*_posix.*"_rr;
    t -= ".*_tz.*"_rr;
    t -= ".*_stub.*"_rr;
    t -= ".*_generic.*"_rr;
    t -= ".*_cf.*"_rr;
    t -= ".*_cf_p.*"_rr;
    t -= ".*_fsevents.*"_rr;
    t -= ".*_fsevents_p.*"_rr;
    t -= ".*_unix_p.*"_rr;
    t -= ".*_glib_p.*"_rr;
    t -= ".*_winrt_p.*"_rr;
    t -= ".*_kqueue_p.*"_rr;
    t -= ".*_inotify_p.*"_rr;
    t -= ".*_icu.*"_rr;
    t -= ".*_mips_dsp.*"_rr;
    t -= ".*_es2.*"_rr;
    t -= ".*_qpa.*"_rr;
};

#pragma sw header off

void automoc(NativeExecutedTarget &moc, NativeExecutedTarget &t, const std::vector<moc_file> &additions = {})
{
    ::automoc(moc.getDependency(), t, additions);
}

void rcc(NativeExecutedTarget &rcc, NativeExecutedTarget &t, const path &fn)
{
    ::rcc(rcc.getDependency(), t, fn);
}

void rcc(NativeExecutedTarget &rcc, NativeExecutedTarget &t, const Files &files)
{
    ::rcc(rcc.getDependency(), t, files);
}

void uic(NativeExecutedTarget &uic, NativeExecutedTarget &t, const path &fn)
{
    ::uic(uic.getDependency(), t, fn);
}

void uic(NativeExecutedTarget &uic, NativeExecutedTarget &t, const Files &files)
{
    ::uic(uic.getDependency(), t, files);
}

void build(Solution &s)
{
    auto &qt = s.addProject("qtproject.qt", "5.11.2");

    auto &base = qt.addProject("base");
    base += Git("git://code.qt.io/qt/qtbase.git", "v{v}");

    auto &base_tools = base.addDirectory("tools");
    auto &moc = base_tools.addTarget<ExecutableTarget>("moc");
    auto &rcc = base_tools.addTarget<ExecutableTarget>("rcc");
    auto &uic = base_tools.addTarget<ExecutableTarget>("uic");

    // base
    {
        auto &third_party = base.addDirectory("third_party");

        auto &harfbuzz = third_party.addStaticLibrary("harfbuzz");
        {
            harfbuzz.setRootDirectory("src/3rdparty/harfbuzz");
            harfbuzz -= "src/.*"_rr;
            harfbuzz +=
                "src/harfbuzz.c",
                "src/harfbuzz-impl.c",
                "src/harfbuzz-stream.c",
                "src/harfbuzz-shaper-all.cpp";
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                harfbuzz.Private += "UNICODE"_d;
                harfbuzz.Public += "WIN32"_d;
            }
        }

        auto &iaccessible2 = third_party.addTarget<StaticLibraryTarget>("iaccessible2");
        {
            iaccessible2 += "src/3rdparty/iaccessible2/generated/.*"_rr;
            iaccessible2.Public += "src/3rdparty/iaccessible2/generated"_idir;
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                iaccessible2.Private += "UNICODE"_d;
            }
            if (s.Settings.TargetOS.is(ArchType::x86))
            {
                iaccessible2 -= "src/3rdparty/iaccessible2/generated/amd64/.*"_rr;
                iaccessible2.Public += "src/3rdparty/iaccessible2/generated/x86"_idir;
            }
            else
            {
                iaccessible2 -= "src/3rdparty/iaccessible2/generated/x86/.*"_rr;
                iaccessible2.Public += "src/3rdparty/iaccessible2/generated/amd64"_idir;
            }
        }

        auto &wintab = third_party.addTarget<StaticLibraryTarget>("wintab");
        {
            wintab += "src/3rdparty/wintab/.*"_rr;
            wintab.Public += "src/3rdparty/wintab"_idir;
        }

        auto &bootstrap = base_tools.addTarget<StaticLibraryTarget>("bootstrap");
        {
            bootstrap +=
                "mkspecs/.*\\.h"_rr,
                "src/3rdparty/freebsd/strtoll.c",
                "src/3rdparty/freebsd/strtoull.c",
                "src/3rdparty/sha1/sha1.cpp",
                "src/corelib/.*\\.h"_rr,
                "src/corelib/codecs/qlatincodec.cpp",
                "src/corelib/codecs/qtextcodec.cpp",
                "src/corelib/codecs/qutfcodec.cpp",
                "src/corelib/global/archdetect.cpp",
                "src/corelib/global/qglobal.cpp",
                "src/corelib/global/qlogging.cpp",
                "src/corelib/global/qmalloc.cpp",
                "src/corelib/global/qnumeric.cpp",
                "src/corelib/global/qoperatingsystemversion.cpp",
                "src/corelib/global/qoperatingsystemversion_darwin.mm",
                "src/corelib/global/qoperatingsystemversion_win.cpp",
                "src/corelib/global/qrandom.cpp",
                "src/corelib/io/qabstractfileengine.cpp",
                "src/corelib/io/qbuffer.cpp",
                "src/corelib/io/qdebug.cpp",
                "src/corelib/io/qdir.cpp",
                "src/corelib/io/qdiriterator.cpp",
                "src/corelib/io/qfile.cpp",
                "src/corelib/io/qfiledevice.cpp",
                "src/corelib/io/qfileinfo.cpp",
                "src/corelib/io/qfilesystemengine.cpp",
                "src/corelib/io/qfilesystemengine_unix.cpp",
                "src/corelib/io/qfilesystemengine_win.cpp",
                "src/corelib/io/qfilesystementry.cpp",
                "src/corelib/io/qfilesystemiterator_unix.cpp",
                "src/corelib/io/qfilesystemiterator_win.cpp",
                "src/corelib/io/qfsfileengine.cpp",
                "src/corelib/io/qfsfileengine_iterator.cpp",
                "src/corelib/io/qfsfileengine_unix.cpp",
                "src/corelib/io/qfsfileengine_win.cpp",
                "src/corelib/io/qiodevice.cpp",
                "src/corelib/io/qloggingcategory.cpp",
                "src/corelib/io/qloggingregistry.cpp",
                "src/corelib/io/qresource.cpp",
                "src/corelib/io/qstandardpaths.cpp",
                "src/corelib/io/qstandardpaths_mac.mm",
                "src/corelib/io/qstandardpaths_unix.cpp",
                "src/corelib/io/qstandardpaths_win.cpp",
                "src/corelib/io/qtemporaryfile.cpp",
                "src/corelib/kernel/qcore_foundation.mm",
                "src/corelib/kernel/qcore_mac.cpp",
                "src/corelib/kernel/qcore_mac_objc.mm",
                "src/corelib/kernel/qcoreapplication.cpp",
                "src/corelib/kernel/qcoreapplication_mac.cpp",
                "src/corelib/kernel/qcoreapplication_win.cpp",
                "src/corelib/kernel/qcoreglobaldata.cpp",
                "src/corelib/kernel/qmetatype.cpp",
                "src/corelib/kernel/qsystemerror.cpp",
                "src/corelib/kernel/qvariant.cpp",
                "src/corelib/plugin/qsystemlibrary.cpp",
                "src/corelib/plugin/quuid.cpp",
                "src/corelib/serialization/qdatastream.cpp",
                "src/corelib/serialization/qjson.cpp",
                "src/corelib/serialization/qjsonarray.cpp",
                "src/corelib/serialization/qjsondocument.cpp",
                "src/corelib/serialization/qjsonobject.cpp",
                "src/corelib/serialization/qjsonparser.cpp",
                "src/corelib/serialization/qjsonvalue.cpp",
                "src/corelib/serialization/qjsonwriter.cpp",
                "src/corelib/serialization/qtextstream.cpp",
                "src/corelib/serialization/qxmlstream.cpp",
                "src/corelib/serialization/qxmlutils.cpp",
                "src/corelib/tools/qarraydata.cpp",
                "src/corelib/tools/qbitarray.cpp",
                "src/corelib/tools/qbytearray.cpp",
                "src/corelib/tools/qbytearraymatcher.cpp",
                "src/corelib/tools/qchar.cpp",
                "src/corelib/tools/qcommandlineoption.cpp",
                "src/corelib/tools/qcommandlineparser.cpp",
                "src/corelib/tools/qcryptographichash.cpp",
                "src/corelib/tools/qdatetime.cpp",
                "src/corelib/tools/qhash.cpp",
                "src/corelib/tools/qline.cpp",
                "src/corelib/tools/qlinkedlist.cpp",
                "src/corelib/tools/qlist.cpp",
                "src/corelib/tools/qlocale.cpp",
                "src/corelib/tools/qlocale_tools.cpp",
                "src/corelib/tools/qmap.cpp",
                "src/corelib/tools/qpoint.cpp",
                "src/corelib/tools/qrect.cpp",
                "src/corelib/tools/qregexp.cpp",
                "src/corelib/tools/qringbuffer.cpp",
                "src/corelib/tools/qsize.cpp",
                "src/corelib/tools/qstring.cpp",
                "src/corelib/tools/qstring_compat.cpp",
                "src/corelib/tools/qstringbuilder.cpp",
                "src/corelib/tools/qstringlist.cpp",
                "src/corelib/tools/qstringmatcher.cpp",
                "src/corelib/tools/qunicodetables.cpp",
                "src/corelib/tools/qvsnprintf.cpp",
                "src/xml/.*\\.h"_rr,
                "src/xml/dom/qdom.cpp",
                "src/xml/sax/qxml.cpp";

            bootstrap -=
                "src/3rdparty/freebsd/strtoll.c",
                "src/3rdparty/freebsd/strtoull.c",
                "src/3rdparty/sha1/sha1.cpp",
                "src/corelib/global/archdetect.cpp",
                "src/corelib/global/qoperatingsystemversion_darwin.mm",
                "src/corelib/global/qoperatingsystemversion_win.cpp",
                "src/corelib/io/qfilesystemengine_unix.cpp",
                "src/corelib/io/qfilesystemengine_win.cpp",
                "src/corelib/io/qfilesystemiterator_unix.cpp",
                "src/corelib/io/qfilesystemiterator_win.cpp",
                "src/corelib/io/qfsfileengine_unix.cpp",
                "src/corelib/io/qfsfileengine_win.cpp",
                "src/corelib/io/qstandardpaths_mac.mm",
                "src/corelib/io/qstandardpaths_unix.cpp",
                "src/corelib/io/qstandardpaths_win.cpp",
                "src/corelib/kernel/qcore_foundation.mm",
                "src/corelib/kernel/qcore_mac.cpp",
                "src/corelib/kernel/qcore_mac_objc.mm",
                "src/corelib/kernel/qcoreapplication_mac.cpp",
                "src/corelib/kernel/qcoreapplication_win.cpp",
                "src/corelib/plugin/qsystemlibrary.cpp",
                "src/corelib/tools/qchar.cpp",
                "src/corelib/tools/qstringmatcher.cpp",
                "src/corelib/tools/qunicodetables.cpp";

            bootstrap.Private +=
                "src/xml"_id,
                "src/xml/dom"_id,
                "src/xml/sax"_id;

            bootstrap.Public += "QT_BOOTSTRAPPED"_d;
            //bootstrap.Public += "QT_NO_QOBJECT"_d;
            bootstrap.Public += "QT_NO_CAST_FROM_ASCII"_d;
            bootstrap.Public += "QT_NO_CAST_TO_ASCII"_d;
            bootstrap.Public += "QT_NO_FOREACH"_d;
            Definition d;
            d.d = "QT_VERSION_MAJOR=" + bootstrap.Variables["PACKAGE_VERSION_MAJOR"].toString();
            bootstrap.Public += d;
            d.d = "QT_VERSION_MINOR=" + bootstrap.Variables["PACKAGE_VERSION_MINOR"].toString();
            bootstrap.Public += d;
            d.d = "QT_VERSION_PATCH=" + bootstrap.Variables["PACKAGE_VERSION_PATCH"].toString();
            bootstrap.Public += d;
            d.d = "QT_VERSION_STR=\"" + bootstrap.getPackage().getVersion().toString() + "\"";
            bootstrap.Public += d;
            if (s.Settings.Native.CompilerType == CompilerType::MSVC)
            {
                bootstrap.Public += "_ENABLE_EXTENDED_ALIGNED_STORAGE"_d;
            }
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                bootstrap.Private += "UNICODE"_d;
                bootstrap.Public += "WIN32"_d;
            }
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                bootstrap.Public += "Netapi32.lib"_lib, "Advapi32.lib"_lib, "Ole32.lib"_lib, "Shell32.lib"_lib;
            }

            bootstrap.Public += "org.sw.demo.madler.zlib-1"_dep;

            bootstrap.writeFileOnce("QtCore/QtConfig", "#include \"qconfig.h\"");
            bootstrap.writeFileOnce("QtCore/qconfig.h", R"xxx(
                #define QT_FEATURE_framework -1
                #define QT_FEATURE_rpath -1
                #define QT_FEATURE_accessibility 1
                #define QT_FEATURE_shared 1
                #define QT_COMPILER_SUPPORTS_SSE2 1
                #define QT_COMPILER_SUPPORTS_SSE3 1
                #define QT_COMPILER_SUPPORTS_SSSE3 1
                #define QT_COMPILER_SUPPORTS_SSE4_1 1
                #define QT_COMPILER_SUPPORTS_SSE4_2 1
                #define QT_COMPILER_SUPPORTS_AVX 1
                #define QT_COMPILER_SUPPORTS_AVX2 1
                #define QT_FEATURE_debug_and_release 1
                #define QT_FEATURE_build_all 1
                #define QT_FEATURE_c__11 1
                #define QT_FEATURE_c__14 -1
                #define QT_FEATURE_c__1z -1
                #define QT_FEATURE_concurrent 1
                #define QT_FEATURE_pkg_config -1
                #define QT_FEATURE_force_asserts -1
                #define QT_LARGEFILE_SUPPORT 64
                #define QT_NO_MREMAP
                #define QT_FEATURE_separate_debug_info -1
                #define QT_FEATURE_simulator_and_device -1
                #define QT_FEATURE_static -1
    )xxx");

            bootstrap.writeFileOnce("QtCore/private/qconfig_p.h", R"xxx(
                #define QT_FEATURE_sse2 1
                #define QT_FEATURE_private_tests -1
                #define QT_FEATURE_dbus 1
                #define QT_FEATURE_dbus_linked -1
                #define QT_FEATURE_gui 1
                #define QT_FEATURE_libudev -1
                #define QT_FEATURE_posix_fallocate -1
                #define QT_FEATURE_reduce_exports -1
                #define QT_FEATURE_reduce_relocations -1
                #define QT_FEATURE_release_tools -1
                #define QT_FEATURE_system_zlib -1
                #define QT_FEATURE_widgets 1
    )xxx");

            bootstrap.writeFileOnce("QtCore/qtcore-config.h.h", R"xxx(
                #define QT_FEATURE_properties 1
                #define QT_FEATURE_animation 1
                #define QT_FEATURE_textcodec 1
                #define QT_FEATURE_big_codecs 1
                #define QT_NO_CLOCK_MONOTONIC
                #define QT_FEATURE_codecs 1
                #define QT_FEATURE_commandlineparser 1
                #define QT_FEATURE_textdate 1
                #define QT_FEATURE_datestring 1
                #define QT_NO_DYNAMIC_LIBRARY true
                #define QT_NO_EVENTFD
                #define QT_FEATURE_filesystemiterator 1
                #define QT_FEATURE_filesystemwatcher 1
                #define QT_FEATURE_gestures 1
                #define QT_NO_GLIB
                #define QT_NO_ICONV
                #define QT_FEATURE_itemmodel 1
                #define QT_FEATURE_proxymodel 1
                #define QT_FEATURE_identityproxymodel 1
                #define QT_NO_INOTIFY
                #define QT_FEATURE_library 1
                #define QT_FEATURE_mimetype 1
                #define QT_FEATURE_process 1
                #define QT_FEATURE_statemachine 1
                #define QT_FEATURE_qeventtransition 1
                #define QT_FEATURE_regularexpression 1
                #define QT_FEATURE_settings 1
                #define QT_FEATURE_sharedmemory 1
                #define QT_FEATURE_sortfilterproxymodel 1
                #define QT_FEATURE_stringlistmodel 1
                #define QT_FEATURE_systemsemaphore 1
                #define QT_FEATURE_temporaryfile 1
                #define QT_FEATURE_timezone 1
                #define QT_FEATURE_topleveldomain 1
                #define QT_FEATURE_translation 1
                #define QT_FEATURE_xmlstream 1
                #define QT_FEATURE_xmlstreamreader 1
                #define QT_FEATURE_xmlstreamwriter 1
    )xxx");

            bootstrap.writeFileOnce("QtCore/private/qtcore-config_p.h", R"xxx(
                #define QT_FEATURE_clock_gettime -1
                #define QT_FEATURE_doubleconversion 1
                #define QT_FEATURE_glib -1
                #define QT_FEATURE_gnu_libiconv -1
                #define QT_FEATURE_icu -1
                #define QT_FEATURE_posix_libiconv -1
                #define QT_FEATURE_iconv -1
                #define QT_FEATURE_inotify -1
                #define QT_FEATURE_libatomic -1
                #define QT_FEATURE_sha3_fast 1
                #define QT_FEATURE_system_doubleconversion -1
                #define QT_FEATURE_system_pcre -1
    )xxx");

            bootstrap.writeFileOnce("QtCore/QtCoreDepends");

            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                bootstrap +=
                    "src/corelib/io/qfilesystemengine_win.cpp",
                    "src/corelib/io/qfilesystemiterator_win.cpp",
                    "src/corelib/io/qfsfileengine_win.cpp",
                    "src/corelib/kernel/qcoreapplication_win.cpp",
                    "src/corelib/plugin/qsystemlibrary.cpp",
                    "src/corelib/global/qoperatingsystemversion_win.cpp",
                    "src/corelib/io/qstandardpaths_win.cpp"
                    ;
            }
            else
            {
                bootstrap +=
                    "src/corelib/io/qfilesystemengine_unix.cpp",
                    "src/corelib/io/qfilesystemiterator_unix.cpp",
                    "src/corelib/io/qfsfileengine_unix.cpp",
                    "src/corelib/io/qstandardpaths_unix.cpp"
                    ;
            }

            if (s.Settings.TargetOS.Type == OSType::Macos)
            {
                bootstrap +=
                    "src/corelib/kernel/qcoreapplication_mac.cpp",
                    "src/corelib/kernel/qcore_mac_objc.cpp",
                    "src/corelib/kernel/qcore_mac.cpp",
                    "src/corelib/kernel/qcore_foundation.cpp",
                    "src/corelib/global/qoperatingsystemversion_darwin.mm",
                    "src/corelib/kernel/qcore_foundation.cpp",
                    "src/corelib/io/qstandardpaths_mac.mm"
                    ;
            }

            auto copy = [](auto &t, const path &in, const path &out)
            {
                SW_MAKE_EXECUTE_BUILTIN_COMMAND_AND_ADD(c, t, "sw_copy_file");
                c->args.push_back(in.u8string());
                c->args.push_back(out.u8string());
                c->addInput(in);
                c->addOutput(out);
                return c;
            };

            copy(bootstrap, bootstrap.SourceDir / "src/xml/sax/qxml_p.h", bootstrap.BinaryDir / "private/qxml_p.h");
            syncqt(bootstrap, { "QtCore", "QtXml" });

            if (s.Settings.Native.CompilerType == CompilerType::MSVC)
                bootstrap.Public += "mkspecs/win32-msvc"_idir;
        }

        // moc
        {
            moc +=
                "src/tools/moc/[^/]*\\.cpp"_rr,
                "src/tools/moc/[^/]*\\.h"_rr;
            moc -=
                "src/tools/moc/keywords.cpp",
                "src/tools/moc/ppkeywords.cpp",
                "src/tools/moc/mwerks_mac.cpp";
            moc.Public += "QT_MOC"_d;
            moc.Public += "QT_NO_CAST_FROM_ASCII"_d;
            moc.Public += "QT_NO_CAST_FROM_BYTEARRAY"_d;
            moc.Public += "QT_NO_COMPRESS"_d;
            moc.Public += "QT_NO_FOREACH"_d;
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                moc.Private += "UNICODE"_d;
                moc.Public += "WIN32"_d;
            }
            moc.Public += bootstrap;
        }

        // rcc
        {
            rcc += "src/tools/rcc/.*"_rr;
            rcc.Public += "src/tools/rcc"_id;
            rcc.Public += "QT_NO_CAST_FROM_ASCII"_d;
            rcc.Public += "QT_NO_FOREACH"_d;
            rcc.Public += "QT_RCC"_d;
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                rcc.Private += "UNICODE"_d;
                rcc.Public += "WIN32"_d;
            }
            rcc.Public += bootstrap;
        }

        // uic
        {
            uic +=
                "src/tools/uic/.*\\.cpp"_rr,
                "src/tools/uic/.*\\.h"_rr;
            uic.Public += "src/tools/uic/cpp"_id;
            uic.Public += "src/tools/uic"_id;
            uic.Public += "QT_NO_CAST_FROM_ASCII"_d;
            uic.Public += "QT_NO_FOREACH"_d;
            uic.Public += "QT_UIC"_d;
            uic.Public += "QT_UIC_CPP_GENERATOR"_d;
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                uic.Private += "UNICODE"_d;
                uic.Public += "WIN32"_d;
            }
            uic.Public += bootstrap;
        }

        auto write_tracepoints = [](auto &t)
        {
            t.writeFileOnce(t.BinaryDir / ("qt" + t.pkg.ppath.back() + "_tracepoints_p.h"), R"xxx(
                #  define Q_TRACE(x, ...)
                #  define Q_UNCONDITIONAL_TRACE(x, ...)
                #  define Q_TRACE_ENABLED(x) false
    )xxx");
        };

        auto &core = base.addTarget<LibraryTarget>("core");
        {
            core -=
                "mkspecs/.*\\.h"_rr;
            core +=
                "src/3rdparty/easing/easing.cpp",
                "src/3rdparty/freebsd/.*\\.c"_rr,
                "src/3rdparty/md[45]/.*"_rr,
                "src/3rdparty/rfc6234/.*\\.[hc]"_rr,
                "src/3rdparty/sha1/sha1.cpp",
                "src/3rdparty/sha3/.*"_rr,
                "src/platformheaders/.*\\.h"_rr;
            core -=
                "src/3rdparty/easing/easing.cpp",
                "src/3rdparty/freebsd/strtoll.c",
                "src/3rdparty/freebsd/strtoull.c",
                "src/3rdparty/md4/md4.cpp",
                "src/3rdparty/md5/md5.cpp",
                "src/3rdparty/rfc6234/sha224-256.c",
                "src/3rdparty/rfc6234/sha384-512.c",
                "src/3rdparty/sha1/sha1.cpp",
                "src/3rdparty/sha3/.*"_rr;
            core -=
                ".*\\.qdoc"_rr;
            core -= "src/3rdparty/sha3/.*"_rr;

            if (s.Settings.Native.CompilerType == CompilerType::MSVC)
                core.Public += "mkspecs/win32-msvc"_idir;

            syncqt(core, { "QtCore", "QtPlatformHeaders" });

            core.SourceDir /= "src/corelib";
            core +=
                "animation/.*"_rr,
                "arch/.*"_rr,
                "codecs/.*"_rr,
                "global/.*"_rr,
                "io/.*"_rr,
                "itemmodels/.*"_rr,
                "json/.*"_rr,
                "kernel/.*"_rr,
                "mimetypes/.*"_rr,
                "plugin/.*"_rr,
                "serialization/.*"_rr,
                "statemachine/.*"_rr,
                "thread/.*"_rr,
                "tools/.*"_rr,
                "xml/.*"_rr;

            core -=
                "global/qfloat16.*"_rr,
                "io/qfilesystemengine_unix.cpp",
                "io/qfilesystemengine_win.cpp",
                "io/qfilesystemiterator_unix.cpp",
                "io/qfilesystemiterator_win.cpp",
                "io/qfilesystemwatcher_.*\\.cpp"_rr,
                "io/qfsfileengine_unix.cpp",
                "io/qfsfileengine_win.cpp",
                "io/qstandardpaths_mac.mm",
                "io/qstandardpaths_unix.cpp",
                "io/qstandardpaths_win.cpp",
                "kernel/qcore_foundation.mm",
                "kernel/qcore_mac.cpp",
                "kernel/qcore_mac_objc.mm",
                "kernel/qcoreapplication_mac.cpp",
                "kernel/qcoreapplication_win.cpp",
                "plugin/qsystemlibrary.cpp",
                "thread/qmutex_.*"_rr;

            core += "io/qfilesystemwatcher_polling.*"_rr;

            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                core -= "codecs/qiconvcodec.cpp";
                core += "io/qfilesystemengine_win.cpp";
                core += "io/qfilesystemiterator_win.cpp";
                core += "io/qfilesystemwatcher_win.cpp";
                core += "io/qfsfileengine_win.cpp";
                core += "kernel/qcoreapplication_win.cpp";
                core += "plugin/qsystemlibrary.cpp";
                core += "io/qstandardpaths_win.cpp";
            }
            else
            {
                core += "io/qfilesystemengine_unix.cpp";
                core += "io/qfilesystemiterator_unix.cpp";
                core += "io/qfsfileengine_unix.cpp";
                core += "io/qstandardpaths_unix.cpp";
            }

            if (s.Settings.TargetOS.Type == OSType::Macos)
            {
                core += "kernel/qcoreapplication_mac.cpp";
                core += "kernel/qcore_mac.cpp";
                core += "kernel/qcore_mac_objc.cpp";
                core += "kernel/qcore_foundation.cpp";
                core += "kernel/qcore_foundation.cpp";
                core += "io/qstandardpaths_mac.mm";
            }

            core -= "tools/qunicodetables.cpp";
            core -= "codecs/qicucodec.cpp";

            core -= "kernel/qjnionload.cpp";
            core -= "kernel/qjni.cpp";
            core -= "kernel/qjnihelpers.cpp";

            core -= "kernel/qpoll.cpp";
            core -= "kernel/qcfsocketnotifier.cpp";
            core -= "kernel/qppsobject.*"_r;
            core -= "io/forkfd_qt.cpp";

            // remove header only
            core -= "tools/qchar.cpp";
            core -= "tools/qstringmatcher.cpp";

            //
            core -= "kernel/qtcore_eval.cpp";

            core.Private +=
                "serialization"_id;
            core.Private +=
                "kernel"_id;
            core.Private +=
                "global"_id;

            Definition d;
            d.d = "QT_VERSION_MAJOR=" + core.Variables["PACKAGE_VERSION_MAJOR"].toString();
            core.Public += d;
            d.d = "QT_VERSION_MINOR=" + core.Variables["PACKAGE_VERSION_MINOR"].toString();
            core.Public += d;
            d.d = "QT_VERSION_PATCH=" + core.Variables["PACKAGE_VERSION_PATCH"].toString();
            core.Public += d;
            d.d = "QT_VERSION_STR=\"" + core.getPackage().getVersion().toString() + "\"";
            core.Public += d;

            core.Private += "QT_BUILD_CORE_LIB"_d;
            core.Public += "QT_COMPILER_SUPPORTS_SIMD_ALWAYS"_d;
            core.Public += "QT_USE_QSTRINGBUILDER"_d;
            if (s.Settings.Native.CompilerType == CompilerType::MSVC)
            {
                core.Public += "_ENABLE_EXTENDED_ALIGNED_STORAGE"_d;
            }
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                core.Private += "UNICODE"_d;
                core.Public += "WIN32"_d;
            }
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                core.Public += "Mincore.lib"_lib;
                core.Public += "Mpr.lib"_lib;
                core.Public += "winmm.lib"_lib;
                core.Public += "User32.lib"_lib;
                core.Public += "Netapi32.lib"_lib, "Advapi32.lib"_lib, "Ole32.lib"_lib, "Shell32.lib"_lib;
            }
            core.Public += sw::Shared, "QT_SHARED"_d;
            core.Public += sw::Static, "QT_STATIC"_d;

            core.Public += "org.sw.demo.google.double_conversion-*"_dep;
            core.Public += "org.sw.demo.madler.zlib-1"_dep;
            core.Public += "org.sw.demo.pcre.pcre16-10"_dep;
            core += harfbuzz;

            String module = "QtCore";
            String module_lower = module;
            std::transform(module_lower.begin(), module_lower.end(), module_lower.begin(), tolower);
            auto &t = core;

            // global
            t.writeFileOnce(module + "/QtConfig", "#include \"qconfig.h\"");
            t.writeFileOnce(module + "/qconfig.h", R"xxx(
                #define QT_FEATURE_processenvironment 1
                #define QT_FEATURE_slog2 -1
                #define QT_FEATURE_syslog -1
                #define QT_FEATURE_journald -1
                #define QT_FEATURE_framework -1
                #define QT_FEATURE_rpath -1
                #define QT_FEATURE_accessibility 1
                #define QT_FEATURE_shared 1
                #define QT_COMPILER_SUPPORTS_SSE2 1
                #define QT_COMPILER_SUPPORTS_SSE3 1
                #define QT_COMPILER_SUPPORTS_SSSE3 1
                #define QT_COMPILER_SUPPORTS_SSE4_1 1
                #define QT_COMPILER_SUPPORTS_SSE4_2 1
                #define QT_COMPILER_SUPPORTS_AVX 1
                #define QT_COMPILER_SUPPORTS_AVX2 1
                #define QT_FEATURE_debug_and_release 1
                #define QT_FEATURE_build_all 1
                #define QT_FEATURE_c__11 1
                #define QT_FEATURE_c__14 -1
                #define QT_FEATURE_c__1z -1
                #define QT_FEATURE_concurrent 1
                #define QT_FEATURE_pkg_config -1
                #define QT_FEATURE_force_asserts -1
                #define QT_LARGEFILE_SUPPORT 64
                #define QT_NO_MREMAP
                #define QT_FEATURE_separate_debug_info -1
                #define QT_FEATURE_simulator_and_device -1
                #define QT_FEATURE_static -1

                #define QT_FEATURE_cxx11_future 1
                #define QT_FEATURE_std_atomic64 1
                #define QT_FEATURE_getentropy -1
                #define QT_FEATURE_datetimeparser 1

                #define QT_FEATURE_future 1
    )xxx");
            t.writeFileOnce(module + "/private/qconfig_p.h", R"xxx(
                #define QT_FEATURE_sse2 1
                #define QT_FEATURE_private_tests -1
                #define QT_FEATURE_dbus 1
                #define QT_FEATURE_dbus_linked -1
                #define QT_FEATURE_gui 1
                #define QT_FEATURE_libudev -1
                #define QT_FEATURE_posix_fallocate -1
                #define QT_FEATURE_reduce_exports -1
                #define QT_FEATURE_reduce_relocations -1
                #define QT_FEATURE_release_tools -1
                #define QT_FEATURE_system_zlib -1
                #define QT_FEATURE_widgets 1
    )xxx");

            // QtCore
            t.writeFileOnce(module + "/" + module_lower + "-config.h", R"xxx(
                #define QT_FEATURE_properties 1
                #define QT_FEATURE_animation 1
                #define QT_FEATURE_textcodec 1
                #define QT_FEATURE_big_codecs 1
                #define QT_NO_CLOCK_MONOTONIC
                #define QT_FEATURE_codecs 1
                #define QT_FEATURE_commandlineparser 1
                #define QT_FEATURE_textdate 1
                #define QT_FEATURE_datestring 1
                #define QT_NO_DYNAMIC_LIBRARY true
                #define QT_NO_EVENTFD
                #define QT_FEATURE_filesystemiterator 1
                #define QT_FEATURE_filesystemwatcher 1
                #define QT_FEATURE_gestures 1
                #define QT_NO_GLIB
                #define QT_NO_ICONV
                #define QT_FEATURE_itemmodel 1
                #define QT_FEATURE_proxymodel 1
                #define QT_FEATURE_identityproxymodel 1
                #define QT_NO_INOTIFY
                #define QT_FEATURE_library 1
                #define QT_FEATURE_mimetype 1
                #define QT_FEATURE_process 1
                #define QT_FEATURE_statemachine 1
                #define QT_FEATURE_qeventtransition 1
                #define QT_FEATURE_regularexpression 1
                #define QT_FEATURE_settings 1
                #define QT_FEATURE_sharedmemory 1
                #define QT_FEATURE_sortfilterproxymodel 1
                #define QT_FEATURE_stringlistmodel 1
                #define QT_FEATURE_systemsemaphore 1
                #define QT_FEATURE_temporaryfile 1
                #define QT_FEATURE_timezone 1
                #define QT_FEATURE_topleveldomain 1
                #define QT_FEATURE_translation 1
                #define QT_FEATURE_xmlstream 1
                #define QT_FEATURE_xmlstreamreader 1
                #define QT_FEATURE_xmlstreamwriter 1
    )xxx");

            t.writeFileOnce(module + "/private/" + module_lower + "-config_p.h", R"xxx(
                #define QT_FEATURE_clock_gettime -1
                #define QT_FEATURE_doubleconversion 1
                #define QT_FEATURE_glib -1
                #define QT_FEATURE_gnu_libiconv -1
                #define QT_FEATURE_icu -1
                #define QT_FEATURE_posix_libiconv -1
                #define QT_FEATURE_iconv -1
                #define QT_FEATURE_inotify -1
                #define QT_FEATURE_libatomic -1
                #define QT_FEATURE_sha3_fast 1
                #define QT_FEATURE_system_doubleconversion -1
                #define QT_FEATURE_system_pcre -1
    )xxx");

            t.writeFileOnce(module + "/" + module + "Depends");

            core.writeFileOnce(core.BinaryDir / "qconfig.cpp", R"xxx(
                /* Build date */
                static const char qt_configure_installation          [11  + 12] = "qt_instdate=2017-01-01";

                /* Installation Info */
                static const char qt_configure_prefix_path_str       [512 + 12] = "qt_prfxpath=.";
                #ifdef QT_BUILD_QMAKE
                static const char qt_configure_ext_prefix_path_str   [512 + 12] = "qt_epfxpath=.";
                static const char qt_configure_host_prefix_path_str  [512 + 12] = "qt_hpfxpath=.";
                #endif

                static const short qt_configure_str_offsets[] = {
                     0, 4, 12, 16, 20, 24, 32, 40, 44, 46, 48, 61, 70,
                #ifdef QT_BUILD_QMAKE
                     76, 77, 81, 85, 87, 102,
                #endif
                };
                static const char qt_configure_strs[] =
                    "doc\0"
                    "include\0"
                    "lib\0"
                    "bin\0"
                    "bin\0"
                    "plugins\0"
                    "imports\0"
                    "qml\0"
                    ".\0"
                    ".\0"
                    "translations\0"
                    "examples\0"
                    "tests\0"
                #ifdef QT_BUILD_QMAKE
                    "\0"
                    "bin\0"
                    "lib\0"
                    ".\0"
                    "win32-msvc2015\0"
                    "win32-msvc2015\0"
                #endif
                ;

                #ifdef QT_BUILD_QMAKE
                # define QT_CONFIGURE_SYSROOTIFY_PREFIX false
                #endif

                #define QT_CONFIGURE_PREFIX_PATH qt_configure_prefix_path_str + 12
                #ifdef QT_BUILD_QMAKE
                # define QT_CONFIGURE_EXT_PREFIX_PATH qt_configure_ext_prefix_path_str + 12
                # define QT_CONFIGURE_HOST_PREFIX_PATH qt_configure_host_prefix_path_str + 12
                #endif
    )xxx");
            core += core.BinaryDir / "qconfig.cpp";

            write_tracepoints(core);
            platform_files(core);

            automoc(moc, core);
            ::rcc(rcc, core, core.SourceDir / "mimetypes/mimetypes.qrc");
        }

        auto &gui = base.addTarget<LibraryTarget>("gui");
        {
            syncqt(gui, { "QtGui" });
            gui.SourceDir /= "src/gui";
            gui +=
                "accessible/.*"_rr,
                "animation/.*"_rr,
                "image/.*"_rr,
                "itemmodels/.*"_rr,
                "kernel/.*"_rr,
                "math3d/.*"_rr,
                "opengl/.*"_rr,
                "painting/.*"_rr,
                "text/.*"_rr,
                "util/.*"_rr;

            gui -= "text/qcssscanner.cpp";

            gui.Public += "src"_id;
            gui.Private += "QT_BUILD_GUI_LIB"_d;

            gui.Public += "org.sw.demo.glennrp.png-1"_dep;
            gui.Public += "org.sw.demo.behdad.harfbuzz-1"_dep;
            gui.Public += core;

            String module = "QtGui";
            String module_lower = module;
            std::transform(module_lower.begin(), module_lower.end(), module_lower.begin(), tolower);
            auto &t = gui;

            t.writeFileOnce(module + "/" + module_lower + "global_p.h", R"xxx(
                #pragma once

                #include <QtGui/qtguiglobal.h>
                #include <QtCore/private/qglobal_p.h>
                #include <QtGui/private/qtgui-config_p.h>
    )xxx");

            t.writeFileOnce(module + "/" + module_lower + "-config.h", R"xxx(
                #define QT_FEATURE_accessibility 1
                #define QT_NO_ACCESSIBILITY_ATSPI_BRIDGE
                #define QT_FEATURE_action 1
                #define QT_FEATURE_opengles2 -1
                #define QT_FEATURE_dynamicgl 1
                #define QT_OPENGL_DYNAMIC true
                #define QT_FEATURE_angle 1
                #define QT_OPENGL_ES_2_ANGLE true
                #define QT_FEATURE_clipboard 1
                #define QT_FEATURE_colornames 1
                #define QT_FEATURE_cssparser 1
                #define QT_FEATURE_cursor 1
                #define QT_FEATURE_desktopservices 1
                #define QT_FEATURE_imageformat_xpm 1
                #define QT_FEATURE_draganddrop 1
                #define QT_FEATURE_opengl 1
                #define QT_NO_FONTCONFIG
                #define QT_FEATURE_highdpiscaling 1
                #define QT_FEATURE_im 1
                #define QT_FEATURE_image_heuristic_mask 1
                #define QT_FEATURE_image_text 1
                #define QT_FEATURE_imageformat_bmp 1
                #define QT_FEATURE_imageformat_jpeg 1
                #define QT_FEATURE_imageformat_png 1
                #define QT_FEATURE_imageformat_ppm 1
                #define QT_FEATURE_imageformat_xbm 1
                #define QT_FEATURE_imageformatplugin 1
                #define QT_FEATURE_movie 1
                #define QT_FEATURE_opengles3 -1
                #define QT_FEATURE_opengles31 -1
                #define QT_FEATURE_pdf 1
                #define QT_FEATURE_picture 1
                #define QT_QPA_DEFAULT_PLATFORM_NAME "windows"
                #define QT_FEATURE_sessionmanager 1
                #define QT_FEATURE_shortcut 1
                #define QT_FEATURE_standarditemmodel 1
                #define QT_FEATURE_systemtrayicon 1
                #define QT_FEATURE_tabletevent 1
                #define QT_FEATURE_texthtmlparser 1
                #define QT_FEATURE_textodfwriter 1
                #define QT_FEATURE_validator 1
                #define QT_FEATURE_wheelevent 1
                #define QT_NO_XRENDER

                #define QT_FEATURE_whatsthis 1
                #define QT_FEATURE_vulkan -1
    )xxx");

            t.writeFileOnce(module + "/private/" + module_lower + "-config_p.h", R"xxx(
                #define QT_FEATURE_xcb -1
                #define QT_FEATURE_accessibility_atspi_bridge -1
                #define QT_FEATURE_android_style_assets -1
                #define QT_FEATURE_direct2d 1
                #define QT_FEATURE_directfb -1
                #define QT_FEATURE_directwrite 1
                #define QT_FEATURE_directwrite2 1
                #define QT_FEATURE_egl 1
                #define QT_FEATURE_egl_x11 -1
                #define QT_FEATURE_eglfs -1
                #define QT_FEATURE_eglfs_brcm -1
                #define QT_FEATURE_eglfs_egldevice -1
                #define QT_FEATURE_eglfs_gbm -1
                #define QT_FEATURE_eglfs_mali -1
                #define QT_FEATURE_eglfs_viv -1
                #define QT_FEATURE_eglfs_viv_wl -1
                #define QT_FEATURE_evdev -1
                #define QT_FEATURE_fontconfig -1
                #define QT_FEATURE_freetype 1
                #define QT_FEATURE_gif 1
                #define QT_FEATURE_harfbuzz 1
                #define QT_FEATURE_ico 1
                #define QT_FEATURE_integrityfb -1
                #define QT_FEATURE_jpeg 1
                #define QT_FEATURE_libinput -1
                #define QT_FEATURE_libinput_axis_api -1
                #define QT_FEATURE_linuxfb -1
                #define QT_FEATURE_mirclient -1
                #define QT_FEATURE_mtdev -1
                #define QT_FEATURE_multiprocess 1
                #define QT_FEATURE_png 1
                #define QT_FEATURE_system_freetype -1
                #define QT_FEATURE_system_harfbuzz -1
                #define QT_FEATURE_system_jpeg -1
                #define QT_FEATURE_system_png -1
                #define QT_FEATURE_system_xcb -1
                #define QT_FEATURE_tslib -1
                #define QT_FEATURE_xinput2 -1
                #define QT_FEATURE_xkb -1
                #define QT_FEATURE_xkbcommon_evdev -1
                #define QT_FEATURE_xlib -1
                #define QT_FEATURE_xrender -1
    )xxx");

            t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                #ifdef __cplusplus
                #include <QtCore/QtCore>
                #endif
    )xxx");

            write_tracepoints(gui);
            platform_files(gui);

            gui.replaceInFileOnce("text/qharfbuzzng_p.h", "#include <harfbuzz/hb.h>", "#include <hb.h>");
            gui.replaceInFileOnce("text/qfontengine.cpp", "#  include <harfbuzz/hb-ot.h>", "#include <hb-ot.h>");

            automoc(moc, gui);
        }

        auto &widgets = base.addTarget<LibraryTarget>("widgets");
        {
            syncqt(widgets, { "QtWidgets" });
            widgets.SourceDir /= "src/widgets";
            widgets +=
                "accessible/.*"_rr,
                "dialogs/.*"_rr,
                "effects/.*"_rr,
                "graphicsview/.*"_rr,
                "itemviews/.*"_rr,
                "kernel/.*"_rr,
                "statemachine/.*"_rr,
                "styles/.*"_rr,
                "util/.*"_rr,
                "widgets/.*"_rr;

            widgets.Private += "QT_BUILD_WIDGETS_LIB"_d;
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                widgets.Private += "QT_NO_STYLE_ANDROID"_d;
                widgets.Private += "QT_NO_STYLE_FUSION"_d;
                widgets.Private += "QT_NO_STYLE_MAC"_d;
                widgets.Private += "UNICODE"_d;
            }
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                widgets.Public += "dwmapi.lib"_lib;
                widgets.Public += "uxtheme.lib"_lib;
            }

            widgets.Public += gui;

            String module = "QtWidgets";
            String module_lower = module;
            std::transform(module_lower.begin(), module_lower.end(), module_lower.begin(), tolower);
            auto &t = widgets;

            t.writeFileOnce(module + "/" + module_lower + "-config.h", R"xxx(
                #define QT_FEATURE_abstractslider 1
                #define QT_FEATURE_abstractbutton 1
                #define QT_FEATURE_widgettextcontrol 1
                #define QT_FEATURE_dialog 1
                #define QT_FEATURE_label 1
                #define QT_FEATURE_checkbox 1
                #define QT_FEATURE_pushbutton 1
                #define QT_FEATURE_radiobutton 1
                #define QT_FEATURE_dialogbuttonbox 1
                #define QT_FEATURE_scroller 1
                #define QT_FEATURE_gestures 1
                #define QT_FEATURE_formlayout 1
                #define QT_FEATURE_commandlinkbutton 1
                #if defined(_WIN32) || defined(WIN32)
                #define QT_FEATURE_style_windows 1
                #define QT_FEATURE_style_windowsvista 1
                #define QT_FEATURE_style_windowsxp 1
                #endif
                #define QT_FEATURE_style_fusion -1
                #define QT_FEATURE_style_mac -1
                #define QT_FEATURE_style_android -1

                #define QT_FEATURE_groupbox 1
                #define QT_FEATURE_buttongroup 1
                #define QT_FEATURE_rubberband 1
                #define QT_FEATURE_slider 1
                #define QT_FEATURE_scrollbar 1
                #define QT_FEATURE_scrollarea 1
                #define QT_FEATURE_itemviews 1
                #define QT_FEATURE_tableview 1
                #define QT_FEATURE_menu 1
                #define QT_FEATURE_spinbox 1
                #define QT_FEATURE_toolbutton 1
                #define QT_FEATURE_calendarwidget 1
                #define QT_FEATURE_colordialog 1
                #define QT_FEATURE_listview 1
                #define QT_FEATURE_columnview 1
                #define QT_FEATURE_lineedit 1
                #define QT_FEATURE_combobox 1
                #define QT_FEATURE_completer 1
                #define QT_FEATURE_contextmenu 1
                #define QT_FEATURE_datawidgetmapper 1
                #define QT_FEATURE_datetimeedit 1
                #define QT_FEATURE_dial 1
                #define QT_FEATURE_filesystemmodel 1
                #define QT_FEATURE_dirmodel 1
                #define QT_FEATURE_resizehandler 1
                #define QT_FEATURE_mainwindow 1
                #define QT_FEATURE_dockwidget 1
                #define QT_FEATURE_textedit 1
                #define QT_FEATURE_errormessage 1
                #define QT_FEATURE_treeview 1
                #define QT_FEATURE_tooltip 1
                #define QT_FEATURE_splitter 1
                #define QT_FEATURE_stackedwidget 1
                #define QT_FEATURE_filedialog 1
                #define QT_FEATURE_fontcombobox 1
                #define QT_FEATURE_fontdialog 1
                #define QT_FEATURE_fscompleter 1
                #define QT_FEATURE_graphicsview 1
                #define QT_FEATURE_graphicseffect 1
                #define QT_FEATURE_inputdialog 1
                #define QT_FEATURE_keysequenceedit 1
                #define QT_FEATURE_lcdnumber 1
                #define QT_FEATURE_listwidget 1
                #define QT_FEATURE_mdiarea 1
                #define QT_FEATURE_menubar 1
                #define QT_FEATURE_messagebox 1
                #define QT_FEATURE_paint_debug 1
                #define QT_FEATURE_progressbar 1
                #define QT_FEATURE_progressdialog 1
                #define QT_FEATURE_sizegrip 1
                #define QT_FEATURE_splashscreen 1
                #define QT_FEATURE_statusbar 1
                #define QT_FEATURE_statustip 1
                #define QT_FEATURE_style_stylesheet 1
                #define QT_FEATURE_syntaxhighlighter 1
                #define QT_FEATURE_tabbar 1
                #define QT_FEATURE_tablewidget 1
                #define QT_FEATURE_tabwidget 1
                #define QT_FEATURE_textbrowser 1
                #define QT_FEATURE_toolbar 1
                #define QT_FEATURE_toolbox 1
                #define QT_FEATURE_treewidget 1
                #define QT_FEATURE_undocommand 1
                #define QT_FEATURE_undostack 1
                #define QT_FEATURE_undogroup 1
                #define QT_FEATURE_undoview 1
                #define QT_FEATURE_whatsthis 1
                #define QT_FEATURE_wizard 1
                #define QT_FEATURE_effects 1
    )xxx");

            t.writeFileOnce(module + "/private/" + module_lower + "-config_p.h", R"xxx(
                #define QT_FEATURE_gtk3 -1
    )xxx");

            t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                #ifdef __cplusplus
                #include <QtCore/QtCore>
                #include <QtGui/QtGui>
                #endif
    )xxx");

            platform_files(widgets);

            automoc(moc, widgets);
            ::rcc(rcc, widgets, Files{ widgets.SourceDir / "styles/qstyle.qrc", widgets.SourceDir / "dialogs/qmessagebox.qrc" });
            ::uic(uic, widgets, widgets.SourceDir / "dialogs/qfiledialog.ui");
        }

        auto &network = base.addTarget<LibraryTarget>("network");
        {
            syncqt(network, { "QtNetwork" });
            network.SourceDir /= "src/network";
            network +=
                "access/.*"_rr,
                "bearer/.*"_rr,
                "kernel/.*"_rr,
                "socket/.*"_rr,
                "ssl/.*"_rr;

            network -=
                "socket/qlocalserver_tcp.*"_rr,
                "socket/qlocalsocket_tcp.*"_rr,
                "socket/qsctpserver.*"_rr,
                "socket/qsctpsocket.*"_rr;

            network -= "ssl/qsslcertificate_qt.cpp"_rr;
            network -= "ssl/qsslkey_qt.cpp"_rr;
            network -= ".*_opensslpre11.*"_rr;
            network -= ".*_libproxy.*"_rr;
            network -= ".*_dummy.*"_rr;

            network.Private +=
                "kernel"_id;

            network.Private += "QT_BUILD_NETWORK_LIB"_d;
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                network.Private += "UNICODE"_d;
                network.Public += "WIN32"_d;
            }
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                network.Public += "Crypt32.lib"_lib;
                network.Public += "Dnsapi.lib"_lib;
                network.Public += "Iphlpapi.lib"_lib;
            }

            network.Public += core;
            network.Public += "org.sw.demo.openssl.ssl-1.*.*.*"_dep;

            String module = "QtNetwork";
            String module_lower = module;
            std::transform(module_lower.begin(), module_lower.end(), module_lower.begin(), tolower);
            auto &t = network;

            t.writeFileOnce(module + "/" + module_lower + "-config.h", R"xxx(
                #define QT_FEATURE_networkinterface 1
                #define QT_FEATURE_bearermanagement 1
                #define QT_FEATURE_ftp 1
                #define QT_NO_GETADDRINFO
                #define QT_NO_GETIFADDRS
                #define QT_FEATURE_http 1
                #define QT_NO_IPV6IFNAME
                #define QT_FEATURE_localserver 1
                #define QT_FEATURE_networkdiskcache 1
                #define QT_FEATURE_networkproxy 1
                #define QT_FEATURE_sctp -1
                #define QT_NO_SCTP
                #define QT_FEATURE_socks5 1
                #define QT_FEATURE_ssl 1
                #define QT_FEATURE_udpsocket 1
                #define QT_FEATURE_opensslv11 1
                #define QT_FEATURE_dnslookup 1
    )xxx");

            t.writeFileOnce(module + "/private/" + module_lower + "-config_p.h", R"xxx(
                #define QT_FEATURE_libproxy -1
                #define QT_FEATURE_securetransport -1
                #define QT_FEATURE_openssl 1
                #define QT_FEATURE_openssl_linked -1 // 1?
                #define QT_FEATURE_system_proxies 1
    )xxx");

            t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                #ifdef __cplusplus
                #include <QtCore/QtCore>
                #endif
    )xxx");

            if (!t.PostponeFileResolving)
            {
                auto dummy = network.BinaryDir / "private/sw_copy_headers.txt";
                if (!fs::exists(dummy))
                {
                    auto dir1 = network.BinaryDir / "QtNetwork/private";
                    auto dir2 = network.BinaryDir / "private";
                    fs::create_directories(dir1);
                    fs::create_directories(dir2);
                    for (auto &f : fs::recursive_directory_iterator(network.SourceDir / "access/http2"))
                    {
                        fs::copy_file(f, dir1 / f.path().filename(), fs::copy_options::overwrite_existing);
                        fs::copy_file(f, dir2 / f.path().filename(), fs::copy_options::overwrite_existing);
                    }
                    write_file(dummy, "");
                }
            }

            platform_files(network);
            automoc(moc, network);
        }

        auto &xml = base.addTarget<LibraryTarget>("xml");
        {
            syncqt(xml, { "QtXml" });
            xml.SourceDir /= "src/xml";
            xml +=
                "dom/qdom.*"_rr,
                "qtxmlglobal.h",
                "sax/qxml.*"_rr;

            xml.Private += "QT_BUILD_XML_LIB"_d;
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                xml.Private += "UNICODE"_d;
            }

            xml.Public += core;

            String module = "QtXml";
            String module_lower = module;
            std::transform(module_lower.begin(), module_lower.end(), module_lower.begin(), tolower);
            auto &t = xml;

            t.writeFileOnce(module + "/" + module_lower + "-config.h", R"xxx(
                #define QT_FEATURE_dom 1
    )xxx");

            t.writeFileOnce(module + "/private/" + module_lower + "-config_p.h", R"xxx(
    )xxx");

            t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                #ifdef __cplusplus
                #include <QtCore/QtCore>
                #endif
    )xxx");
        }

        auto &platformsupport = base.addDirectory("platformsupport");

        auto &accessibility = platformsupport.addTarget<StaticLibraryTarget>("accessibility");
        auto &eventdispatchers = platformsupport.addTarget<StaticLibraryTarget>("eventdispatchers");
        auto &themes = platformsupport.addTarget<StaticLibraryTarget>("themes");
        auto &windowsuiautomation = platformsupport.addTarget<StaticLibraryTarget>("windowsuiautomation");

        auto &fontdatabases = platformsupport.addDirectory("fontdatabases");
        auto &fontdatabases_windows = fontdatabases.addTarget<StaticLibraryTarget>("windows");

        // platformsupport
        {
            // accessibility
            {
                accessibility += "src/platformsupport/accessibility/.*"_rr;
                if (s.Settings.TargetOS.Type == OSType::Windows)
                {
                    accessibility.Private += "UNICODE"_d;
                }

                accessibility.Public += gui;

                String module = "QtAccessibilitySupport";
                auto &t = accessibility;

                syncqt(accessibility, { module });

                t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                #ifdef __cplusplus
                #include <QtCore/QtCore>
                #include <QtGui/QtGui>
                #endif
    )xxx");
            }

            // eventdispatchers
            {
                String module = "QtEventDispatcherSupport";
                syncqt(eventdispatchers, { module });

                eventdispatchers.SourceDir /= "src/platformsupport/eventdispatchers";
                eventdispatchers += ".*"_rr;
                eventdispatchers -= ".*_glib.*"_rr;
                eventdispatchers -= ".*_qpa.*"_rr;
                if (s.Settings.TargetOS.Type == OSType::Windows)
                {
                    eventdispatchers.Private += "UNICODE"_d;
                    eventdispatchers -= ".*unix.*"_rr;
                }
                else
                    eventdispatchers -= ".*windows.*"_rr;

                eventdispatchers.Public += gui;
                auto &t = eventdispatchers;

                automoc(moc, eventdispatchers);

                t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                #ifdef __cplusplus
                #include <QtCore/QtCore>
                #include <QtGui/QtGui>
                #endif
    )xxx");
            }

            // themes
            {
                themes +=
                    "src/platformsupport/themes/[^/]*"_rr;

                themes.Public +=
                    "src"_id;

                if (s.Settings.TargetOS.Type == OSType::Windows)
                {
                    themes.Private += "UNICODE"_d;
                }

                themes.Public += gui;

                String module = "QtThemeSupport";
                syncqt(themes, { module });
                themes.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                #ifdef __cplusplus
                #include <QtCore/QtCore>
                #include <QtGui/QtGui>
                #endif
    )xxx");
            }

            // windowsuiautomation
            {
                windowsuiautomation +=
                    "src/platformsupport/windowsuiautomation/.*"_rr,
                    "src/plugins/platforms/windows/.*\\.h"_rr;

                windowsuiautomation.Private +=
                    "src/plugins/platforms/windows"_id;

                if (s.Settings.TargetOS.Type == OSType::Windows)
                {
                    windowsuiautomation.Private += "UNICODE"_d;
                }

                windowsuiautomation.Public += gui;

                String module = "QtWindowsUIAutomationSupport";
                syncqt(windowsuiautomation, { module });
                windowsuiautomation.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                    #ifdef __cplusplus
                    #include <QtCore/QtCore>
                    #include <QtGui/QtGui>
                    #endif
    )xxx");
            }

            // fontdatabases
            {
                auto &freetype = fontdatabases.addTarget<StaticLibraryTarget>("freetype");
                {
                    String module = "QtFontDatabaseSupport";
                    syncqt(freetype, { module });

                    freetype +=
                        "src/platformsupport/fontdatabases/freetype/.*"_rr;

                    if (s.Settings.TargetOS.Type == OSType::Windows)
                    {
                        freetype.Private += "UNICODE"_d;
                    }

                    freetype.Public += gui;

                    freetype.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                #ifdef __cplusplus
                #include <QtCore/QtCore>
                #include <QtGui/QtGui>
                #endif
    )xxx");
                }

                // windows
                {
                    auto &windows = fontdatabases_windows;

                    String module = "QtFontDatabaseSupport";
                    syncqt(windows, { module });

                    windows +=
                        "src/platformsupport/fontdatabases/.*\\.h"_rr,
                        "src/platformsupport/fontdatabases/windows/.*"_rr;

                    windows.Public +=
                        "src"_id;

                    if (s.Settings.TargetOS.Type == OSType::Windows)
                    {
                        windows.Private += "QT_USE_DIRECTWRITE2"_d;
                        windows.Private += "UNICODE"_d;
                    }

                    windows.Public += freetype;

                    windows.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                #ifdef __cplusplus
                #include <QtCore/QtCore>
                #include <QtGui/QtGui>
                #endif
    )xxx");
                }
            }
        }

        auto &plugins = base.addDirectory("plugins");
        auto &platforms = plugins.addDirectory("platforms");
        auto &styles = plugins.addDirectory("styles");

        auto &windows = platforms.addTarget<LibraryTarget>("windows");
        {
            windows.setOutputDir("plugins/platforms");
            windows +=
                "src/plugins/platforms/windows/.*"_rr;

            windows -=
                "src/plugins/platforms/windows/qwindowsvulkaninstance.cpp";

            windows.Public +=
                "src/plugins/platforms/windows"_id;

            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                windows.Private += "UNICODE"_d;
            }
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                windows.Public += "Dwmapi.lib"_lib;
                windows.Public += "Imm32.lib"_lib;
                windows.Public += "UxTheme.lib"_lib;
                windows.Public += "Winmm.lib"_lib;
                windows += "EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE=EGL_PLATFORM_ANGLE_DEVICE_TYPE_D3D_WARP_ANGLE"_d;
            }
            windows.Private += sw::Static, "QT_STATICPLUGIN"_d;

            windows.Public += accessibility;
            windows.Public += eventdispatchers;
            windows.Public += themes;
            windows.Public += windowsuiautomation;
            windows.Public += fontdatabases_windows;

            automoc(moc, windows);
            ::rcc(rcc, windows, Files{
                windows.SourceDir / "src/plugins/platforms/windows/cursors.qrc",
                windows.SourceDir / "src/plugins/platforms/windows/openglblacklists.qrc" });

            windows.Public += "org.sw.demo.google.angle.egl-*"_dep;
            windows.Public += wintab;
            windows.Public += iaccessible2;
        }

        auto &windowsvista = styles.addTarget<LibraryTarget>("windowsvista");
        {
            windowsvista.setOutputDir("plugins/styles");
            windowsvista +=
                "src/plugins/styles/windowsvista/.*"_rr;
            windowsvista.Public +=
                "src/plugins/styles/windowsvista"_id;

            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                windowsvista.Private += "UNICODE"_d;
            }
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                windowsvista.Public += "gdi32.lib"_lib;
                windowsvista.Public += "user32.lib"_lib;
            }
            windowsvista.Private += sw::Static, "QT_STATICPLUGIN"_d;

            windowsvista.Public += widgets;

            automoc(moc, windowsvista);
        }

        auto &winmain = base.addTarget<StaticLibraryTarget>("winmain");
        {
            winmain += "src/winmain/qtmain_win.cpp";
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                winmain.Private += "UNICODE"_d;
            }
            winmain.Public += core;
        }

        auto &printsupport = base.addTarget<LibraryTarget>("printsupport");
        {
            String module = "QtPrintSupport";
            String module_lower = module;
            std::transform(module_lower.begin(), module_lower.end(), module_lower.begin(), tolower);
            auto &t = printsupport;

            syncqt(printsupport, { module });
            printsupport.SourceDir /= "src/printsupport";

            printsupport +=
                "dialogs/.*"_rr,
                "kernel/.*"_rr,
                "widgets/.*"_rr;

            printsupport.Private += "QT_BUILD_PRINTSUPPORT_LIB"_d;
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                printsupport.Private += "UNICODE"_d;
                printsupport += "Winspool.lib"_lib;
                printsupport += "Comdlg32.lib"_lib;
                printsupport -= "kernel/qcups.cpp";
                printsupport -= "widgets/qcups.*"_rr;
            }

            printsupport.Public += widgets;

            t.writeFileOnce(module + "/" + module_lower + "-config.h", R"xxx(
                #define QT_NO_CUPS
                #define QT_NO_CUPSJOBWIDGET
                #define QT_FEATURE_printer 1
                #define QT_FEATURE_printdialog 1
                #define QT_FEATURE_printpreviewwidget 1
                #define QT_FEATURE_printpreviewdialog 1
    )xxx");

            t.writeFileOnce(module + "/private/" + module_lower + "-config_p.h", R"xxx(
                #define QT_FEATURE_cups -1
                #define QT_FEATURE_cupsjobwidget -1
    )xxx");

            t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                #ifdef __cplusplus
                #include <QtCore/QtCore>
                #include <QtGui/QtGui>
                #include <QtWidgets/QtWidgets>
                #endif
    )xxx");

            platform_files(printsupport);
            automoc(moc, printsupport);

            ::rcc(rcc, printsupport, printsupport.SourceDir / "dialogs/qprintdialog.qrc");

            for (auto &f : enumerate_files_like(printsupport.SourceDir / "dialogs", ".*\\.ui"))
                ::uic(uic, printsupport, f);
        }
    }
}
