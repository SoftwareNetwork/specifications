#pragma sw require header pub.egorpugin.primitives.tools.syncqt-master

#pragma sw header on
#pragma sw require pub.egorpugin.primitives.hash-master
#pragma sw include "sw_qt_rules.h"
#include "sw_qt_rules.h"
#pragma sw header off

#pragma sw include "sw_qt_features.h"
#include "sw_qt_features.h"

static int mime_preprocess_file(path i, path o)
{
    auto s = read_file(i);
    std::string out;
    out += "static const unsigned char mimetype_database[] = {";
    int in = 0;
    for (uint8_t c : s)
    {
        String h(2, 0);
        sprintf(&h[0], "%02x", c);
        out += "0x" + h + ",";
        if (++in % 25 == 0)
            out += "\n";
        else
            out += " ";
    }
    out += "};\n";
    out += "static constexpr size_t MimeTypeDatabaseOriginalSize = " + std::to_string(s.size()) + ";";
    write_file(o, out);

    return 0;
}
SW_DEFINE_VISIBLE_FUNCTION_JUMPPAD2(mime_preprocess_file)

static void platform_files(NativeExecutedTarget &t)
{
    // remove platform
    t.AllowEmptyRegexes = true;
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
    t.AllowEmptyRegexes = false;
};

void moc(NativeExecutedTarget &moc, NativeExecutedTarget &t/*, const std::vector<moc_file> &additions = {}*/)
{
    t.addRuleDependency("moc", moc.getDependency());
}

void rcc(NativeExecutedTarget &rcc, NativeExecutedTarget &t, const path &fn)
{
    ::rcc(rcc.getDependency(), t, fn);
}

void rcc(NativeExecutedTarget &rcc, NativeExecutedTarget &t, const Files &files)
{
    ::rcc(rcc.getDependency(), t, files);
}

void rcc(NativeExecutedTarget &rcc, NativeExecutedTarget &t, const RccData &d)
{
    ::rcc(rcc.getDependency(), t, d);
}

void uic(NativeExecutedTarget &uic, NativeExecutedTarget &t, const path &fn)
{
    ::uic(uic.getDependency(), t, fn);
}

void uic(NativeExecutedTarget &uic, NativeExecutedTarget &t, const Files &files)
{
    ::uic(uic.getDependency(), t, files);
}

template <class T>
T &add_subproject(Target &base, const String &name)
{
    auto qt_name = "qt" + name;
    auto &p = base.add<T>(name);
    p += Git("https://github.com/qt/" + qt_name, "v{M}.{m}.{p}");
    if (p.isLocal() && fs::exists(p.SourceDir / qt_name))
        p.setSourceDirectory(qt_name);
    return p;
}

