#pragma sw header on

void qt_add_translation(const DependencyPtr &lrelease, NativeExecutedTarget &t, const Files &ts_files)
{
    // before dry run
    (t + lrelease)->Dummy = true;

    if (t.PostponeFileResolving || t.DryRun)
        return;

    for (auto &ts : ts_files)
    {
        auto c = t.addCommand();
        c << cmd::prog(lrelease)
            << cmd::in(ts)
            << "-qm"
            << cmd::out(ts.filename().stem().u8string() + ".qm");
    }
}

void qt_create_translation(const DependencyPtr &lupdate, const DependencyPtr &lrelease, NativeExecutedTarget &t)
{
    // before dry run
    (t + lupdate)->Dummy = true;
    (t + lrelease)->Dummy = true;

    if (t.PostponeFileResolving || t.DryRun)
        return;

    auto ts_lst_fn = t.BinaryDir / "ts.lst";
    String ts_lst_file;
    Files ts_files, sources;
    for (auto &[p, f] : t)
    {
        if (f->skip)
            continue;
        auto ext = p.extension().u8string();
        if (ext == ".ts")
            ts_files.insert(p);
        else
        {
            sources.insert(p);
            ts_lst_file += normalize_path(p) + "\n";
        }
    }

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
    for (auto &d : idirs)
        ts_lst_file += "-I" + normalize_path(d) + "\n";

    //
    t.writeFileOnce(ts_lst_fn, ts_lst_file);

    // add update commands
    for (auto &ts : ts_files)
    {
        auto c = t.addCommand();
        c << cmd::prog(lupdate)
            << cmd::in(ts_lst_fn, cmd::Prefix{ "@" })
            << "-ts"
            << cmd::in(ts)
            << cmd::end();
        for (auto &f : sources)
            c << cmd::in(sources);
    }

    qt_add_translation(lrelease, t, ts_files);
}

#pragma sw header off

void build(Solution &s)
{
    auto &qt = s.addProject("qtproject.qt", "5.12.0");

    auto &tools = qt.addProject("tools");
    tools += Git("https://github.com/qt/qttools", "v{v}");

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

            formats.replaceInFileOnce("qm.cpp", "Q_CONSTRUCTOR_FUNCTION(initQM)",
                "int initTS();Q_CONSTRUCTOR_FUNCTION(initTS)Q_CONSTRUCTOR_FUNCTION(initQM )");
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

            update.replaceInFileOnce("src/linguist/lupdate/main.cpp",
                "int main(int argc, char **argv)",
                "int initTS(); Q_CONSTRUCTOR_FUNCTION(initTS) int main(int argc, char **argv )");
        }
    }
}
