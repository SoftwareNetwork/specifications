#pragma sw require header org.sw.demo.qtproject.qt.base.tools.moc-*
#pragma sw require header org.sw.demo.qtproject.qt.tools.linguist.release-*

#pragma sw header on

static std::tuple<StaticLibraryTarget &, Files> qt_translations_create_qm_files(const DependencyPtr &base, TargetBase &t)
{
    auto ts = std::make_shared<Dependency>(base->package);
    ts->package.ppath /= "translations";

    auto qt_trs_r = FileRegex(ts->getPackage().resolve().getDirSrc2() / "translations", std::regex(".*\\.ts"));
    auto &qt_trs = t.addStaticLibrary("qt_translations");
    qt_trs += qt_trs_r;
    qt_trs += ts;
    return { qt_trs, qt_tr(base, qt_trs) };
}

static void qt_translations_rcc(const DependencyPtr &base, NativeExecutedTarget &t, path in, const Files &qms)
{
    if (!in.is_absolute())
        in = t.SourceDir / in;

    auto r = std::make_shared<Dependency>(base->package);
    r->package.ppath /= "base.tools.rcc";

    for (auto &q : qms)
    {
        auto o = t.BinaryDir / q.filename();
        SW_MAKE_EXECUTE_BUILTIN_COMMAND_AND_ADD(copy_cmd, t, "sw_copy_file");
        copy_cmd->args.push_back(normalize_path(q));
        copy_cmd->args.push_back(normalize_path(o));
        copy_cmd->addInput(q);
        copy_cmd->addOutput(o);
        copy_cmd->name = "copy: " + normalize_path(o);
        copy_cmd->maybe_unused = builder::Command::MU_ALWAYS;
    }

    t.configureFile(in,
        t.BinaryDir / in.filename(),
        ConfigureFlags::CopyOnly);
    rcc(r, t,
        t.BinaryDir / in.filename())
        .c->working_directory = t.BinaryDir;
}

static void qt_translations_rcc(const DependencyPtr &base, TargetBase &tr_base, NativeExecutedTarget &t, path in)
{
    auto [qt_trs, qms] = qt_translations_create_qm_files(base, tr_base);
    qt_translations_rcc("org.sw.demo.qtproject.qt-*"_dep, t, in, qms);
    (t + qt_trs)->Dummy = true;
}

#pragma sw header off

void build(Solution &s)
{
    auto &qt = s.addProject("qtproject.qt", "5.12.0");

    auto &translations = qt.addTarget<LibraryTarget>("translations");
    translations += Git("https://github.com/qt/qttranslations", "v{v}");

    //translations.Empty = true;
    translations += "translations/.*\\.ts"_rr;
}
