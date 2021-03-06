#pragma sw require header pub.egorpugin.primitives.tools.syncqt-master
#pragma sw require header org.sw.demo.qtproject.qt.base.widgets-5

void build(Solution &s)
{
    auto &qt = s.addProject("qtproject.qt", "5.11.2");

    auto &declarative = qt.addProject("declarative");
    declarative += Git("git://code.qt.io/qt/qtdeclarative.git", "v{v}");

    {
        auto &third_party = declarative.addDirectory("third_party");

        auto &qml = declarative.addLibrary("qml");

        auto &masm = third_party.addStaticLibrary("masm");
        {
            masm.ApiNames.insert("WTF_EXPORT_PRIVATE");
            masm.ApiNames.insert("JS_EXPORT_PRIVATE");
            masm += "src/3rdparty/masm/.*"_rr;
            masm -= "src/3rdparty/masm/wtf/OSAllocator.*.cpp"_rr;

            masm.Protected += "src/3rdparty/masm"_idir;
            masm.Protected += "src/3rdparty/masm/wtf"_idir;
            masm.Protected += "src/3rdparty/masm/stubs"_idir;
            masm.Protected += "src/3rdparty/masm/stubs/wtf"_idir;
            masm.Protected += "src/3rdparty/masm/assembler"_idir;
            masm.Protected += "src/3rdparty/masm/disassembler"_idir;
            masm.Protected += "src/3rdparty/masm/disassembler/udis86"_idir;
            masm.Protected += "src/3rdparty/masm/jit"_idir;
            masm.Protected += "src/3rdparty/masm/runtime"_idir;

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
                c << cmd::prog("org.sw.demo.python.exe-3"_dep)
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
                c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                    << cmd::in("src/3rdparty/masm/create_regex_tables")
                    << cmd::std_out("RegExpJitTables.h")
                    ;
            }
        }

        // qml
        {
            String module = "QtQml";
            syncqt(qml, { module });
            qml.SourceDir /= "src/qml";
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

            qml += "qml/v8"_idir;
            qml += "compiler"_idir;
            qml += "debugger"_idir;
            qml += "jsruntime"_idir;

            qml += "QT_BUILD_QML_LIB"_d;

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
            automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, qml);

            String module_lower = module;
            std::transform(module_lower.begin(), module_lower.end(), module_lower.begin(), tolower);
            auto &t = qml;

            t.writeFileOnce(module + "/" + module_lower + "-config.h", R"xxx(
            #define QT_FEATURE_accessibility 1
            #define QT_FEATURE_qml_debug 1
            #define QT_FEATURE_qml_network 1
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
        }

        auto &quick = declarative.addLibrary("quick");
        {
            String module = "QtQuick";
            syncqt(quick, { module });
            quick.SourceDir /= "src/quick";

            quick += "[^/]*"_rr;
            quick += "util/.*"_rr;
            quick += "scenegraph/.*"_rr;
            quick += "items/.*"_rr;
            quick += "handlers/.*"_rr;
            quick += "accessible/.*"_rr;

            quick += "QT_BUILD_QUICK_LIB"_d;

            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                quick.Private += "UNICODE"_d;
            }

            quick.Public += qml, "org.sw.demo.qtproject.qt.base.gui-5"_dep;

            automoc("org.sw.demo.qtproject.qt.base.tools.moc-5"_dep, quick);
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
    }
}
