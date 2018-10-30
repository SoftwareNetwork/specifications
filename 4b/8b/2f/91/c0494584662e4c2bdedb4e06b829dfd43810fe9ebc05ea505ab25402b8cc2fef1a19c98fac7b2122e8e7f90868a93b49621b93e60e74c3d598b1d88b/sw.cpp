void build(Solution &s)
{
    auto &qt = s.addProject("qtproject.qt", "5.11.2");

    auto &tools = qt.addProject("tools");
    tools += Git("git://code.qt.io/qt/qttools.git", "v{v}");

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
                "xliff.cpp";

            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                formats.Private += "UNICODE"_d;
            }

            formats.Public += "org.sw.demo.qtproject.qt.base.xml-5"_dep;

            /*replace_in_file_once(${SDIR}/qm.cpp
                "Q_CONSTRUCTOR_FUNCTION(initQM)"
                "int initTS();Q_CONSTRUCTOR_FUNCTION(initTS)Q_CONSTRUCTOR_FUNCTION(initQM )")*/
        }

        auto &convert = linguist.addTarget<ExecutableTarget>("convert");
        {
            convert += "src/linguist/lconvert/main.cpp";
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                convert.Private += "UNICODE"_d;
            }
            convert.Public += formats;
        }

        auto &proparser = linguist.addTarget<StaticLibraryTarget>("proparser");
        {
            proparser.SourceDir /= "src/linguist/shared";
            proparser +=
                "ioutils.cpp",
                "ioutils.h",
                "profileevaluator.cpp",
                "profileevaluator.h",
                "proitems.cpp",
                "proitems.h",
                "qmake_global.h",
                "qmakebuiltins.cpp",
                "qmakeevaluator.cpp",
                "qmakeevaluator.h",
                "qmakeevaluator_p.h",
                "qmakeglobals.cpp",
                "qmakeglobals.h",
                "qmakeparser.cpp",
                "qmakeparser.h",
                "qmakevfs.cpp",
                "qmakevfs.h";

            proparser.Public += "PROEVALUATOR_CUMULATIVE"_d;
            proparser.Public += "PROEVALUATOR_INIT_PROPS"_d;
            proparser.Public += "QMAKE_BUILTIN_PRFS"_d;
            proparser.Public += "QMAKE_OVERRIDE_PRFS"_d;
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                proparser.Private += "UNICODE"_d;
            }

            proparser.Public += "org.sw.demo.qtproject.qt.base.core-5"_dep;
        }

        auto &release = linguist.addTarget<ExecutableTarget>("release");
        {
            release += "src/linguist/lrelease/main.cpp";
            if (s.Settings.TargetOS.Type == OSType::Windows)
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
            if (s.Settings.TargetOS.Type == OSType::Windows)
            {
                update.Private += "UNICODE"_d;
            }

            update.Public += proparser;
            update.Public += formats;

            /*
    replace_in_file_once(${SDIR}/src/linguist/lupdate/main.cpp "int main(int argc, char **argv)" "int initTS(); Q_CONSTRUCTOR_FUNCTION(initTS) int main(int argc, char **argv )")*/
        }
    }
}