void build(Solution &s)
{
    auto &qt = s.addProject("qtproject.qt", "5.15.0.1");

    auto &base = add_subproject<Project>(qt, "base");

    auto &base_tools = base.addDirectory("tools");
    auto &moc = base_tools.addTarget<ExecutableTarget>("moc");
    {
        auto p = std::make_unique<MocCompiler>();
        p->file = moc.getOutputFile();
        auto r = std::make_unique<MocRule>(std::move(p), moc);
        moc.setRule("moc", std::move(r));
    }
    auto &rcc = base_tools.addTarget<ExecutableTarget>("rcc");
    auto &uic = base_tools.addTarget<ExecutableTarget>("uic");
    auto &tracegen = base_tools.addTarget<ExecutableTarget>("tracegen");

    // to setup descs
    auto &core = base.addTarget<LibraryTarget>("core");
    auto &gui = base.addTarget<LibraryTarget>("gui");
    auto &network = base.addTarget<LibraryTarget>("network");
    auto &widgets = base.addTarget<LibraryTarget>("widgets");

    qt_desc.config.public_.definitions["QT_VERSION_MAJOR"] = core.Variables["PACKAGE_VERSION_MAJOR"].toString();
    qt_desc.config.public_.definitions["QT_VERSION_MINOR"] = core.Variables["PACKAGE_VERSION_MINOR"].toString();
    qt_desc.config.public_.definitions["QT_VERSION_PATCH"] = core.Variables["PACKAGE_VERSION_PATCH"].toString();
    qt_desc.config.public_.definitions["QT_VERSION_STR"] = "\"" + core.getPackage().getVersion().toString() + "\"";

    // early decls
    auto &xml = base.addTarget<LibraryTarget>("xml");
    auto &qlalr = base_tools.addTarget<ExecutableTarget>("qlalr");

    auto write_tracepoints = [&tracegen](auto &t)
    {
        auto c = t.addCommand();
        c << cmd::prog(tracegen);
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            c << "etw";
        else
            c << "lttng";
        c
            << cmd::in("qt" + t.getPackage().getPath().back() + ".tracepoints")
            << cmd::out("qt" + t.getPackage().getPath().back() + "_tracepoints_p.h")
            ;
    };

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
            if (harfbuzz.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                harfbuzz.Private += "UNICODE"_d;
                harfbuzz.Public += "WIN32"_d;
            }
        }

        auto &iaccessible2 = third_party.addTarget<StaticLibraryTarget>("iaccessible2");
        {
            iaccessible2 += "src/3rdparty/iaccessible2/generated/.*"_rr;
            iaccessible2.Public += "src/3rdparty/iaccessible2/generated"_idir;
            if (iaccessible2.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                iaccessible2.Private += "UNICODE"_d;
            }
            if (iaccessible2.getBuildSettings().TargetOS.is(ArchType::x86))
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

        auto &tinycbor = third_party.addTarget<StaticLibraryTarget>("tinycbor");
        {
            tinycbor.setRootDirectory("src/3rdparty/tinycbor");
        }

        auto &bootstrap = base_tools.addTarget<StaticLibraryTarget>("bootstrap");
        {
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
                    "src/corelib/global/qendian.cpp",
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
                    "src/corelib/io/qipaddress.cpp",
                    "src/corelib/io/qloggingcategory.cpp",
                    "src/corelib/io/qloggingregistry.cpp",
                    "src/corelib/io/qresource.cpp",
                    "src/corelib/io/qstandardpaths.cpp",
                    "src/corelib/io/qstandardpaths_mac.mm",
                    "src/corelib/io/qstandardpaths_unix.cpp",
                    "src/corelib/io/qstandardpaths_win.cpp",
                    "src/corelib/io/qtemporaryfile.cpp",
                    "src/corelib/io/qurl.cpp",
                    "src/corelib/io/qurlidna.cpp",
                    "src/corelib/io/qurlquery.cpp",
                    "src/corelib/io/qurlrecode.cpp",
                    "src/corelib/kernel/qcore_foundation.mm",
                    "src/corelib/kernel/qcoreapplication.cpp",
                    "src/corelib/kernel/qcoreapplication_mac.cpp",
                    "src/corelib/kernel/qcoreapplication_win.cpp",
                    "src/corelib/kernel/qcoreglobaldata.cpp",
                    "src/corelib/kernel/qmetatype.cpp",
                    "src/corelib/kernel/qsystemerror.cpp",
                    "src/corelib/kernel/qvariant.cpp",
                    "src/corelib/plugin/qsystemlibrary.cpp",
                    "src/corelib/plugin/quuid.cpp",
                    "src/corelib/serialization/qcborstreamwriter.cpp",
                    "src/corelib/serialization/qcborvalue.cpp",
                    "src/corelib/serialization/qdatastream.cpp",
                    "src/corelib/serialization/qjsonarray.cpp",
                    "src/corelib/serialization/qjsoncbor.cpp",
                    "src/corelib/serialization/qjsondocument.cpp",
                    "src/corelib/serialization/qjsonobject.cpp",
                    "src/corelib/serialization/qjsonparser.cpp",
                    "src/corelib/serialization/qjsonvalue.cpp",
                    "src/corelib/serialization/qjsonwriter.cpp",
                    "src/corelib/serialization/qtextstream.cpp",
                    "src/corelib/serialization/qxmlstream.cpp",
                    "src/corelib/serialization/qxmlutils.cpp",
                    "src/corelib/text/qbytearray.cpp",
                    "src/corelib/text/qbytearraylist.cpp",
                    "src/corelib/text/qbytearraymatcher.cpp",
                    "src/corelib/text/qchar.cpp",
                    "src/corelib/text/qlocale.cpp",
                    "src/corelib/text/qlocale_tools.cpp",
                    "src/corelib/text/qregexp.cpp",
                    "src/corelib/text/qstring.cpp",
                    "src/corelib/text/qstring_compat.cpp",
                    "src/corelib/text/qstringbuilder.cpp",
                    "src/corelib/text/qstringlist.cpp",
                    "src/corelib/text/qstringmatcher.cpp",
                    "src/corelib/text/qunicodetables.cpp",
                    "src/corelib/text/qvsnprintf.cpp",
                    "src/corelib/time/qcalen.*"_rr,
                    "src/corelib/time/qdatetime.cpp",
                    "src/corelib/time/qgregorian.*"_rr,
                    "src/corelib/time/qroman.*"_rr,
                    "src/corelib/tools/qarraydata.cpp",
                    "src/corelib/tools/qbitarray.cpp",
                    "src/corelib/tools/qcommandlineoption.cpp",
                    "src/corelib/tools/qcommandlineparser.cpp",
                    "src/corelib/tools/qcryptographichash.cpp",
                    "src/corelib/tools/qhash.cpp",
                    "src/corelib/tools/qline.cpp",
                    //"src/corelib/tools/qlinkedlist.cpp",
                    "src/corelib/tools/qlist.cpp",
                    "src/corelib/tools/qmap.cpp",
                    "src/corelib/tools/qpoint.cpp",
                    "src/corelib/tools/qrect.cpp",
                    "src/corelib/tools/qringbuffer.cpp",
                    "src/corelib/tools/qsize.cpp",
                    "src/corelib/tools/qversionnumber.cpp",
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
                    "src/corelib/kernel/qcoreapplication_mac.cpp",
                    "src/corelib/kernel/qcoreapplication_win.cpp",
                    "src/corelib/plugin/qsystemlibrary.cpp",
                    "src/corelib/text/qchar.cpp",
                    "src/corelib/text/qstringmatcher.cpp",
                    "src/corelib/text/qunicodetables.cpp";

                // included via qcborvalue.cpp
                bootstrap -= "src/corelib/serialization/qcborarray.cpp";
                bootstrap -= "src/corelib/serialization/qcbormap.cpp";

                bootstrap.Private +=
                    "src/xml"_id,
                    "src/xml/dom"_id,
                    "src/xml/sax"_id;
            }

            Definition d;
            d.d = "QT_VERSION_MAJOR=" + bootstrap.Variables["PACKAGE_VERSION_MAJOR"].toString();
            bootstrap.Public += d;
            d.d = "QT_VERSION_MINOR=" + bootstrap.Variables["PACKAGE_VERSION_MINOR"].toString();
            bootstrap.Public += d;
            d.d = "QT_VERSION_PATCH=" + bootstrap.Variables["PACKAGE_VERSION_PATCH"].toString();
            bootstrap.Public += d;
            d.d = "QT_VERSION_STR=\"" + bootstrap.getPackage().getVersion().toString() + "\"";
            bootstrap.Public += d;

            bootstrap.Public += "QT_BOOTSTRAPPED"_d;
            //bootstrap.Public += "QT_NO_QOBJECT"_d;
            bootstrap.Public += "QT_NO_CAST_FROM_ASCII"_d;
            bootstrap.Public += "QT_NO_CAST_TO_ASCII"_d;
            bootstrap.Public += "QT_NO_FOREACH"_d;
            bootstrap.Public += "QT_NO_LINKED_LIST"_def;
            bootstrap.Public += "QT_USE_QSTRINGBUILDER"_def;
            if (bootstrap.getCompilerType() == CompilerType::MSVC)
            {
                bootstrap.Public += "_ENABLE_EXTENDED_ALIGNED_STORAGE"_d;
            }
            if (bootstrap.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                bootstrap.Private += "UNICODE"_d;
                bootstrap.Public += "WIN32"_d;
            }
            if (bootstrap.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                bootstrap.Public += "Netapi32.lib"_slib, "Advapi32.lib"_slib, "Ole32.lib"_slib, "Shell32.lib"_slib, "uuid.lib"_slib;
            }

            bootstrap.Public += tinycbor;
            bootstrap.Public += "org.sw.demo.madler.zlib"_dep;

            qt_desc.print(bootstrap);

            if (bootstrap.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                bootstrap +=
                    "src/corelib/io/qfilesystemengine_win.cpp",
                    "src/corelib/io/qfilesystemiterator_win.cpp",
                    "src/corelib/io/qfsfileengine_win.cpp",
                    "src/corelib/kernel/qcoreapplication_win.cpp",
                    "src/corelib/kernel/qwinreg.*"_rr,
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

            if (bootstrap.getBuildSettings().TargetOS.Type == OSType::Macos)
            {
                bootstrap +=
                    "src/corelib/kernel/qcoreapplication_mac.cpp",
                    "src/corelib/kernel/qcore_mac_objc.cpp",
                    "src/corelib/kernel/qcore_foundation.cpp",
                    "src/corelib/global/qoperatingsystemversion_darwin.mm",
                    "src/corelib/kernel/qcore_foundation.cpp",
                    "src/corelib/io/qstandardpaths_mac.mm"
                    ;
            }

            auto c = bootstrap.addCommand(SW_VISIBLE_BUILTIN_FUNCTION(copy_file));
            c << cmd::in("src/xml/sax/qxml_p.h") << cmd::out("private/qxml_p.h");
            syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, bootstrap, { "QtCore", "QtXml" });

            if (bootstrap.getCompilerType() == CompilerType::MSVC)
                bootstrap.Public += "mkspecs/win32-msvc"_idir;
        }

        // moc
        {
            moc +=
                "src/tools/moc/[^/]*\\.cpp"_rr,
                "src/tools/moc/[^/]*\\.h"_rr;
            moc -=
                "src/tools/moc/keywords.cpp",
                "src/tools/moc/ppkeywords.cpp";
            moc.Public += "QT_MOC"_d;
            //moc.Public += "QT_NO_CAST_FROM_ASCII"_d;
            moc.Public += "QT_NO_CAST_FROM_BYTEARRAY"_d;
            moc.Public += "QT_NO_COMPRESS"_d;
            moc.Public += "QT_NO_FOREACH"_d;
            if (moc.getBuildSettings().TargetOS.Type == OSType::Windows)
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
            //rcc.Public += "QT_NO_CAST_FROM_ASCII"_d;
            rcc.Public += "QT_NO_FOREACH"_d;
            rcc.Public += "QT_RCC"_d;
            if (rcc.getBuildSettings().TargetOS.Type == OSType::Windows)
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
            uic.Public += "src/tools/uic"_id;
            uic.Public += "src/tools/uic/cpp"_id;
            uic.Public += "src/tools/uic/python"_id;
            uic.Public += "src/tools/uic/shared"_id;
            //uic.Public += "QT_NO_CAST_FROM_ASCII"_d;
            uic.Public += "QT_NO_FOREACH"_d;
            uic.Public += "QT_UIC"_d;
            uic.Public += "QT_UIC_CPP_GENERATOR"_d;
            if (uic.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                uic.Private += "UNICODE"_d;
                uic.Public += "WIN32"_d;
            }
            uic.Public += bootstrap;
        }

        // qlalr
        {
            qlalr +=
                "src/tools/qlalr/[^/]*\\.cpp"_rr,
                "src/tools/qlalr/[^/]*\\.[gh]"_rr;
            qlalr.Public += "src/tools/qlalr"_id;
            if (qlalr.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                qlalr.Private += "UNICODE"_d;
                qlalr.Public += "WIN32"_d;
            }
            qlalr.Public += bootstrap;
        }

        // tracegen
        {
            tracegen +=
                "src/tools/tracegen/[^/]*\\.cpp"_rr,
                "src/tools/tracegen/[^/]*\\.[h]"_rr;
            tracegen.Public += "src/tools/tracegen"_id;
            if (tracegen.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                tracegen.Private += "UNICODE"_d;
                tracegen.Public += "WIN32"_d;
            }
            tracegen.Public += bootstrap;

            tracegen.pushFrontToFileOnce("src/tools/tracegen/provider.h", "#include <qstringlist.h>");
        }

        // core
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
                "src/3rdparty/tinycbor/src/.*"_rr,
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
                "src/3rdparty/tinycbor/src/.*"_rr,
                "src/3rdparty/sha3/.*"_rr;
            core -=
                ".*\\.qdoc"_rr;
            core -= "src/3rdparty/sha3/.*"_rr;
            core += "src/3rdparty/tinycbor/src"_idir;

            if (core.getCompilerType() == CompilerType::MSVC)
                core.Public += "mkspecs/win32-msvc"_idir;

            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, core, { "QtCore", "QtPlatformHeaders" });

            SwapAndRestore sr(core.SourceDir);
            core.SourceDir /= "src/corelib";
            core +=
                "animation/.*"_rr,
                "codecs/.*"_rr,
                "global/.*"_rr,
                "io/.*"_rr,
                "itemmodels/.*"_rr,
                "kernel/.*"_rr,
                "mimetypes/.*"_rr,
                "plugin/.*"_rr,
                "serialization/.*"_rr,
                "statemachine/.*"_rr,
                "text/.*"_rr,
                "thread/.*"_rr,
                "time/.*"_rr,
                "tools/.*"_rr
                ;

            core -=
                "io/qfilesystemengine_unix.cpp",
                "io/qfilesystemengine_win.cpp",
                "io/qfilesystemiterator_unix.cpp",
                "io/qfilesystemiterator_win.cpp",
                "io/qfilesystemwatcher_.*\\.cpp"_rr,
                "io/qfsfileengine_unix.cpp",
                "io/qfsfileengine_win.cpp",
                "io/qsettings_wasm.cpp",
                "io/qstandardpaths_mac.mm",
                "io/qstandardpaths_unix.cpp",
                "io/qstandardpaths_win.cpp",
                "kernel/qcore_foundation.mm",
                "kernel/qcoreapplication_mac.cpp",
                "kernel/qcoreapplication_win.cpp",
                "plugin/qsystemlibrary.cpp",
                "thread/qmutex_.*"_rr;

            core -= "global/qfloat16ta.*"_rr;

            core += "io/qfilesystemwatcher_polling.*"_rr;

            if (core.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                core -= "codecs/qiconvcodec.cpp";
                core += "io/qfilesystemengine_win.cpp";
                core += "io/qfilesystemiterator_win.cpp";
                core += "io/qfilesystemwatcher_win.cpp";
                core += "io/qfsfileengine_win.cpp";
                core += "kernel/qcoreapplication_win.cpp";
                core += "plugin/qsystemlibrary.cpp";
                core += "io/qstandardpaths_win.cpp";
                core += "NOMINMAX"_def;
            }
            else
            {
                core += "io/qfilesystemengine_unix.cpp";
                core += "io/qfilesystemiterator_unix.cpp";
                core += "io/qfsfileengine_unix.cpp";
                core += "io/qstandardpaths_unix.cpp";
            }

            if (core.getBuildSettings().TargetOS.Type == OSType::Macos)
            {
                core += "kernel/qcoreapplication_mac.cpp";
                core += "kernel/qcore_mac_objc.cpp";
                core += "kernel/qcore_foundation.cpp";
                core += "kernel/qcore_foundation.cpp";
                core += "io/qstandardpaths_mac.mm";
            }

            core -= "codecs/qicucodec.cpp";

            core -= "kernel/qjnionload.cpp";
            core -= "kernel/qjni.cpp";
            core -= "kernel/qjnihelpers.cpp";

            core -= "kernel/qpoll.cpp";
            core -= "kernel/qcfsocketnotifier.cpp";
            core -= "kernel/qppsobject.*"_r;
            core -= "io/forkfd_qt.cpp";

            // remove header only
            core -= "text/qchar.cpp";
            core -= "text/qstringmatcher.cpp";
            core -= "text/qunicodetables.cpp";

            // included via qcborvalue.cpp
            core -= "serialization/qcborarray.cpp";
            core -= "serialization/qcbormap.cpp";

            core.Private +=
                "serialization"_id;
            core.Private +=
                "kernel"_id;
            core.Private +=
                "global"_id;
            core += IncludeDirectory(core.SourceDir);

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
            core.Protected += "QT_USE_QSTRINGBUILDER"_d;
            if (core.getCompilerType() == CompilerType::MSVC)
            {
                core.Public += "_ENABLE_EXTENDED_ALIGNED_STORAGE"_d;
                core.Public += "QT_COMPILER_SUPPORTS_F16C"_d;
            }
            if (core.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                core.Protected += "UNICODE"_d;
                core.Public += "WIN32"_d;
            }
            if (core.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                core.Public += "Mincore.lib"_slib;
                core.Public += "Mpr.lib"_slib;
                core.Public += "winmm.lib"_slib;
                core.Public += "User32.lib"_slib;
                core.Public += "uuid.lib"_slib;
                core.Public += "Netapi32.lib"_slib, "Advapi32.lib"_slib, "Ole32.lib"_slib, "Shell32.lib"_slib;
            }
            core.Public += sw::Shared, "QT_SHARED"_d;
            core.Public += sw::Static, "QT_STATIC"_d;

            core.Public += "org.sw.demo.google.double_conversion"_dep;
            core.Public += "org.sw.demo.madler.zlib"_dep;
            core.Public += "org.sw.demo.facebook.zstd.zstd"_dep;
            core.Public += "org.sw.demo.pcre.pcre16-10"_dep;
            core += harfbuzz;

            if (core.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                qt_core_desc.config.public_.features.insert({ "alloca", false });
                qt_core_desc.config.public_.features.insert({ "alloca_h", false });
            }
            else
            {
                qt_core_desc.config.public_.features.insert({ "alloca", true });
                qt_core_desc.config.public_.features.insert({ "alloca_h", true });
            }
            qt_core_desc.config.public_.features.insert({"alloca_malloc_h", false});

            qt_desc.print(core);
            qt_core_desc.print(core);

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

            platform_files(core);

            ::moc(moc, core);
            ::rcc(rcc, core, core.SourceDir / "mimetypes/mimetypes.qrc");

            // after moc
            write_tracepoints(core);

            {
                auto c = core.addCommand(SW_VISIBLE_FUNCTION(mime_preprocess_file));
                c << cmd::in("mimetypes/mime/packages/freedesktop.org.xml");
                c << cmd::out("qmimeprovider_database.cpp", cmd::Skip);
            }
        }

        // gui
        {
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, gui, { "QtGui" });
            gui += "src/3rdparty/icc/sRGB2014.icc";
            SwapAndRestore sr(gui.SourceDir);
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
                "rhi/.*"_rr,
                "text/.*"_rr,
                "util/.*"_rr;

            gui -= "text/qcssscanner.cpp";
            gui -= "painting/webgradients.cpp";

            gui -= "rhi/qrhivulkan.*"_rr;
            gui -= "rhi/qrhimetal.*"_rr;
            gui -= "rhi/qrhid3d11.*"_rr;
            if (gui.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                gui += "rhi/qrhid3d11.*"_rr;
                gui += "d3d11.lib"_slib;
                gui += "dxgi.lib"_slib;
                gui += "dxguid.lib"_slib;
            }
            if (gui.getBuildSettings().TargetOS.Type == OSType::Macos)
                gui += "rhi/qrhimetal.*"_rr;

            //gui.Public += "src"_id;
            gui.Private += "QT_BUILD_GUI_LIB"_d;

            gui.Public += "org.sw.demo.glennrp.png"_dep;
            gui.Public += "org.sw.demo.behdad.harfbuzz"_dep;
            gui += "org.sw.demo.mity.md4c"_dep;
            gui.Public += core;

            qt_gui_desc.print(gui);

            platform_files(gui);

            gui.replaceInFileOnce("text/qharfbuzzng_p.h", "#include <harfbuzz/hb.h>", "#include <hb.h>");
            gui.replaceInFileOnce("text/qfontengine.cpp", "#  include <harfbuzz/hb-ot.h>", "#include <hb-ot.h>");
            gui.replaceInFileOnce("text/qtextmarkdownimporter.cpp", "#include \"../../3rdparty/md4c/md4c.h\"", "#include <md4c/md4c.h>");

            ::moc(moc, gui);
            ::rcc(rcc, gui, Files{ gui.SourceDir / "painting/qpdf.qrc", });

            // after moc
            write_tracepoints(gui);
        }

        // widgets
        {
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, widgets, { "QtWidgets" });

            SwapAndRestore sr(widgets.SourceDir);
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
            if (widgets.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                widgets.Public += "dwmapi.lib"_slib;
                widgets.Public += "uxtheme.lib"_slib;
            }

            widgets.Public += gui;
            widgets += "dialogs"_id;

            qt_widgets_desc.print(widgets);

            platform_files(widgets);

            ::moc(moc, widgets);
            ::rcc(rcc, widgets, Files{ widgets.SourceDir / "styles/qstyle.qrc", widgets.SourceDir / "dialogs/qmessagebox.qrc" });
            ::uic(uic, widgets, widgets.SourceDir / "dialogs/qfiledialog.ui");

            // after moc
            write_tracepoints(widgets);

            widgets.patch("kernel/qwidgetsvariant.cpp", "qRegisterMetaType<QWidget*>();", "//qRegisterMetaType<QWidget*>();");
        }


        // network
        {
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, network, { "QtNetwork" });

            SwapAndRestore sr(network.SourceDir);
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
            network -= "access/qnetworkreplywasmimpl.*"_rr;

            network -= "ssl/qsslcertificate_qt.cpp"_rr;
            network -= "ssl/qsslkey_qt.cpp"_rr;
            network -= ".*_libproxy.*"_rr;
            network -= ".*_dummy.*"_rr;
            network -= ".*_schannel.*"_rr;

            network.Private +=
                "."_id,
                "kernel"_id;

            network.Private += "QT_BUILD_NETWORK_LIB"_d;
            network += "QT_USE_QSTRINGBUILDER"_d;
            if (network.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                network += "com.Microsoft.Windows.SDK.winrt"_dep;

                network.Public += "Crypt32.lib"_slib;
                network.Public += "Dnsapi.lib"_slib;
                network.Public += "Iphlpapi.lib"_slib;
            }

            network.Public += core;
            network.Public += "org.sw.demo.openssl.ssl"_dep;

            qt_network_desc.print(network);

            if (!network.DryRun)
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
            ::moc(moc, network);
        }

        // xml
        {
            syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, xml, { "QtXml" });

            SwapAndRestore sr(xml.SourceDir);
            xml.SourceDir /= "src/xml";
            xml +=
                "dom/qdom.*"_rr,
                "qtxmlglobal.h",
                "sax/qxml.*"_rr;

            xml.Private += "QT_BUILD_XML_LIB"_d;
            xml.Public += core;

            qt_xml_desc.print(xml);
        }

        // sql
        auto &sql = base.addTarget<LibraryTarget>("sql");
        {
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, sql, { "QtSql" });
            SwapAndRestore sr(sql.SourceDir);
            sql.SourceDir /= "src/sql";
            sql +=
                "kernel/.*"_rr,
                "models/.*"_rr
                ;

            sql.Private += "QT_BUILD_SQL_LIB"_d;
            sql.Public += core;

            qt_sql_desc.print(sql);

            ::moc(moc, sql);
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
                accessibility.Public += gui;

                String module = "QtAccessibilitySupport";
                auto &t = accessibility;

                syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, accessibility, { module });

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
                auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, eventdispatchers, { module });

                SwapAndRestore sr(eventdispatchers.SourceDir);
                eventdispatchers.SourceDir /= "src/platformsupport/eventdispatchers";
                eventdispatchers += ".*"_rr;
                eventdispatchers -= ".*_glib.*"_rr;
                eventdispatchers -= ".*_qpa.*"_rr;
                if (eventdispatchers.getBuildSettings().TargetOS.Type == OSType::Windows)
                    eventdispatchers -= ".*unix.*"_rr;
                else
                    eventdispatchers -= ".*windows.*"_rr;

                eventdispatchers.Public += gui;
                auto &t = eventdispatchers;

                ::moc(moc, eventdispatchers);

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

                themes.Public += gui;

                String module = "QtThemeSupport";
                syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, themes, { module });
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
                    "src/platformsupport/windowsuiautomation/.*"_rr
                    ;

                windowsuiautomation.Public += gui;

                String module = "QtWindowsUIAutomationSupport";
                syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, windowsuiautomation, { module });
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
                    syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, freetype, { module });

                    freetype +=
                        "src/platformsupport/fontdatabases/freetype/.*"_rr;
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
                    syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, windows, { module });

                    windows +=
                        "src/platformsupport/fontdatabases/.*\\.h"_rr,
                        "src/platformsupport/fontdatabases/windows/.*"_rr;

                    windows.Public +=
                        "src"_id;

                    if (windows.getBuildSettings().TargetOS.Type == OSType::Windows)
                        windows.Private += "QT_USE_DIRECTWRITE2"_d;

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

        auto make_qt_plugin = [](auto &t, const String &name)
        {
            path p = name + ".cpp";
            t.writeFileOnce(p, R"(#ifndef SW_QT_PLUGIN_)" + name + R"(
#define SW_QT_PLUGIN_)" + name + R"(

#if defined(QT_STATIC)
#include <QtPlugin>
Q_IMPORT_PLUGIN()" + name + R"();
#endif

#endif
)");
            t.Interface += p;
        };

        auto &windows = platforms.addTarget<LibraryTarget>("windows");
        {
            windows.setOutputDir("plugins/platforms");
            windows +=
                "src/plugins/platforms/windows/.*"_rr;

            windows -=
                "src/plugins/platforms/windows/qwindowsvulkaninstance.cpp";

            windows.Public +=
                "src/plugins/platforms/windows"_id;

            if (windows.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                windows.Public += "Dwmapi.lib"_slib;
                windows.Public += "Imm32.lib"_slib;
                windows.Public += "UxTheme.lib"_slib;
                windows.Public += "Winmm.lib"_slib;
                windows.Public += "Wtsapi32.lib"_slib;
                windows += "EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE=EGL_PLATFORM_ANGLE_DEVICE_TYPE_D3D_WARP_ANGLE"_d;
            }
            windows.Private += sw::Static, "QT_STATICPLUGIN"_d;

            windows.Public += accessibility;
            windows.Public += eventdispatchers;
            windows.Public += themes;
            windows.Public += windowsuiautomation;
            windows.Public += fontdatabases_windows;

            ::moc(moc, windows);
            ::rcc(rcc, windows, Files{
                windows.SourceDir / "src/plugins/platforms/windows/cursors.qrc",
                windows.SourceDir / "src/plugins/platforms/windows/openglblacklists.qrc" });

            windows.Public += "org.sw.demo.google.angle.egl"_dep;
            windows.Public += wintab;
            windows.Public += iaccessible2;

            make_qt_plugin(windows, "QWindowsIntegrationPlugin");
        }

        auto &windowsvista = styles.addTarget<LibraryTarget>("windowsvista");
        {
            windowsvista.setOutputDir("plugins/styles");
            windowsvista +=
                "src/plugins/styles/windowsvista/.*"_rr;
            windowsvista.Public +=
                "src/plugins/styles/windowsvista"_id;

            if (windowsvista.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                windowsvista.Public += "gdi32.lib"_slib;
                windowsvista.Public += "user32.lib"_slib;
            }
            windowsvista.Private += sw::Static, "QT_STATICPLUGIN"_d;

            windowsvista.Public += widgets;

            ::moc(moc, windowsvista);
            make_qt_plugin(windowsvista, "QWindowsVistaStylePlugin");
        }

        auto &winmain = base.addTarget<StaticLibraryTarget>("winmain");
        {
            winmain += "src/winmain/qtmain_win.cpp";
            winmain.Public += core;
        }

        auto &printsupport = base.addTarget<LibraryTarget>("printsupport");
        {
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, printsupport, { "QtPrintSupport" });

            SwapAndRestore sr(printsupport.SourceDir);
            printsupport.SourceDir /= "src/printsupport";

            printsupport +=
                "dialogs/.*"_rr,
                "kernel/.*"_rr,
                "widgets/.*"_rr;

            printsupport.Protected += "dialogs"_idir, "kernel"_idir, "widgets"_idir;

            printsupport.Private += "QT_BUILD_PRINTSUPPORT_LIB"_d;
            if (printsupport.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                printsupport += "Winspool.lib"_slib;
                printsupport += "Comdlg32.lib"_slib;
                printsupport -= "kernel/qcups.cpp";
                printsupport -= "widgets/qcups.*"_rr;
            }

            printsupport.Public += widgets;

            qt_printsupport_desc.print(printsupport);

            platform_files(printsupport);
            ::moc(moc, printsupport);

            ::rcc(rcc, printsupport, printsupport.SourceDir / "dialogs/qprintdialog.qrc");

            for (auto &f : enumerate_files_like(printsupport.SourceDir / "dialogs", ".*\\.ui"))
                ::uic(uic, printsupport, f);
        }

        auto &plugins_printsupport = plugins.addDirectory("printsupport");

        auto &plugins_printsupport_windows = plugins_printsupport.addTarget<LibraryTarget>("windows");
        {
            plugins_printsupport_windows.setOutputDir("plugins/printsupport");
            plugins_printsupport_windows +=
                "src/plugins/printsupport/windows/.*"_rr;
            plugins_printsupport_windows.Public +=
                "src/plugins/printsupport/windows"_id;
            plugins_printsupport_windows += "Winspool.lib"_slib;

            plugins_printsupport_windows.Private += sw::Static, "QT_STATICPLUGIN"_d;
            plugins_printsupport_windows.Public += printsupport;
            ::moc(moc, plugins_printsupport_windows);
            make_qt_plugin(plugins_printsupport_windows, "QWindowsPrinterSupportPlugin");
        }

        //
        auto &sqldrivers = plugins.addDirectory("sqldrivers");

        auto &sqlite = sqldrivers.addTarget<LibraryTarget>("sqlite");
        {
            sqlite.setOutputDir("plugins/sqldrivers");
            sqlite += "src/plugins/sqldrivers/sqlite/.*"_rr;
            sqlite.Public += sql;
            sqlite.Private += sw::Shared, "QT_PLUGIN"_d;
            sqlite += "org.sw.demo.sqlite3"_dep;

            ::moc(moc, sqlite);
        }

        auto &psql = sqldrivers.addTarget<LibraryTarget>("psql");
        {
            psql.setOutputDir("plugins/sqldrivers");
            psql += "src/plugins/sqldrivers/psql/.*"_rr;
            psql.Public += sql;
            psql.Private += sw::Shared, "QT_PLUGIN"_d;
            psql += "org.sw.demo.postgres.pq"_dep;

            ::moc(moc, psql);
        }

        auto &odbc = sqldrivers.addTarget<LibraryTarget>("odbc");
        {
            odbc.setOutputDir("plugins/sqldrivers");
            odbc += "src/plugins/sqldrivers/odbc/.*"_rr;
            odbc.Public += sql;
            odbc.Private += sw::Shared, "QT_PLUGIN"_d;
            if (odbc.getBuildSettings().TargetOS.Type == OSType::Windows)
                odbc += "odbc32.lib"_slib;

            ::moc(moc, odbc);
        }
    }

    // declarative
    auto &declarative = add_subproject<Project>(qt, "declarative");
    NativeExecutedTarget *qml_imports_layouts, *qml_imports_window, *pquick;
    {
        auto &third_party = declarative.addDirectory("third_party");

        auto &qml = declarative.addLibrary("qml");

        auto &masm = third_party.addStaticLibrary("masm");
        {
            masm.ApiNames.insert("WTF_EXPORT_PRIVATE");
            masm.ApiNames.insert("JS_EXPORT_PRIVATE");
            masm += "src/3rdparty/masm/.*"_rr;
            masm -= "src/3rdparty/masm/wtf/OSAllocator.*.cpp"_rr;
            masm -= "src/3rdparty/masm/yarr/YarrErrorCode.cpp";

            masm.Protected += "src/3rdparty/masm"_idir;
            masm.Protected += "src/3rdparty/masm/wtf"_idir;
            masm.Protected += "src/3rdparty/masm/stubs"_idir;
            masm.Protected += "src/3rdparty/masm/stubs/wtf"_idir;
            masm.Protected += "src/3rdparty/masm/assembler"_idir;
            masm.Protected += "src/3rdparty/masm/disassembler"_idir;
            masm.Protected += "src/3rdparty/masm/disassembler/udis86"_idir;
            masm.Protected += "src/3rdparty/masm/jit"_idir;
            masm.Protected += "src/3rdparty/masm/runtime"_idir;
            masm.Protected += "src/3rdparty/masm/stubs/runtime"_idir;

            masm.Protected += "ENABLE_ASSEMBLER_WX_EXCLUSIVE"_d;
            masm.Protected += "WTF_USE_UDIS86"_d;
            masm.Protected += "ENABLE_LLINT=0"_d;
            masm.Protected += "ENABLE_DFG_JIT=0"_d;
            masm.Protected += "ENABLE_DFG_JIT_UTILITY_METHODS"_d;
            masm.Protected += "ENABLE_JIT_CONSTANT_BLINDING=0"_d;
            masm.Protected += "BUILDING_QT__"_d;
            masm.Protected += "NOMINMAX"_d;

            masm.Public += core;
            (masm + qml)->IncludeDirectoriesOnly = true;

            if (masm.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                masm += "src/3rdparty/masm/wtf/OSAllocatorWin.cpp"_rr;
            }
            else
            {
                masm += "src/3rdparty/masm/wtf/OSAllocatorPosix.cpp"_rr;
            }

            {
                auto c = masm.addCommand();
                c << cmd::prog("org.sw.demo.python.exe-2"_dep)
                    << cmd::wdir(masm.BinaryDir)
                    << cmd::in("src/3rdparty/masm/disassembler/udis86/itab.py")
                    << cmd::in("src/3rdparty/masm/disassembler/udis86/optable.xml")
                    << cmd::end()
                    << cmd::out("udis86_itab.c")
                    << cmd::out("udis86_itab.h")
                    ;
            }

            {
                auto c = masm.addCommand();
                c << cmd::prog("org.sw.demo.python.exe-2"_dep)
                    << cmd::in("src/3rdparty/masm/yarr/create_regex_tables")
                    << cmd::std_out("RegExpJitTables.h")
                    ;
            }
        }

        // qml
        {
            String module = "QtQml";
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, qml, { module });
            masm += sqt; // masm needs this sqt dependency, but creates cyclic dep
            //qml ^= sqt; // so we remove sqt from qml

            SwapAndRestore sr(qml.SourceDir, qml.SourceDir / "src/qml");
            qml += "qtqmlglobal.*"_rr;
            qml += "animations/.*"_rr;
            qml -= "compiler/.*"_rr;
            qml += "debugger/.*"_rr;
            qml += "jit/.*"_rr;
            qml += "jsapi/.*"_rr;
            qml += "jsruntime/.*"_rr;
            qml -= "jsruntime/qv4functiontable_noop.cpp";
            qml += "memory/.*"_rr;
            qml += "parser/.*"_rr;
            qml += "qml/.*"_rr;
            qml += "qmldirparser/.*"_rr;
            qml += "types/.*"_rr;
            qml += "util/.*"_rr;

            qml +=
                "compiler/qv4bytecodegenerator.cpp",
                "compiler/qv4bytecodehandler.cpp",
                "jsruntime/qv4compilationunitmapper.cpp",
                //"jsruntime/qv4compileddata.cpp",
                "compiler/qv4compiler.cpp",
                "compiler/qv4compilercontext.cpp",
                "compiler/qv4compilerscanfunctions.cpp",
                "compiler/qv4codegen.cpp",
                "compiler/qqmlirbuilder.cpp",
                "compiler/qv4instr_moth.cpp",
                "qml/qqmltypecompiler.cpp",
                "qml/qqmlpropertycachecreator.cpp",
                "qml/qqmlpropertyvalidator.cpp"
                ;
            qml -=
                "jsruntime/qv4compilationunitmapper_.*.cpp"_rr;

            qml += "."_idir;
            qml += "qml/v8"_idir;
            qml += "compiler"_idir;
            qml += "debugger"_idir;
            qml += "memory"_idir;
            qml += "qmldirparser"_idir;
            //qml += "qml/jsapi"_idir;
            qml.Protected += "jsruntime"_idir;

            qml += "QT_BUILD_QML_LIB"_d;
            qml.Protected += "QT_USE_QSTRINGBUILDER"_d;

            if (qml.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                qml += "jsruntime/qv4compilationunitmapper_win.cpp";
                qml.Private += "UNICODE"_d;
            }
            else
            {
                qml += "jsruntime/qv4compilationunitmapper_unix.cpp";
            }

            qml += masm;
            qml.Public += network;

            qml.writeFileOnce("private/qml_compile_hash_p.h", R"xxx(
            #define QML_COMPILE_HASH ""
            #define QML_COMPILE_HASH_LENGTH 0
)xxx");

            platform_files(qml);
            ::moc(moc, qml);

            // after moc
            write_tracepoints(qml);

            String module_lower = module;
            std::transform(module_lower.begin(), module_lower.end(), module_lower.begin(), tolower);
            auto &t = qml;

            t.writeFileOnce(module + "/" + module_lower + "-config.h", R"xxx(
            #define QT_FEATURE_accessibility 1
            #define QT_FEATURE_qml_debug 1
            #define QT_FEATURE_qml_network 1
            #define QT_FEATURE_cxx14_make_unique 1
            #define QT_FEATURE_qml_xml_http_request 1
            #define QT_FEATURE_qml_sequence_object 1
            #define QT_FEATURE_qml_animation 1
            #define QT_FEATURE_qml_worker_script 1
            #define QT_FEATURE_qml_list_model 1
            #define QT_FEATURE_qml_locale 1
            #define QT_FEATURE_qml_tracing 1

            #define QT_FEATURE_qml_jit 1
)xxx");

            t.writeFileOnce(module + "/private/" + module_lower + "-config_p.h", R"xxx(
            #define QT_FEATURE_qml_delegate_model 1
            #define QT_FEATURE_qml_profiler 1

            #define QT_FEATURE_qml_itemmodel 1
)xxx");

            t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
            #ifdef __cplusplus
            #include <QtCore/QtCore>
            #include <QtNetwork/QtNetwork>
            #endif
)xxx");

            {
                auto c = qml.addCommand();
                c << cmd::prog(qlalr)
                    << cmd::wdir(qml.BinaryDir / "private")
                    << "--no-debug" << "--qt"
                    << cmd::in("parser/qqmljs.g")
                    << cmd::end()
                    << cmd::out(qml.BinaryDir / "private" / "qqmljsparser.cpp")
                    << cmd::out(qml.BinaryDir / "private" / "qqmljsparser_p.h")
                    << cmd::out(qml.BinaryDir / "private" / "qqmljsgrammar.cpp")
                    << cmd::out(qml.BinaryDir / "private" / "qqmljsgrammar_p.h")
                    ;
            }
        }

        auto &qml_models = qml.addLibrary("models");
        {
            String module = "QtQmlModels";
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, qml_models, { module });
            qml_models.SourceDir /= "src/qmlmodels";

            qml_models += ".*"_r;
            qml_models += "QT_BUILD_QMLMODELS_LIB"_def;

            qml_models.Public += qml;

            ::moc(moc, qml_models);

            String module_lower = module;
            std::transform(module_lower.begin(), module_lower.end(), module_lower.begin(), tolower);
            auto &t = qml_models;

            t.writeFileOnce(module + "/" + module_lower + "-config.h", R"xxx(
            #define QT_FEATURE_qml_object_model 1
            #define QT_FEATURE_qml_table_model 1
)xxx");

            t.writeFileOnce(module + "/private/" + module_lower + "-config_p.h", R"xxx(
)xxx");

            t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
            #ifdef __cplusplus
            #include <QtCore/QtCore>
            #include <QtQml/QtQml>
            #endif
)xxx");
        }

        auto &qml_workerscript = qml.addLibrary("workerscript");
        {
            auto &t = qml_workerscript;

            String module = "QtQmlWorkerScript";
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, t, { module });
            t.SourceDir /= "src/qmlworkerscript";

            t += ".*"_r;
            t += "QT_BUILD_WORKERSCRIPT_LIB"_def;

            t.Public += qml, masm;

            ::moc(moc, t);

            String module_lower = module;
            std::transform(module_lower.begin(), module_lower.end(), module_lower.begin(), tolower);

            t.writeFileOnce(module + "/" + module_lower + "-config.h", R"xxx(
)xxx");

            t.writeFileOnce(module + "/private/" + module_lower + "-config_p.h", R"xxx(
)xxx");

            t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
            #ifdef __cplusplus
            #include <QtCore/QtCore>
            #include <QtQml/QtQml>
            #endif
)xxx");
        }

        auto &shared = declarative.addStaticLibrary("tools.shared");
        {
            shared.SourceDir /= "tools/shared";
            shared += ".*"_rr;
            shared.Public += core;
        }

        auto &qmltyperegistrar = qml.addExecutable("typeregistrar");
        {
            qmltyperegistrar.SourceDir /= "src/qmltyperegistrar";
            qmltyperegistrar += ".*"_rr;
            qmltyperegistrar += shared;
        }

        auto &quick = declarative.addLibrary("quick");
        pquick = &quick;
        {
            String module = "QtQuick";
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, quick, { module });
            quick.SourceDir /= "src/quick";

            quick += "[^/]*"_rr;
            quick += "util/.*"_rr;
            quick += "scenegraph/.*"_rr;
            quick += "items/.*"_rr;
            quick += "handlers/.*"_rr;
            quick += "accessible/.*"_rr;

            quick += "."_idir;

            quick += "QT_BUILD_QUICK_LIB"_d;
            quick += "M_PI=3.14159265358979323846"_d;

            if (quick.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                quick.Private += "UNICODE"_d;
            }

            quick.Public += qml, gui, qml_models;

            ::moc(moc, quick);
            ::rcc(rcc, quick, quick.SourceDir / "items/items.qrc");
            ::rcc(rcc, quick, quick.SourceDir / "scenegraph/scenegraph.qrc");

            // after moc
            write_tracepoints(quick);

            String module_lower = module;
            std::transform(module_lower.begin(), module_lower.end(), module_lower.begin(), tolower);
            auto &t = quick;

            t.writeFileOnce(module + "/" + module_lower + "-config.h", R"xxx(
            #define QT_FEATURE_d3d12 1
            #define QT_FEATURE_opengl 1

            #define QT_FEATURE_quick_draganddrop 1
)xxx");

            t.writeFileOnce(module + "/private/" + module_lower + "-config_p.h", R"xxx(
            #define QT_FEATURE_quick_animatedimage 1
            #define QT_FEATURE_quick_path 1
            #define QT_FEATURE_quick_canvas 1
            #define QT_FEATURE_quick_designer 1
            #define QT_FEATURE_quick_flipable 1
            #define QT_FEATURE_quick_gridview 1
            #define QT_FEATURE_quick_itemview 1
            #define QT_FEATURE_quick_listview 1
            #define QT_FEATURE_quick_shadereffect 1
            #define QT_FEATURE_quick_sprite 1
            #define QT_FEATURE_quick_particles 1
            #define QT_FEATURE_quick_pathview 1
            #define QT_FEATURE_quick_positioners 1
            #define QT_FEATURE_quick_repeater 1
            #define QT_FEATURE_quick_viewtransitions 1
            #define QT_FEATURE_quick_tableview 1
)xxx");

            t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
            #ifdef __cplusplus
            #include <QtCore/QtCore>
            #include <QtGui/QtGui>
            #include <QtNetwork/QtNetwork>
            #include <QtQml/QtQml>
            #endif
)xxx");

            /*{
                //https://www.qt.io/blog/qml-type-registration-in-qt-5.15
                //#include "qtquickglobal_p.h"

                auto c = quick.addCommand();
                c << cmd::prog(qmltyperegistrar)
                    << "--import-name=" + module
                    << "--private-includes"
                    << "--major-version=2"
                    << "--minor-version=" + std::to_string(quick.getPackage().getVersion().getMinor())
                    << "--generate-qmltypes" << cmd::out("plugins.qmltypes")
                    << "-o" << cmd::out("qmltypes.cpp")
                    ;
            }*/
        }

        auto &imports = qml.addDirectory("imports");

        auto add_import = [&imports, &quick, &moc](const String &name) -> decltype(auto)
        {
            auto &t = imports.addLibrary(name);
            t.setRootDirectory("src/imports");
            t.Public += quick;
            t += FileRegex(name + "/plugin.*", true);
            t += FileRegex(name + "/.*\\.[ch]p?p?", true);
            t += IncludeDirectory(name);

            auto qmldir = path(name) / "qmldir";
            t += qmldir;
            t.patch(qmldir, "plugin ", "# plugi n");

            auto settings = path(name) / (name + ".pro");
            t += settings;

            if (t.DryRun)
                return t;

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

            ::moc(moc, t);

            return t;
        };

        add_import("qtquick2") += qml_workerscript;
        qml_imports_layouts = &add_import("layouts");
        qml_imports_window = &add_import("window");
        add_import("settings");
        add_import("folderlistmodel");
    }

    // svg
    {
        auto &svg = add_subproject<Library>(qt, "svg");

        auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, svg, { "QtSvg" });
        auto &t = svg;

        svg += "src/svg/[^/]*"_rr;
        svg.Private += "QT_BUILD_SVG_LIB"_d;
        svg.Public += widgets;
        if (svg.getBuildSettings().TargetOS.Type == OSType::Windows)
            svg.Private += "UNICODE"_d;
        ::moc(moc, svg);
        qt_svg_desc.print(svg);
    }

    // winextras
    {
        auto &winextras = add_subproject<Library>(qt, "winextras");

        syncqt("pub.egorpugin.primitives.tools.syncqt-master"_dep, winextras, { "QtWinExtras" });
        auto &t = winextras;

        winextras.SourceDir /= "src/winextras";
        winextras += "[^/]*"_rr;
        winextras.Private += "QT_BUILD_WINEXTRAS_LIB"_d;
        winextras.Public += gui;
        if (winextras.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            winextras.Private += "UNICODE"_d;
            winextras += "dwmapi.lib"_slib;
        }
        ::moc(moc, winextras);
        qt_winextras_desc.print(winextras);
    }

    //auto &plugins = qt.addDirectory("plugins");

    //
    {
        auto &imageformats = add_subproject<Project>(qt, "imageformats");

        auto add_imageformat1 = [&s, &moc, &gui](auto &t, const String &name)
        {
            t.setOutputDir("plugins/imageformats");
            t.SourceDir /= "src/plugins/imageformats";
            t.SourceDir /= name;
            t += ".*\\.cpp"_rr, ".*\\.h"_rr, ".*\\.json"_rr;
            if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
                t.Private += "UNICODE"_d;
            t.Private += sw::Static, "QT_STATICPLUGIN"_d;
            ::moc(moc, t);
            // pass plugin class name here
            //make_qt_plugin(t, "Q" + plugin class name + "Plugin");
            t.Public += gui;
        };

        auto add_imageformat = [&s, &imageformats, &add_imageformat1](const String &name) -> decltype(auto)
        {
            auto &t = imageformats.addLibrary("q" + name);
            add_imageformat1(t, name);
            return t;
        };

        auto add_imageformat_base = [&base, &imageformats, &add_imageformat1](const String &name) -> decltype(auto)
        {
            auto &t = base.addLibrary("q" + name);
            add_imageformat1(t, name);
            return t;
        };

        add_imageformat_base("gif");
        add_imageformat_base("ico");
        add_imageformat_base("jpeg") += "org.sw.demo.jpeg"_dep;

        add_imageformat("dds");
        add_imageformat("icns");
        add_imageformat("tga");
        add_imageformat("tiff") += "org.sw.demo.tiff"_dep;
        add_imageformat("wbmp");
        add_imageformat("webp") += "org.sw.demo.webmproject.webp"_dep;
        add_imageformat("jp2") += "org.sw.demo.mdadams.jasper"_dep;
        add_imageformat("mng") += "org.sw.demo.mng"_dep;
    }

    auto &tools = add_subproject<Project>(qt, "tools");
    {
        auto &linguist = tools.addDirectory("linguist");

        auto &formats = linguist.addTarget<StaticLibraryTarget>("formats");
        {
            formats.SourceDir /= "src/linguist/shared";
            formats +=
                "numerus.cpp",
                "po.cpp",
                "qm.cpp",
                "qph.cpp",
                "simtexth.*"_rr,
                "translator.cpp",
                "translator.h",
                "translatormessage.cpp",
                "translatormessage.h",
                "ts.cpp",
                "xliff.cpp",
                "xmlparser.cpp",
                "xmlparser.h"
                ;

            if (formats.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                formats.Private += "UNICODE"_d;
            }

            formats.Public += xml;

            formats.replaceInFileOnce("qm.cpp", "Q_CONSTRUCTOR_FUNCTION(initQM)",
                "int initTS();Q_CONSTRUCTOR_FUNCTION(initTS)Q_CONSTRUCTOR_FUNCTION(initQM )");
        }

        auto &convert = linguist.addTarget<ExecutableTarget>("convert");
        {
            convert += "src/linguist/lconvert/main.cpp";
            if (convert.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                convert.Private += "UNICODE"_d;
            }
            convert.Public += formats;
        }

        auto &proparser = linguist.addTarget<StaticLibraryTarget>("proparser");
        {
            proparser.SourceDir /= "src/linguist/shared";
            proparser += ".*\\.h"_rr;
            // don't use .cpp regex, there will be ling.tools run failures
            proparser +=
                "ioutils.cpp",
                "qrcreader.cpp",
                "runqttool.cpp",
                "projectdescriptionreader.cpp",
                "profileevaluator.cpp",
                "proitems.cpp",
                "qmakebuiltins.cpp",
                "qmakeevaluator.cpp",
                "qmakeglobals.cpp",
                "qmakeparser.cpp",
                "qmakevfs.cpp"
                ;

            proparser.Public += "PROEVALUATOR_CUMULATIVE"_d;
            proparser.Public += "PROEVALUATOR_INIT_PROPS"_d;
            proparser.Public += "QMAKE_BUILTIN_PRFS"_d;
            proparser.Public += "QMAKE_OVERRIDE_PRFS"_d;
            if (proparser.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                proparser.Private += "UNICODE"_d;
            }

            proparser.Public += core;
            proparser.Public += xml;
        }

        auto &release = linguist.addTarget<ExecutableTarget>("release");
        {
            release += "src/linguist/lrelease/main.cpp";
            if (release.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                release.Private += "UNICODE"_d;
            }

            release.Public += proparser;
            release.Public += formats;
        }

        auto &update = linguist.addTarget<ExecutableTarget>("update");
        {
            update +=
                "src/linguist/lupdate/.*\\.cpp"_rr,
                "src/linguist/lupdate/.*\\.h"_rr;

            update -=
                "src/linguist/lupdate/qdeclarative.cpp";

            update.Public +=
                "src"_id;

            update.Public += "PROEVALUATOR_DEBUG"_d;
            update.Public += "QT_NO_QML"_d;
            if (update.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                update.Private += "UNICODE"_d;
            }

            update.Public += proparser;
            update.Public += formats;

            update.replaceInFileOnce("src/linguist/lupdate/main.cpp",
                "int main(int argc, char **argv)",
                "int initTS(); Q_CONSTRUCTOR_FUNCTION(initTS) int main(int argc, char **argv )");
        }
    }

    auto &translations = add_subproject<LibraryTarget>(qt, "translations");
    //translations.Empty = true;
    translations += "translations/.*\\.ts"_rr;

    auto &quickcontrols = add_subproject<Project>(qt, "quickcontrols");
    {
        auto setup_output = [](auto &t, const String &out_dir, const String &proj_dir)
        {
            t.setOutputDir("qml/QtQuick/" + out_dir);

            auto qmldir = path(proj_dir) / "qmldir";
            t += qmldir;
            t.patch(qmldir, "plugin ", "# plugi n");

            auto settings = path(proj_dir) / (t.getPackage().getPath().back() + ".pro");
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
            controls += "src/controls/.*\\.cpp"_r;
            controls += "src/controls/.*\\.h"_r;
            controls += "src/controls/.*\\.qml"_r;
            controls += "src/controls/Private/.*"_rr;
            controls += "src/controls/Shaders/.*"_rr;
            controls += "src/controls/Styles/.*"_rr;
            controls -= "src/controls/Styles/Android/.*"_rr;
            controls -= "src/controls/Styles/WinRT/.*"_rr;
            controls.Public += widgets;
            controls.Public += *pquick;
            controls.Public += *qml_imports_layouts;
            controls.Public += *qml_imports_window;
            ::moc(moc, controls);

            setup_output(controls, "Controls", "src/controls");
        }

        auto &dialogs = quickcontrols.addLibrary("dialogs");
        auto &dialogs_pvt = dialogs.addLibrary("Private");
        {
            dialogs_pvt += "src/dialogs/Private/.*"_rr;
            dialogs_pvt.Public += controls;
            ::moc(moc, dialogs_pvt);

            setup_output(dialogs_pvt, "Dialogs/Private", "src/dialogs/Private");
        }

        {
            dialogs += "src/dialogs/[^/]*\\.cpp"_rr;
            dialogs += "src/dialogs/[^/]*\\.h"_rr;
            dialogs += "src/dialogs/[^/]*\\.qml"_rr;
            dialogs += "src/dialogs/images/.*"_rr;
            dialogs += "src/dialogs/qml/.*"_rr;
            dialogs.Public += dialogs_pvt;
            ::moc(moc, dialogs);

            setup_output(dialogs, "Dialogs", "src/dialogs");
        }

        auto &widgets = quickcontrols.addLibrary("widgets");
        {
            widgets += "src/widgets/[^/]*\\.cpp"_rr;
            widgets += "src/widgets/[^/]*\\.h"_rr;
            //widgets += "src/widgets/[^/]*\\.qml"_rr;
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
            ::moc(moc, widgets);

            setup_output(widgets, "PrivateWidgets", "src/widgets");
        }
    }
}
