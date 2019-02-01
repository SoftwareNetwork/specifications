#pragma sw require header pub.egorpugin.primitives.tools.syncqt-master
#pragma sw require header org.sw.demo.qtproject.qt.base.widgets-5

void build(Solution &s)
{
    auto &qt = s.addProject("qtproject.qt", "5.12.1");

    auto &declarative = qt.addProject("declarative");
    declarative += Git("https://github.com/qt/qtdeclarative", "v{v}");

    {
        auto &third_party = declarative.addDirectory("third_party");

        auto &qml = declarative.addLibrary("qml");
        auto &quick = declarative.addLibrary("quick");

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

            masm.Public += "org.sw.demo.qtproject.qt.base.core-5"_dep;
            (masm + qml)->IncludeDirectoriesOnly = true;
            (masm + qml)->IncludeDirectoriesOnly = true;

            if (s.Settings.TargetOS.Type == OSType::Windows)
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
            qml ^= sqt; // so we remove sqt from qml

            SwapAndRestore sr(qml.SourceDir, qml.SourceDir / "src/qml");
            qml += "qtqmlglobal.*"_rr;
            qml += "animations/.*"_rr;
            qml -= "compiler/.*"_rr;
            qml += "debugger/.*"_rr;
            qml += "jit/.*"_rr;
            qml += "jsapi/.*"_rr;
            qml += "jsruntime/.*"_rr;
            qml += "memory/.*"_rr;
            qml += "parser/.*"_rr;
            qml += "qml/.*"_rr;
            qml += "types/.*"_rr;
            qml += "util/.*"_rr;

            qml +=
                "compiler/qv4bytecodegenerator.cpp",
                "compiler/qv4bytecodehandler.cpp",
                "compiler/qv4compilationunitmapper.cpp",
                "compiler/qv4compileddata.cpp",
                "compiler/qv4compiler.cpp",
                "compiler/qv4compilercontext.cpp",
                "compiler/qv4compilerscanfunctions.cpp",
                "compiler/qv4codegen.cpp",
                "compiler/qqmlirbuilder.cpp",
                "compiler/qv4instr_moth.cpp",
                "compiler/qqmltypecompiler.cpp",
                "compiler/qqmlpropertycachecreator.cpp",
                "compiler/qqmlpropertyvalidator.cpp"
                ;
            qml -=
                "compiler/qv4compilationunitmapper_.*.cpp"_rr;

            qml += "."_idir;
            qml += "qml/v8"_idir;
            qml += "compiler"_idir;
            qml += "debugger"_idir;
            qml += "qml/jsapi"_idir;
            qml.Protected += "jsruntime"_idir;

            qml += "QT_BUILD_QML_LIB"_d;
            qml.Protected += "QT_USE_QSTRINGBUILDER"_d;

            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                qml += "compiler/qv4compilationunitmapper_win.cpp";
                qml.Private += "UNICODE"_d;
            }
            else
            {
                qml += "compiler/qv4compilationunitmapper_unix.cpp";
            }

            qml += masm;
            qml.Public += "org.sw.demo.qtproject.qt.base.network-5"_dep;

            qml.writeFileOnce("qml_compile_hash_p.h", R"xxx(
            #define QML_COMPILE_HASH ""
            #define QML_COMPILE_HASH_LENGTH 0
)xxx");

            platform_files(qml);
            auto mocs = automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, qml);
            SW_QT_ADD_MOC_DEPS(qml);

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
)xxx");

            t.writeFileOnce(module + "/private/" + module_lower + "-config_p.h", R"xxx(
            #define QT_FEATURE_qml_delegate_model 1
            #define QT_FEATURE_qml_profiler 1

            #define QT_FEATURE_alloca_h -1
            #define QT_FEATURE_alloca_malloc_h 1
            #define QT_FEATURE_alloca 1
)xxx");

            t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
            #ifdef __cplusplus
            #include <QtCore/QtCore>
            #include <QtNetwork/QtNetwork>
            #endif
)xxx");

            {
                auto c = qml.addCommand();
                c << cmd::prog("org.sw.demo.qtproject.qt.base.tools.qlalr-5"_dep)
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

        // quick
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

            quick += "QT_BUILD_QUICK_LIB"_d;
            quick += "M_PI=3.14159265358979323846"_d;

            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                quick.Private += "UNICODE"_d;
            }

            quick.Public += qml, "org.sw.demo.qtproject.qt.base.gui-5"_dep;

            auto mocs = automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, quick);
            SW_QT_ADD_MOC_DEPS(quick);
            ::rcc("org.sw.demo.qtproject.qt.base.tools.rcc-5"_dep, quick, quick.SourceDir / "items/items.qrc");
            ::rcc("org.sw.demo.qtproject.qt.base.tools.rcc-5"_dep, quick, quick.SourceDir / "scenegraph/scenegraph.qrc");

            String module_lower = module;
            std::transform(module_lower.begin(), module_lower.end(), module_lower.begin(), tolower);
            auto &t = quick;

            t.writeFileOnce(module + "/" + module_lower + "-config.h", R"xxx(
            #define QT_FEATURE_d3d12 1
            #define QT_FEATURE_opengl 1
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
        }

        auto &imports = qml.addDirectory("imports");

        auto add_import = [&imports, &quick](const String &name) -> decltype(auto)
        {
            auto &t = imports.addLibrary(name);
            t.setRootDirectory("src/imports");
            t.Public += quick;
            t += FileRegex(name + "/plugin.*", true);
            t += FileRegex(name + "/.*\\.cpp", true);
            t += FileRegex(name + "/.*\\.h", true);
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

            automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, t);

            return t;
        };

        add_import("qtquick2");
        add_import("layouts");
        add_import("window");
        add_import("settings");
        add_import("folderlistmodel");
    }
}
