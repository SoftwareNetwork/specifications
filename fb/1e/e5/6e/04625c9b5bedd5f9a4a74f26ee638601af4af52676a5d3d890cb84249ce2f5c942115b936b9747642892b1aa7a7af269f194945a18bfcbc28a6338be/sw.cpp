#pragma sw require header pub.egorpugin.primitives.tools.syncqt

#pragma sw header on

#pragma sw require pub.egorpugin.primitives.hash

//#include <iostream>
#include <regex>

#define SW_QT_ADD_MOC_DEPS(t)                                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        for (auto &m : mocs)                                                                                           \
        {                                                                                                              \
            auto mg = File(m, t.getFs()).getGenerator();                                                               \
            for (auto &c : sqt)                                                                                        \
            {                                                                                                          \
                mg->dependencies.insert(File(c, t.getFs()).getGenerator());                                            \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

// http://doc.qt.io/qt-6/moc.html
struct moc_file
{
    path p;
    bool build = false;
    path from_file;
    bool from_cpp = false;
    path ext;
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
        t.getMergeObject().addDefinitionsAndIncludeDirectories(*this);
        Command::prepare();
        for (auto &&a : arguments) {
            auto s = a->toString();
            if (s.find("-D") == 0 && s.find(" ") != -1 && s.find("=") != -1 && s.back() != '\"') {
                auto p = s.find("=");
                a = std::make_unique<primitives::command::SimpleArgument>(s.substr(0, p) + "\"" + s.substr(p + 2) + "\"");
            }
        }
    }
};

static auto find_idirs(NativeExecutedTarget &t)
{
    // get current idirs
    // we do not need them as args, because args will get all target idirs and defs
    FilesOrdered idirs;
    //t.IncludeDirectories.insert(t.BinaryDir); // add bdir early
    t.TargetOptionsGroup::iterate(
        [&idirs](auto &v, auto i)
    {
        if (i == InheritanceType::Interface)
            return;
        for (auto &i : v.IncludeDirectories)
            idirs.push_back(i);
    });
    return idirs;
}

static Files automoc(const DependencyPtr &moc, NativeExecutedTarget &t, const std::vector<moc_file> &additions = {})
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

    // before dry run
    t.addDummyDependency(moc);

    if (t.DryRun)
        return {};

    auto idirs = find_idirs(t);

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

    auto has_mm = t.getBuildSettings().TargetOS.isApple();
    auto is_cpp_ext = [&](auto &&ext) {
        return ext == ".cpp" || (has_mm && ext == ".mm");
    };

    std::vector<moc_file> files;
    for (auto &[p, f] : t)
    {
        if (f->skip)
            continue;
        if (is_cpp_ext(p.extension())
            && p.filename() != "qrc_sw_qm.cpp"s
            )
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
                    f.ext = p.extension();
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
        auto ext = to_string(p.extension().u8string());
        if (is_cpp_ext(ext)
            && p.filename() != "qrc_sw_qm.cpp"s
            ||
            (ext.size() > 1 && HeaderFileExtensionsSet.find(ext.substr(1)) != HeaderFileExtensionsSet.end()))
        {
            auto s = read_file(p);
            if (0
                || s.find("Q_OBJECT") != s.npos
                || s.find("Q_GADGET") != s.npos
                || s.find("Q_NAMESPACE") != s.npos
                // ?
                //|| s.find("QMETATYPE_CONVERTER") != s.npos
                //|| s.find("QT_METATYPE_CONVERT_ID_TO_TYPE") != s.npos
                //|| s.find("Q_CONSTRUCTOR_FUNCTION") != s.npos
                )
            {
                moc_file f;
                f.p = p.parent_path() / p.stem();
                f.build = true;
                f.ext = p.extension();
                files.push_back(f);
            }
        }
    }

    auto moc_dir = t.BinaryDir / "moc";
    Files used_files;
    Files mocs;
    for (auto &f : files)
    {
#ifdef _WIN32
        f.p = normalize_path_windows(f.p);
#endif

        auto dir = sha256(to_string(f.p.parent_path().u8string())).substr(0, 8);
        String fn;

        if (f.from_cpp)
            f.p += f.ext;
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
                //std::cout << f.p << "\n";
                if (!f.ext.empty()) {
                    f.p += f.ext;
                } else {
                    f.p += ".cpp";
                }
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

        auto c = t.addCommand(std::make_shared<MocCommand>(t));
        c << cmd::prog(moc)
            << cmd::in(f.p);
        if (!f.from_file.empty())
            c << "-i";
        // currently cmd out will add file to merge object and pvt
        // but below we exclude only from pvt
        //c << "-o" << cmd::out(o);
        c->push_back("-o");
        c->push_back(o);
        c->addInput(f.p);
        c->addOutput(o);

        if (f.build)
            t += o;
        else
            t -= o;
        mocs.insert(o);
    }

    for (auto &f : files)
    {
        auto dir = sha256(to_string(f.p.parent_path().u8string())).substr(0, 8);
        t.IncludeDirectories.insert(moc_dir / dir);
    }

    return mocs;
}

// http://doc.qt.io/qt-6/rcc.html
static Files rcc_read_files(NativeExecutedTarget &t, const path &fn)
{
    if (t.DryRun)
        return {};

    static std::regex r("<file([^<]+)");
    std::smatch m;
    auto s = read_file(fn);
    Files files;
    while (std::regex_search(s, m, r))
    {
        auto f = m[1].str();
        path f2 = f.substr(f.find('>') + 1);
        if (fs::exists(fn.parent_path() / f2))
            f2 = fn.parent_path() / f2;
        else if (!t.check_absolute(f2, true))
        {
            auto rel = (fn.parent_path() / f2).lexically_relative(t.SourceDir).lexically_normal();
            if (!rel.empty() && t.check_absolute(rel, true))
                f2 = rel;
            else
                t.check_absolute(f2);
        }
        files.insert(f2);
        s = m.suffix().str();
    }

    return files;
}

static auto rcc(const DependencyPtr &rcc, NativeExecutedTarget &t, const path &fn)
{
    // before dry run
    t.addDummyDependency(rcc);

    auto c = t.addCommand();
    if (t.DryRun)
        return c;

    auto files = rcc_read_files(t, fn);

    auto outfilename = fn.filename().stem();
    auto outfile = t.BinaryDir / ("qrc_" + to_string(outfilename.u8string()) + ".cpp");
    c << cmd::prog(rcc)
        << "--name" << outfilename
        << "--output" << cmd::out(outfile)
        << cmd::in(fn)
        << cmd::end()
        << cmd::in(files);
    return c;
}

static void rcc(const DependencyPtr &rcc, NativeExecutedTarget &t, const Files &files)
{
    for (auto &f : files)
        ::rcc(rcc, t, f);
}

struct RccData
{
    std::unordered_map<path, String /* alias */> files;
    String name;
    String prefix;
    String base; // unused for now
    String outfilename_prefix;
};

static void rcc(const DependencyPtr &rcc, NativeExecutedTarget &t, const RccData &d)
{
    String s;
    s += "<!DOCTYPE RCC><RCC version=\"1.0\">";
    s += "<qresource";
    if (!d.prefix.empty())
        s += " prefix=\"" + d.prefix + "/\"";
    s += ">";
    for (auto &[fn, alias] : d.files)
    {
        s += "<file alias=\"";
        if (alias.empty())
            s += fn.filename().string();
        else
            s += alias;
        auto p = fn;
        if (!p.is_absolute())
            p = t.SourceDir / p;
        s += "\">" + to_string(normalize_path(p)) + "</file>";
    }
    s += "</qresource>";
    s += "</RCC>";
    auto in = t.BinaryDir / (d.name + ".qrc");
    write_file_if_different(in, s);
    File(in, t.getFs()).setGenerated(true);

    auto outfilename = d.outfilename_prefix + d.name;
    auto outfile = t.BinaryDir / ("qrc_" + outfilename + ".cpp");
    auto c = t.addCommand();
    c << cmd::prog(rcc)
        << "--name" << outfilename
        << "--output" << cmd::out(outfile)
        << cmd::in(in)
        << cmd::end();
    for (auto &[fn, _] : d.files)
        c << cmd::in(fn);
}

static void qt_rcc(const DependencyPtr &rcc, NativeExecutedTarget &t)
{
    for (const auto &[p, _] : t[".*\\.qrc"_rr])
        ::rcc(rcc, t, p);
}

// http://doc.qt.io/qt-6/uic.html
static void uic(const DependencyPtr &uic, NativeExecutedTarget &t, const path &fn)
{
    auto outfilename = fn.filename().stem();
    auto outfile = t.BinaryDir / ("ui_" + to_string(outfilename.u8string()) + ".h");
    auto c = t.addCommand();
    c << cmd::prog(uic)
        << "-o" << cmd::out(outfile)
        << cmd::in(fn);
    t += outfile;
}

static void uic(const DependencyPtr &uic, NativeExecutedTarget &t, const Files &files)
{
    for (auto &f : files)
        ::uic(uic, t, f);
}

static void qt_uic(const DependencyPtr &uic, NativeExecutedTarget &t)
{
    for (const auto &[p, _] : t[".*\\.ui"_rr])
        ::uic(uic, t, p);
}

static void qt_moc_rcc_uic(const DependencyPtr &base, NativeExecutedTarget &t)
{
    auto moc = std::make_shared<Dependency>(base->package);
    moc->package.ppath /= "base.tools.moc";

    auto rcc = std::make_shared<Dependency>(base->package);
    rcc->package.ppath /= "base.tools.rcc";

    auto uic = std::make_shared<Dependency>(base->package);
    uic->package.ppath /= "base.tools.uic";

    automoc(moc, t);
    qt_rcc(rcc, t);
    qt_uic(uic, t);
}

static void platform_files(NativeExecutedTarget &t)
{
    // remove platform
    t.AllowEmptyRegexes = true;
    t -= ".*.mm"_rr;
    t -= ".*_mac.*"_rr;
    t -= ".*_unix.*"_rr;
    t -= ".*_win.*"_rr;
    t -= ".*_android.*"_rr;
    t -= ".*_systemv.*"_rr;
    t -= ".*_haiku.*"_rr;
    t -= ".*_nacl.*"_rr;
    t -= ".*_winrt.*"_rr;
    t -= ".*_glib.*"_rr;
    t -= ".*_inotify.*"_rr;
    t -= ".*_kqueue.*"_rr;
    t -= ".*_linux.*"_rr;
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
    t -= ".*_x11.*"_rr;
    t -= ".*_wasm.*"_rr;
    t -= ".*_coretext.*"_rr;
    t -= ".*_osx.*"_rr;
    t -= ".*_freebsd.*"_rr;
    if (t.getBuildSettings().TargetOS.Type == OSType::Windows) {
        t += ".*_win.*"_rr;
    } else if (t.getBuildSettings().TargetOS.Type == OSType::Linux) {
        t += ".*_unix.*"_rr;
        t += ".*_posix.*"_rr;
        t += ".*_systemv.*"_rr;
        t += ".*_tz.*"_rr;
        t += ".*_qpa.*"_rr;
        t += ".*_stub.*"_rr;
        //t += ".*_generic.*"_rr;
    } else if (t.getBuildSettings().TargetOS.isApple()) {
        t += ".*qcf.*"_rr;
        t += ".*_kqueue.*"_rr;
        t += ".*_kqueue_p.*"_rr;
        t += ".*_unix.*"_rr;
        t += ".*_posix.*"_rr;
        t += ".*_systemv.*"_rr;
        t += ".*.mm"_rr;
        t += ".*_mac.*"_rr;
        t += ".*_macx.*"_rr;
        t += ".*_coretext.*"_rr;
        t += ".*_cf.*"_rr;
        t += ".*_cf_p.*"_rr;
        t += ".*_fsevents.*"_rr;
        t += ".*_fsevents_p.*"_rr;
        t += ".*_qpa.*"_rr;
    }
    t -= "doc/.*"_rr;
    t.AllowEmptyRegexes = false;
};

// qt tools
static Files qt_add_translation(const DependencyPtr &lrelease, NativeExecutedTarget &t, const Files &ts_files)
{
    // before dry run
    t.addDummyDependency(lrelease);

    if (t.DryRun)
        return {};

    Files out;
    for (auto &ts : ts_files)
    {
        auto c = t.addCommand();
        auto o = t.BinaryDir / (ts.filename().stem() += ".qm");
        c << cmd::prog(lrelease)
            << cmd::in(ts)
            << "-qm"
            << cmd::out(o);
        out.insert(o);
    }
    return out;
}

static Files qt_create_translation(const DependencyPtr &lupdate, const DependencyPtr &lrelease, NativeExecutedTarget &t)
{
    // before dry run
    t.addDummyDependency(lupdate);
    t.addDummyDependency(lrelease);

    if (t.DryRun)
        return {};

    auto ts_lst_fn = t.BinaryDir / "ts.lst";
    String ts_lst_file;
    Files ts_files, sources;
    for (auto &[p, f] : t)
    {
        auto ext = p.extension().string();
        bool ok = 0
            || sw::isCppHeaderFileExtension(ext)
            || sw::isCppSourceFileExtensions(ext)
            || ext == ".ui" // include qt .ui files
        ;
        if (ok)
            ;
        else if (f->skip)
            continue;
        if (p.extension() == ".ts")
            ts_files.insert(p);
        else
        {
            if (ok)
                ;
            else if (!f->isActive())
                continue;
            sources.insert(p);
            ts_lst_file += to_string(normalize_path(p)) + "\n";
        }
    }

    auto idirs = find_idirs(t);
    for (auto &d : idirs)
        ts_lst_file += "-I" + to_string(normalize_path(d)) + "\n";

    //
    t.writeFileOnce(ts_lst_fn, ts_lst_file);

    // add create (update) commands
    auto c = t.addCommand();
    c << cmd::prog(lupdate)
        << cmd::in(ts_lst_fn, cmd::Prefix{ "@" })
        << "-ts";
    for (auto &ts : ts_files)
        c << cmd::out(ts);
    c << cmd::end();
    for (auto &f : sources) {
        c.getCommand()->addInput(f);
    }

    return qt_add_translation(lrelease, t, ts_files);
}

static auto qt_create_translation(const DependencyPtr &base, NativeExecutedTarget &t)
{
    auto update = std::make_shared<Dependency>(base->package);
    update->package.ppath /= "tools.linguist.update";

    auto release = std::make_shared<Dependency>(base->package);
    release->package.ppath /= "tools.linguist.release";

    return qt_create_translation(update, release, t);
}

static auto qt_tr(const DependencyPtr &base, NativeExecutedTarget &t)
{
    auto qm = qt_create_translation(base, t);

    RccData d;
    d.prefix = "/ts";
    d.name = "qm";
    d.outfilename_prefix = "sw_"; //"qmake_";
    for (auto &p : qm)
        d.files[p];
    auto rccbin = std::make_shared<Dependency>(base->package);
    rccbin->package.ppath /= "base.tools.rcc";
    rcc(rccbin, t, d);

    return qm;
}

// qt translations
static std::tuple<StaticLibraryTarget &, Files> qt_translations_create_qm_files(const DependencyPtr &base, TargetBase &t, NativeExecutedTarget &nt)
{
    auto ts = std::make_shared<Dependency>(base->package);
    ts->package.ppath /= "translations";

    auto qt_trs_r = FileRegex(nt.getFile(ts, "translations"), ".*\\.ts", false);
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
        auto c = t.addCommand(SW_VISIBLE_BUILTIN_FUNCTION(copy_file));
        c << cmd::in(q) << cmd::out(q.filename());
    }

    t.configureFile(in,
        t.BinaryDir / in.filename(),
        ConfigureFlags::CopyOnly);
    rcc(r, t,
        t.BinaryDir / in.filename())
        ->working_directory = t.BinaryDir;
}

static void qt_translations_rcc(const DependencyPtr &base, TargetBase &tr_base, NativeExecutedTarget &t, path in)
{
    auto[qt_trs, qms] = qt_translations_create_qm_files(base, tr_base, t);
    qt_translations_rcc(base, t, in, qms);
    t.addDummyDependency(qt_trs);
}

#pragma sw header off

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

struct QtFeature
{
    String name;
    bool enabled = false;
    Strings ifdefs; // or!

    bool operator==(const QtFeature &rhs) const
    {
        return name == rhs.name;
    }

    bool operator<(const QtFeature &rhs) const
    {
        return name < rhs.name;
    }

    String print() const
    {
        String s;
        if (!ifdefs.empty())
        {
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
            return (QtFeature&)*i;
        return (QtFeature&)*insert({ k }).first;
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

        if (name == "q")
        {
            actual_name = "QtCore";
            t.writeFileOnce(actual_name + "/QtConfig", "#include \"" + config_fn + "\"");
        }
        else
        {
            config_fn += "-";
            private_config_fn += "-";
        }

        config_fn += "config.h";
        private_config_fn += "config_p.h";

        auto all = config.public_.print() + "\n" + config.private_.print();
        t.writeFileOnce(actual_name + "/" + config_fn, all);
        t.writeFileOnce(actual_name + "/private/" + private_config_fn, all);
        t.writeFileOnce("private/" + private_config_fn, all);
        t += IncludeDirectory(t.BinaryDir / actual_name / "private");

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

auto automoc(NativeExecutedTarget &moc, NativeExecutedTarget &t, const std::vector<moc_file> &additions = {})
{
    return ::automoc(moc.getDependency(), t, additions);
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
    p += Git("https://github.com/qt/" + qt_name, "v{v}");
    if (p.isLocal() && fs::exists(p.SourceDir / qt_name))
        p.setSourceDirectory(qt_name);
    return p;
}

void build(Solution &s)
{
    QtLibrary qt_desc{
        "q",
        // config
        {
            // public
            {
                // features
                {
                    {"accessibility", true},
        {"build_all", true},
        {"c__11", true},
        {"c__14", true},
        {"c__1z", true},
        {"concurrent", true},
        {"cxx11_future", true},
        {"datetimeparser", true},
        {"debug_and_release", true},
        {"force_asserts", false},
        {"framework", false},
        {"future", true},
        {"getentropy", false},
        {"journald", false},
        {"pkg_config", false},
        {"processenvironment", true},
        {"rpath", false},
        {"separate_debug_info", false},
        {"shared", false},
        {"simulator_and_device", false},
        {"slog2", false},
        {"static", false},
        {"std_atomic64", true},
        {"syslog", false},
        {"thread", true},
                },
                // defs
                {
        {"QT_LARGEFILE_SUPPORT", "64"},
                },
            },
            // private
            {
                // features
                {
                    {"dbus_linked", false},
        {"dbus", true},
        {"gui", true},
        {"libudev", false},
        {"posix_fallocate", false},
        {"private_tests", false},
        {"reduce_exports", false},
        {"reduce_relocations", false},
        {"release_tools", false},
        {"system_zlib", false},
        {"widgets", true},
                },
            },
    },
    };

    QtLibrary qt_core_desc{
        "QtCore",
        // config
        {
            // public
            {
                // features
                {
                    {"animation", true},
        {"big_codecs", true},
        {"codecs", true},
        {"commandlineparser", true},
        {"datestring", true},
        {"easingcurve", true},
        {"filesystemiterator", true},
        {"filesystemwatcher", true},
        {"gestures", true},
        {"library", true},
        {"mimetype", true},
        {"mimetype_database", true},
        {"properties", true},
        {"qeventtransition", true},
        {"regularexpression", true},
        {"settings", true},
        {"sharedmemory", true},
        {"signaling_nan", true},
        {"statemachine", true},
        {"systemsemaphore", true},
        {"temporaryfile", true},
        {"textcodec", true},
        {"textdate", true},
        {"timezone", true},
        {"topleveldomain", true},
        {"translation", true},
        {"xmlstreamreader", true},
        {"xmlstream", true},
        {"xmlstreamwriter", true},
        {"zstd", true},

        {"hijricalendar", true},
        {"jalalicalendar", true},
        {"islamiccivilcalendar", true},

        {"concatenatetablesproxymodel", true},
        {"itemmodel", true},
        {"identityproxymodel", true},
        {"proxymodel", true},
        {"sortfilterproxymodel", true},
        {"stringlistmodel", true},
        {"transposeproxymodel", true},

        {"cborstreamreader", true},
        {"cborstreamwriter", true},
        {"binaryjson", true},

        //
        {"shortcut", true},
        {"cxx17_filesystem", true},
        {"publicsuffix_qt", true},
        {"publicsuffix_system", false},

        {"opensslv30", false},
                },
                // defs
                {
                    {"QT_NO_INOTIFY", ""},
        {"QT_NO_GLIB", ""},
        {"QT_NO_ICONV", ""},
        {"QT_NO_EVENTFD", ""},
        {"QT_NO_CLOCK_MONOTONIC", ""},
        {"QT_CONFIGURE_LIBLOCATION_TO_PREFIX_PATH", "\".\""},
                },
            },
            // private
            {
                // features
                {
                    {"clock_gettime", false},
        {"doubleconversion", true},
        {"glib", false},
        {"gnu_libiconv", false},
        {"iconv", false},
        {"inotify", false},
        {"libatomic", false},
        {"posix_libiconv", false},
        {"sha3_fast", true},
        {"system_doubleconversion", false},
        {"system_pcre", true},
        {"relocatable", true},
        {"system_libb2", true},
                },
            },
    },
    };

    QtLibrary qt_gui_desc{
        "QtGui",
        // config
        {
            // public
            {
                // features
                {
                    {"accessibility", true},
        {"action", true},
        {"angle", true},
        {"clipboard", true},
        {"colornames", true},
        {"cssparser", true},
        {"cursor", true},
        {"desktopservices", true},
        {"draganddrop", true},
        {"dynamicgl", true},
        {"filesystemmodel", true},
        {"highdpiscaling", true},
        {"imageformat_bmp", true},
        {"imageformat_jpeg", true},
        {"imageformatplugin", true},
        {"imageformat_png", true},
        {"imageformat_ppm", true},
        {"imageformat_xbm", true},
        {"imageformat_xpm", true},
        {"image_heuristic_mask", true},
        {"image_text", true},
        {"im", true},
        {"movie", true},
        {"opengles2", false},
        {"opengles31", false},
        {"opengles3", false},
        {"opengl", true},
        {"pdf", true},
        {"picture", true},
        {"sessionmanager", true},
        {"shortcut", true},
        {"standarditemmodel", true},
        {"systemtrayicon", true},
        {"tabletevent", true},
        {"texthtmlparser", true},
        {"textodfwriter", true},
        {"validator", true},
        {"vulkan", false},
        {"whatsthis", true},
        {"wheelevent", true},

        {"textmarkdownreader", true},
        {"textmarkdownwriter", true},
        {"raster_fp", true},
        {"system_textmarkdownreader", true},
        {"vsp2", false},
        {"undogroup", true},
        {"undocommand", true},
        {"undostack", true},
                },
                // defs
                {
                    {"QT_NO_XRENDER", ""},
        {"QT_NO_ACCESSIBILITY_ATSPI_BRIDGE", ""},
        {"QT_OPENGL_DYNAMIC", "true"},
        {"QT_OPENGL_ES_2_ANGLE", "true"},
        //{"QT_NO_FONTCONFIG", ""},
                },
            },
            // private
            {
                // features
                {
                    {"accessibility_atspi_bridge", false},
        {"android_style_assets", false},
        {"direct2d", true},
        {"directfb", false},
        {"directwrite3", true},
        {"directwrite2", true},
        {"directwrite", true},
        {"evdev", false},
        //{"fontconfig", false},
        {"freetype", true},
        {"gif", true},
        {"harfbuzz", true},
        {"ico", true},
        {"integrityfb", false},
        {"jpeg", true},
        {"libinput_axis_api", false},
        {"libinput", false},
        {"linuxfb", false},
        {"mirclient", false},
        {"mtdev", false},
        {"multiprocess", true},
        {"png", true},
        {"system_freetype", false},
        {"system_harfbuzz", true},
        {"system_jpeg", false},
        {"system_png", false},
        {"system_xcb", false},
        {"tslib", false},
        {"xinput2", false},
        //{"xkbcommon_evdev", false},
        //{"xkb", false},
        {"xlib", false},
        {"xrender", false},

        {"raster_64bit", true},
                },
            },
    },
    // deps
        {"QtCore"},
    };

    QtLibrary qt_widgets_desc{
        "QtWidgets",
        // config
        {
            // public
            {
                // features
                {
                    {"abstractbutton", true},
        {"abstractslider", true},
        {"buttongroup", true},
        {"calendarwidget", true},
        {"checkbox", true},
        {"colordialog", true},
        {"columnview", true},
        {"combobox", true},
        {"commandlinkbutton", true},
        {"completer", true},
        {"contextmenu", true},
        {"datawidgetmapper", true},
        {"datetimeedit", true},
        {"dialogbuttonbox", true},
        {"dialog", true},
        {"dial", true},
        {"dirmodel", true},
        {"dockwidget", true},
        {"effects", true},
        {"errormessage", true},
        {"filedialog", true},
        {"filesystemmodel", true},
        {"fontcombobox", true},
        {"fontdialog", true},
        {"formlayout", true},
        {"fscompleter", true},
        {"gestures", true},
        {"graphicseffect", true},
        {"graphicsview", true},
        {"groupbox", true},
        {"inputdialog", true},
        {"itemviews", true},
        {"keysequenceedit", true},
        {"label", true},
        {"lcdnumber", true},
        {"lineedit", true},
        {"listview", true},
        {"listwidget", true},
        {"mainwindow", true},
        {"mdiarea", true},
        {"menubar", true},
        {"menu", true},
        {"messagebox", true},
        {"paint_debug", true},
        {"progressbar", true},
        {"progressdialog", true},
        {"pushbutton", true},
        {"radiobutton", true},
        {"resizehandler", true},
        {"rubberband", true},
        {"scrollarea", true},
        {"scrollbar", true},
        {"scroller", true},
        {"sizegrip", true},
        {"slider", true},
        {"spinbox", true},
        {"splashscreen", true},
        {"splitter", true},
        {"stackedwidget", true},
        {"statusbar", true},
        {"statustip", true},
        {"style_android", false},
        {"style_fusion", true},
        {"style_mac", false},
        {"syntaxhighlighter", true},
        {"tabbar", true},
        {"tableview", true},
        {"tablewidget", true},
        {"tabwidget", true},
        {"textbrowser", true},
        {"textedit", true},
        {"toolbar", true},
        {"toolbox", true},
        {"toolbutton", true},
        {"tooltip", true},
        {"treeview", true},
        {"treewidget", true},
        {"undocommand", true},
        {"undogroup", true},
        {"undostack", true},
        {"undoview", true},
        {"whatsthis", true},
        {"widgettextcontrol", true},
        {"wizard", true},
                },
            },
            // private
            {
                // features
                {
                    {"gtk3", false},
                },
            },
    },
    // deps
        {
            "QtCore",
            "QtGui",
        },
    };

    QtLibrary qt_network_desc{
        "QtNetwork",
        // config
        {
            // public
            {
                // features
                {
                    {"bearermanagement", true},
        {"dnslookup", true},
        {"dtls", true},
        {"ftp", true},
        {"http", true},
        {"localserver", true},
        {"networkdiskcache", true},
        {"networkinterface", true},
        {"networkproxy", true},
        {"ocsp", true},
        {"opensslv11", true},
        {"schannel", false},
        {"sctp", false},
        {"socks5", true},
        {"ssl", true},
        {"udpsocket", true},

        {"brotli", true},
                },
                // defs
                {
                    {"QT_NO_SCTP", ""},
        {"QT_NO_IPV6IFNAME", ""},
        {"QT_NO_GETADDRINFO", ""},
        {"QT_NO_GETIFADDRS", ""},
                },
            },
            // private
            {
                // features
                {
                    {"libproxy", false},
        {"openssl_linked", false}, // true?
        {"openssl", true},
        {"securetransport", false},
        {"system_proxies", true},
                },
            },
    },
    // deps
        {"QtCore"},
    };

    QtLibrary qt_opengl_desc{
        "QtOpenGL",
        // config
        {
            // public
            {
                // features
                {
                    {"opengl", true},
                },
            },
    },
    // deps
        {"QtGui"},
    };

    QtLibrary qt_xml_desc{
        "QtXml",
        // config
        {
            // public
            {
                // features
                {
                    {"dom", true},
                },
            },
    },
    // deps
        {"QtCore"},
    };

    QtLibrary qt_sql_desc{
        "QtSql",
        // config
        {
            // public
            {
                // features
                {
                    {"sqlmodel", true},
                },
            },
    },
    // deps
        {"QtCore"},
    };

    QtLibrary qt_printsupport_desc{
        "QtPrintSupport",
        // config
        {
            // public
            {
                // features
                {
                    {"printer", true},
        {"printdialog", true},
        {"printpreviewwidget", true},
        {"printpreviewdialog", true},
                }/*,
                {
                    {"QT_NO_CUPS", ""},
        {"QT_NO_CUPSJOBWIDGET", ""},
                },*/
            }
    },
    // deps
        {
            "QtCore",
            "QtGui",
            "QtWidgets",
        },
    };

    QtLibrary qt_svg_desc{
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

    QtLibrary qt_tools_desc{
        "QtTools",
        // config
        {
            // public
            {},
            // private
            {
                // features
                {
                    {"clangcpp", false},
                }
            }
        },
        // deps
        {
            "QtXml",
        },
    };

    //
    auto &qt = s.addProject("qtproject.qt", "6.5.0");

    auto &base = add_subproject<Project>(qt, "base");

    auto &base_tools = base.addDirectory("tools");
    auto &moc = base_tools.addTarget<ExecutableTarget>("moc");
    auto &rcc = base_tools.addTarget<ExecutableTarget>("rcc");
    auto &uic = base_tools.addTarget<ExecutableTarget>("uic");
    auto &tracegen = base_tools.addTarget<ExecutableTarget>("tracegen");
    auto &tracepointgen = base_tools.addTarget<ExecutableTarget>("tracepointgen");

    // to setup descs
    auto &core = base.addTarget<LibraryTarget>("core");
    auto &gui = base.addTarget<LibraryTarget>("gui");
    auto &network = base.addTarget<LibraryTarget>("network");
    auto &widgets = base.addTarget<LibraryTarget>("widgets");
    auto &opengl = base.addTarget<LibraryTarget>("opengl");
    auto &dbus = base.addTarget<LibraryTarget>("dbus");

    qt_desc.config.public_.definitions["QT_VERSION_MAJOR"] = core.Variables["PACKAGE_VERSION_MAJOR"].toString();
    qt_desc.config.public_.definitions["QT_VERSION_MINOR"] = core.Variables["PACKAGE_VERSION_MINOR"].toString();
    qt_desc.config.public_.definitions["QT_VERSION_PATCH"] = core.Variables["PACKAGE_VERSION_PATCH"].toString();
    qt_desc.config.public_.definitions["QT_VERSION_STR"] = "\"" + core.getPackage().getVersion().toString() + "\"";

    // early decls
    auto &xml = base.addTarget<LibraryTarget>("xml");
    auto &qlalr = base_tools.addTarget<ExecutableTarget>("qlalr");

    auto write_tracepoints = [&tracegen, &tracepointgen](auto &t, auto && ... args)
    {
        auto provider = t.getPackage().getPath().back();
        auto providername = "qt"s + provider;
        auto fn = path{providername + ".tracepoints"};

        {
            auto c = t.addCommand();
            c << cmd::prog(tracepointgen);
            c << providername;
            c << cmd::out(fn);
            ((c << cmd::in(args)),...);
        }

        auto c = t.addCommand();
        c << cmd::prog(tracegen);
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows) {
            c
                << "etw"
                << cmd::in(fn)
                << cmd::out("qt" + t.getPackage().getPath().back() + "_tracepoints_p.h")
            ;
        } else if (t.getBuildSettings().TargetOS.Type == OSType::Linux) {
            c
                << "lttng"
                << cmd::in(fn)
                << cmd::out("qt" + t.getPackage().getPath().back() + "_tracepoints_p.h")
            ;
        } else {
            t.writeFileOnce("qt" + t.getPackage().getPath().back() + "_tracepoints_p.h", R"(
                #define Q_TRACE(...)
                #define Q_TRACE_SCOPE(...)
                #define Q_TRACE_EXIT(...)
                #define Q_HAS_TRACEPOINTS 0
                #define Q_TRACE_ENABLED(...) 0
                )");
        }
        return c;
    };

    auto exports_setup = [&](auto &t, const String &custom_name = {}, bool bootstrap = false) {
        auto name = t.getPackage().getPath().back();
        if (!custom_name.empty())
            name = custom_name;
        auto lower = boost::to_lower_copy(name);
        auto sentence = lower;
        sentence[0] = toupper(sentence[0]);
        if (lower == "dbus")
            sentence = "DBus";
        if (lower == "printsupport")
            sentence = "PrintSupport";
        if (lower == "opengl")
            sentence = "OpenGL";
        if (lower == "core5compat")
            sentence = "Core5Compat";
        if (custom_name == "WaylandClient")
            sentence = "WaylandClient";
        if (custom_name == "webenginecore")
            sentence = "WebEngineCore";
        if (custom_name == "qmlmodels")
            sentence = "QmlModels";
        auto upper = boost::to_upper_copy(name);
        //t += sw::ApiNameType{"Q_" + upper + "_EXPORT"};
        //t += sw::ApiNameType{"Q_" + upper + "_PRIVATE_EXPORT"};
        /*t.writeFileOnce("Qt" + sentence + "/qt" + lower + "exports.h",
                "#pragma once\n"
                "#define QT_" + upper + "_REMOVED_SINCE(x,y) QT_DEPRECATED_SINCE(x,y)\n"
                "#define QT_" + upper + "_INLINE_SINCE(x,y) QT_IF_DEPRECATED_SINCE(x,y, inline, /* not inline /)\n"
                "#define QT_" + upper + "_INLINE_IMPL_SINCE(x,y) QT_IF_DEPRECATED_SINCE(x,y, 1, 0)\n"
        );*/
        t.Variables["module_include_name"] = "Qt" + sentence;
        auto header_base_name = "qt"s + lower + "exports";
        t.Variables["header_base_name"] = header_base_name;
        t.Variables["header_base_name_upper"] = "SW_QT_EXPORTS_GUARD_" + upper;
        t.Variables["module_define_infix"] = upper;
        t.configureFile(t.getFile(core, "cmake/modulecppexports.h.in"), "Qt" + sentence + "/" + header_base_name + ".h");
        t.configureFile(t.getFile(core, "cmake/modulecppexports_p.h.in"), "Qt" + sentence + "/private/" + header_base_name + "_p.h");
        if (!bootstrap)
            t += Definition("QT_BUILD_" + upper + "_LIB");
        //t.writeFileOnce("Qt" + sentence + "/private/qt" + lower + "exports_p.h");
    };
    auto common_setup = [&](auto &t, const String &custom_name = {}) {
        t += cpp20;
        if (t.getCompilerType() == CompilerType::MSVC || t.getCompilerType() == CompilerType::ClangCl) {
            t.Public.CompileOptions.push_back("/Zc:__cplusplus");
            // cpp17 + msvc requires /permissive-
            // when client app is built with cpp17, there will be build error
            t.Public.CompileOptions.push_back("/permissive-");
        }
        if (t.getBuildSettings().TargetOS.Type != OSType::Windows) {
            t.ExportAllSymbols = true;
        }
        exports_setup(t, custom_name);
    };
    auto set_apple_flags = [](auto &t) {
        if (t.getBuildSettings().TargetOS.isApple()) {
            if (t.getBuildSettings().TargetOS.Arch != ArchType::aarch64) {
                ////t.CompileOptions.push_back("-mavx2");
                //t.CompileOptions.push_back("-march=x86-64-v3");
                //t.CompileOptions.push_back("-mf16c");
                if (t.getContext().getHostOs().Arch != ArchType::aarch64) {
                    t.CompileOptions.push_back("-march=native");
                }
            }
        }
    };

    auto make_qt_plugin = [](auto &t, const String &name)
    {
        t += sw::Static, "QT_STATICPLUGIN"_def;

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

    // base
    {
        auto &third_party = base.addDirectory("third_party");

        auto &iaccessible2 = third_party.addTarget<StaticLibraryTarget>("iaccessible2");
        {
            iaccessible2 -= "src/3rdparty/iaccessible2/generated/.*"_rr;
            iaccessible2.Public += "src/3rdparty/iaccessible2/generated"_idir;
            if (iaccessible2.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                iaccessible2.Private += "UNICODE"_d;
                iaccessible2 += "src/3rdparty/iaccessible2/generated/.*"_rr;
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
            else
            {
                iaccessible2.AutoDetectOptions = false;
                iaccessible2.Empty = false;
            }
        }

        auto &wintab = third_party.addTarget<StaticLibraryTarget>("wintab");
        {
            wintab += "src/3rdparty/wintab/.*"_rr;
            wintab.Public += "src/3rdparty/wintab"_idir;
        }

        auto &forkfd = third_party.addTarget<StaticLibraryTarget>("forkfd");
        {
            forkfd -= "src/3rdparty/forkfd/.*"_rr;
            if (forkfd.getBuildSettings().TargetOS.Type != OSType::Windows && !forkfd.getBuildSettings().TargetOS.isApple())
            {
                forkfd += c99;
                forkfd += "src/3rdparty/forkfd/forkfd.c";
                forkfd += "FORKFD_DISABLE_FORK_FALLBACK"_def;
                forkfd.Public += "src/3rdparty/forkfd"_idir;
            }
            else
            {
                forkfd.AutoDetectOptions = false;
                forkfd.Empty = false;
            }
        }

        auto &tinycbor = third_party.addTarget<StaticLibraryTarget>("tinycbor");
        {
            tinycbor.setRootDirectory("src/3rdparty/tinycbor");
        }

        auto &psl = third_party.addTarget<StaticLibraryTarget>("psl");
        {
            psl += "src/3rdparty/libpsl/.*\\.[hc]"_rr;
        }

        auto mkspecs = [](auto &&t) {
            if (t.getCompilerType() == CompilerType::MSVC || t.getCompilerType() == CompilerType::ClangCl) {
                if (t.getBuildSettings().TargetOS.Arch == ArchType::aarch64) {
                    t.Public += "mkspecs/win32-arm64-msvc"_idir;
                } else {
                    t.Public += "mkspecs/win32-msvc"_idir;
                }
            } else if (t.getCompilerType() == CompilerType::GNU) {
                t.Public += "mkspecs/linux-g++-64"_idir;
            }

            if (t.getBuildSettings().TargetOS.isApple())
            {
                if (t.getBuildSettings().TargetOS.Type == OSType::Macos)
                {
                    // macx-xcode
                    if (t.getCompilerType() == CompilerType::GNU)
                        t.Public += "mkspecs/macx-g++"_idir;
                    else
                        t.Public += "mkspecs/macx-clang"_idir;
                }

                t.Protected += "CoreFoundation"_framework;
                t.Protected += "CoreServices"_framework;
                t.Protected += "Foundation"_framework;
                t.Protected += "Security"_framework;
                t.Protected += "AppKit"_framework;
                t.Protected += "IOKit"_framework;
            }
        };

        auto &bootstrap = base_tools.addTarget<StaticLibraryTarget>("bootstrap");
        {
            common_setup(bootstrap);
            {
                bootstrap -=
                    "src/3rdparty/blake2/.*"_rr,
                    "src/3rdparty/md4/.*"_rr,
                    "src/3rdparty/rfc6234/.*"_rr,
                    "src/3rdparty/sha1/.*"_rr,
                    "src/3rdparty/sha3/.*"_rr
                    ;

                bootstrap +=
                    "mkspecs/.*\\.h"_rr,
                    "src/3rdparty/sha1/sha1.cpp",
                    //"src/corelib/compat/removed_api.cpp",
                    "src/corelib/.*\\.h"_rr,
                    "src/corelib/global/archdetect.cpp",
                    "src/corelib/global/qassert.cpp",
                    "src/corelib/global/qglobal.cpp",
                    "src/corelib/global/qendian.cpp",
                    "src/corelib/global/qfloat16.cpp",
                    "src/corelib/global/qfloat16tables.cpp",
                    "src/corelib/global/qlogging.cpp",
                    "src/corelib/global/qmalloc.cpp",
                    "src/corelib/global/qnumeric.cpp",
                    "src/corelib/global/qoperatingsystemversion.cpp",
                    "src/corelib/global/qoperatingsystemversion_darwin.mm",
                    "src/corelib/global/qoperatingsystemversion_win.cpp",
                    "src/corelib/global/qrandom.cpp",
                    "src/corelib/global/qtenvironmentvariables.cpp",
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
                    //"src/corelib/kernel/qcoreglobaldata.cpp",
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
                    "src/corelib/serialization/qxmlstreamgrammar.cpp",
                    "src/corelib/serialization/qxmlutils.cpp",
                    "src/corelib/text/qbytearray.cpp",
                    "src/corelib/text/qbytearraylist.cpp",
                    "src/corelib/text/qbytearraymatcher.cpp",
                    "src/corelib/text/qchar.cpp",
                    "src/corelib/text/qlatin1stringmatcher.cpp",
                    "src/corelib/text/qlocale.cpp",
                    "src/corelib/text/qlocale_tools.cpp",
                    "src/corelib/text/qregularexpression.cpp",
                    "src/corelib/text/qstring.cpp",
                    "src/corelib/text/qstringbuilder.cpp",
                    "src/corelib/text/qstringconverter.cpp",
                    "src/corelib/text/qstringlist.cpp",
                    "src/corelib/text/qstringmatcher.cpp",
                    "src/corelib/text/qunicodetables.cpp",
                    "src/corelib/text/qvsnprintf.cpp",
                    "src/corelib/time/qcalen.*"_rr,
                    "src/corelib/time/qdatetime.cpp",
                    "src/corelib/time/qlocaltime.cpp",
                    "src/corelib/time/qgregorian.*"_rr,
                    "src/corelib/time/qroman.*"_rr,
                    "src/corelib/time/qtimezone.cpp",
                    //"src/corelib/time/qtimezoneprivate.cpp",
                    //"src/corelib/time/qtimezoneprivate_tz.cpp",
                    "src/tools/shared/.*"_rr,
                    "src/corelib/tools/qarraydata.cpp",
                    "src/corelib/tools/qbitarray.cpp",
                    "src/corelib/tools/qcommandlineoption.cpp",
                    "src/corelib/tools/qcommandlineparser.cpp",
                    "src/corelib/tools/qcryptographichash.cpp",
                    "src/corelib/tools/qhash.cpp",
                    "src/corelib/tools/qline.cpp",
                    //"src/corelib/tools/qlinkedlist.cpp",
                    "src/corelib/tools/qpoint.cpp",
                    "src/corelib/tools/qrect.cpp",
                    "src/corelib/tools/qringbuffer.cpp",
                    "src/corelib/tools/qsize.cpp",
                    "src/corelib/tools/qversionnumber.cpp",
                    "src/xml/.*\\.h"_rr,
                    "src/xml/dom/qdom.cpp"
                    ;

                bootstrap -=
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
                    "src/corelib"_id,
                    "src/xml"_id,
                    "src/xml/dom"_id
                    ;
                bootstrap.Public +=
                    "src/tools/shared"_id
                    ;
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

            //bootstrap += "QT_BUILD_BOOTSTRAP_LIB"_d;
            //bootstrap += "QT_BUILD_CORE_LIB"_d;
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
                //bootstrap += "src/corelib/time/qtimezoneprivate_win.cpp";
                bootstrap += "src/corelib/kernel/qfunctions_win.cpp";
                bootstrap.Private += "UNICODE"_d;
                bootstrap.Public += "WIN32"_d;
                bootstrap.Public += "Netapi32.lib"_slib, "Advapi32.lib"_slib, "Ole32.lib"_slib, "Shell32.lib"_slib, "uuid.lib"_slib;
            }
            if (bootstrap.getBuildSettings().TargetOS.isApple())
            {
                //bootstrap += "src/corelib/time/qtimezoneprivate_mac.mm";
            }

            bootstrap.Public += tinycbor;
            bootstrap.Public += "org.sw.demo.madler.zlib"_dep;
            bootstrap.Public += "org.sw.demo.pcre.pcre16-10"_dep;
            // natvis
            bootstrap.Public += "org.sw.demo.qtproject.qt.labs.vstools.natvis.qt6"_dep;

            qt_desc.print(bootstrap);

            bootstrap -=
                "src/corelib/io/qfilesystemengine_win.cpp",
                "src/corelib/io/qfilesystemiterator_win.cpp",
                "src/corelib/io/qfsfileengine_win.cpp",
                "src/corelib/kernel/qcoreapplication_win.cpp",
                "src/corelib/kernel/qwinreg.*"_rr,
                "src/corelib/plugin/qsystemlibrary.cpp",
                "src/corelib/global/qoperatingsystemversion_win.cpp",
                "src/corelib/io/qstandardpaths_win.cpp"
                ;
            bootstrap -=
                "src/corelib/kernel/qcore_unix.cpp",
                "src/corelib/io/qfilesystemengine_unix.cpp",
                "src/corelib/io/qfilesystemiterator_unix.cpp",
                "src/corelib/io/qfsfileengine_unix.cpp",
                "src/corelib/io/qstandardpaths_unix.cpp"
                ;
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
                    "src/corelib/kernel/qcore_unix.cpp",
                    "src/corelib/io/qfilesystemengine_unix.cpp",
                    "src/corelib/io/qfilesystemiterator_unix.cpp",
                    "src/corelib/io/qfsfileengine_unix.cpp",
                    "src/corelib/io/qstandardpaths_unix.cpp"
                    ;
            }

            bootstrap -=
                "src/corelib/kernel/qcoreapplication_mac.cpp",
                "src/corelib/global/qoperatingsystemversion_darwin.mm",
                "src/corelib/io/qstandardpaths_mac.mm"
                ;
            bootstrap -=
                "src/corelib/kernel/qcore_mac.mm",
                "src/corelib/kernel/qcore_foundation.mm",
                "src/corelib/kernel/qcoreapplication_mac.cpp",
                "src/corelib/global/qoperatingsystemversion_darwin.mm",
                "src/corelib/io/qstandardpaths_mac.mm",
                "src/corelib/io/qfilesystemengine_mac.mm"
                ;
            if (bootstrap.getBuildSettings().TargetOS.Type == OSType::Macos)
            {
                bootstrap +=
                    "src/corelib/kernel/qcore_mac.mm",
                    "src/corelib/kernel/qcore_foundation.mm",
                    "src/corelib/kernel/qcoreapplication_mac.cpp",
                    "src/corelib/global/qoperatingsystemversion_darwin.mm",
                    "src/corelib/io/qstandardpaths_mac.mm",
                    "src/corelib/io/qfilesystemengine_mac.mm"
                    ;
            }

            //auto c = bootstrap.addCommand(SW_VISIBLE_BUILTIN_FUNCTION(copy_file));
            //c << cmd::in("src/xml/sax/qxml_p.h") << cmd::out("private/qxml_p.h");
            syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, bootstrap, { "QtCore", "QtXml" });

            mkspecs(bootstrap);

            //bootstrap.patch("src/corelib/global/qconfig-bootstrapped.h", "#define QT_NO_DEPRECATED", "//#define  QT_NO_DEPRECATED");
            //bootstrap.patch("src/corelib/tools/qhashfunctions.h", "#if QT_DEPRECATED_SINCE(6,6)", "#if 1");

            /*bootstrap += "Q_CORE_EXPORT"_api;
            bootstrap += "Q_XML_EXPORT"_api;
            bootstrap.writeFileOnce("QtCore/qtcoreexports.h", R"(
                #pragma once
                #define QT_CORE_REMOVED_SINCE(x,y) (QT_VERSION_MAJOR == x && QT_VERSION_MINOR < y || QT_VERSION_MAJOR < x)
                #define QT_CORE_INLINE_SINCE(x,y) inline
                #define QT_CORE_INLINE_IMPL_SINCE(x,y) 1 inline
            )");*/
            exports_setup(bootstrap, "core", true);
            exports_setup(bootstrap, "xml");
            //bootstrap.writeFileOnce("QtXml/qtxmlexports.h");
            bootstrap.writeFileOnce("QtXml/qtxml-config.h");
        }

        // moc
        {
            common_setup(moc);
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
            common_setup(rcc);
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
            common_setup(uic);
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

            uic.patch("src/tools/uic/main.cpp", "qSetGlobalQHashSeed(0)", "QHashSeed::globalSeed()");
            uic.patch("src/tools/uic/main.cpp",
                "const QString version = QString::fromLatin1(qVersion());",
                "const QString version = QString::fromLatin1(\"" + uic.Variables["PACKAGE_VERSION"].toString() + "\");"
            );
        }

        // qlalr
        {
            common_setup(qlalr);
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
            common_setup(tracegen);
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

        // tracepointgen
        {
            common_setup(tracepointgen);
            tracepointgen +=
                "src/tools/tracepointgen/[^/]*\\.cpp"_rr,
                "src/tools/tracepointgen/[^/]*\\.[h]"_rr;
            tracepointgen.Public += "src/tools/tracepointgen"_id;
            if (tracepointgen.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                tracepointgen.Private += "UNICODE"_d;
                tracepointgen.Public += "WIN32"_d;
            }
            tracepointgen.Public += bootstrap;

            //tracegen.pushFrontToFileOnce("src/tools/tracegen/provider.h", "#include <qstringlist.h>");
        }

        // core
        {
            common_setup(core);
            core -=
                "mkspecs/.*\\.h"_rr;
            core +=
                "cmake/modulecppexports.h.in",
                "cmake/modulecppexports_p.h.in",
                "src/corelib/compat/removed_api.cpp",
                "src/3rdparty/easing/easing.cpp",
                //"src/3rdparty/freebsd/.*\\.c"_rr,
                "src/3rdparty/md[45]/.*"_rr,
                "src/3rdparty/rfc6234/.*\\.[hc]"_rr,
                "src/3rdparty/sha1/sha1.cpp",
                "src/3rdparty/sha3/.*"_rr,
                "src/3rdparty/tinycbor/src/.*"_rr
                ;
            core -=
                "src/3rdparty/easing/easing.cpp",
                //"src/3rdparty/freebsd/strtoll.c",
                //"src/3rdparty/freebsd/strtoull.c",
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

            mkspecs(core);

            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, core, { "QtCore" });

            SwapAndRestore sr(core.SourceDir);
            core.SourceDir /= "src/corelib";
            core +=
                "animation/.*"_rr,
                "global/.*"_rr,
                "io/.*"_rr,
                "itemmodels/.*"_rr,
                "kernel/.*"_rr,
                "mimetypes/.*"_rr,
                "plugin/.*"_rr,
                "serialization/.*"_rr,
                "text/.*"_rr,
                "thread/.*"_rr,
                "time/.*"_rr,
                "tools/.*"_rr
                ;

            core -=
                "platform/.*"_rr,
                "plugin/qcoffpeparser.cpp",
                "plugin/qmachparser.cpp",
                "global/qversiontagging.cpp",
                "io/qfilesystemwatcher_.*\\.cpp"_rr,
                "io/qsettings_wasm.cpp",
                "kernel/qwineventnotifier.cpp",
                "plugin/qsystemlibrary.cpp",
                "thread/qmutex_.*"_rr;

            core -= "global/qfloat16ta.*"_rr;

            core += "io/qfilesystemwatcher_polling.*"_rr;

            if (core.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                core += "platform/windows/.*"_rr;
                core += "plugin/qcoffpeparser.cpp";
                core += "plugin/qsystemlibrary.cpp";
                core += "kernel/qwineventnotifier.cpp";
                core += "NOMINMAX"_def;
            }
            else
            {
                core -= "io/qwindowspipereader_p.h";
                core -= "io/qwindowspipereader.cpp";
                core -= "io/qwindowspipewriter_p.h";
                core -= "io/qwindowspipewriter.cpp";
                core -= "kernel/qwinregistry.cpp";
            }

            core -= "kernel/qjniobject.cpp";
            core -= "kernel/qjnihelpers.cpp";
            core -= "kernel/qjnienvironment.cpp";

            core -= "kernel/qpoll.cpp";
            core -= "kernel/qcfsocketnotifier.cpp";
            core -= "io/forkfd_qt.cpp";

            // remove header only
            core -= "text/qchar.cpp";
            core -= "text/qstringmatcher.cpp";
            core -= "text/qunicodetables.cpp";

            // included via qcborvalue.cpp
            core -= "serialization/qcborarray.cpp";
            core -= "serialization/qcbormap.cpp";

            /*core -= "kernel/.*_glib.*"_rr;
            core -= "kernel/.*_wasm.*"_rr;
            if (core.getBuildSettings().TargetOS.Type == OSType::Windows)
                core -= "kernel/.*unix.*"_rr;
            else
                core -= "kernel/.*windows.*"_rr;*/

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

            core.Interface += sw::Shared, "QT_NO_VERSION_TAGGING"_d;
            core.Public += "QT_COMPILER_SUPPORTS_SIMD_ALWAYS"_d;
            core.Protected += "QT_USE_QSTRINGBUILDER"_d;
            if (core.getCompilerType() == CompilerType::MSVC)
            {
                core.Public += "_ENABLE_EXTENDED_ALIGNED_STORAGE"_d;
                core.Public += "QT_COMPILER_SUPPORTS_F16C"_d;
            }
            core -= forkfd;
            if (core.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                // we do not do -= these deps, because they are internal only
                core.Public += "com.Microsoft.Windows.SDK.winrt"_dep;
                core.Public += "com.Microsoft.Windows.SDK.cppwinrt"_dep;

                core.Protected += "UNICODE"_d;
                core.Public += "WIN32"_d;

                core.Public += "Mincore.lib"_slib;
                core.Public += "Mpr.lib"_slib;
                core.Public += "winmm.lib"_slib;
                core.Public += "User32.lib"_slib;
                core.Public += "uuid.lib"_slib;
                core.Public += "Netapi32.lib"_slib, "Advapi32.lib"_slib, "Ole32.lib"_slib, "Shell32.lib"_slib;
            }
            else {
                core += "QT_CONFIGURE_SETTINGS_PATH=\".\""_def;
                core += forkfd;
            }
            core.Public += sw::Shared, "QT_SHARED"_d;
            core.Public += sw::Static, "QT_STATIC"_d;

            core.Public += "org.sw.demo.google.double_conversion"_dep;
            core.Public += "org.sw.demo.madler.zlib"_dep;
            core.Public += "org.sw.demo.facebook.zstd.zstd"_dep;
            core.Public += "org.sw.demo.pcre.pcre16-10"_dep;
            core.Public += "org.sw.demo.behdad.harfbuzz"_dep;
            core.Public += "org.sw.demo.blake2.blake2"_dep;
            // natvis
            core.Public += "org.sw.demo.qtproject.qt.labs.vstools.natvis.qt6"_dep;

            if (core.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                qt_core_desc.config.public_.features.insert({ "alloca", false });
                qt_core_desc.config.public_.features.insert({ "alloca_h", false });
                qt_core_desc.config.public_.features.insert({ "getauxval", false });
                qt_core_desc.config.public_.features.insert({ "dlopen", false });
                qt_core_desc.config.public_.features.insert({ "fontconfig", false });
                qt_core_desc.config.public_.features.insert({ "cpp_winrt", true });
                qt_core_desc.config.public_.features.insert({ "backtrace", false });
                qt_core_desc.config.public_.features.insert({ "process", false });
                core -= "kernel/qpermissions.*"_rr;
                qt_core_desc.config.public_.features.insert({ "permissions", false });
                qt_core_desc.config.private_.features.insert({ "icu", false });
                qt_core_desc.config.private_.features.insert({ "icu", false });
            }
            else
            {
                qt_core_desc.config.public_.features.insert({ "cpp_winrt", false });
                qt_core_desc.config.public_.features.insert({ "alloca", true });
                qt_core_desc.config.public_.features.insert({ "alloca_h", true });
                qt_core_desc.config.public_.features.insert({ "linkat", true });
                qt_core_desc.config.public_.features.insert({ "renameat2", true });
                qt_core_desc.config.public_.features.insert({ "statx", true });
                qt_core_desc.config.public_.features.insert({ "futimens", true });
                qt_core_desc.config.public_.features.insert({ "dlopen", true });
                qt_core_desc.config.public_.features.insert({ "forkfd_pidfd", true });
                qt_core_desc.config.public_.features.insert({ "fontconfig", true });
                qt_core_desc.config.public_.features.insert({ "backtrace", false });
                qt_core_desc.config.public_.features.insert({ "permissions", true });
                qt_core_desc.config.private_.features.insert({ "icu", false });
            }
            if (core.getBuildSettings().TargetOS.Type == OSType::Linux) {
                qt_core_desc.config.public_.features.insert({ "getauxval", true });
                qt_core_desc.config.public_.features.insert({ "glibc", true });
                qt_core_desc.config.public_.features.insert({ "process", true });
                qt_core_desc.config.public_.features.insert({ "poll_poll", false });
                qt_core_desc.config.public_.features.insert({ "poll_pollts", false });
                qt_core_desc.config.public_.features.insert({ "poll_ppoll", true });
                qt_core_desc.config.public_.features.insert({ "poll_select", true });
            }
            if (core.getBuildSettings().TargetOS.Type == OSType::Macos) {
                //core += "platform/darwin/.*"_rr; > qt6. 4. 1
                qt_core_desc.config.public_.features.insert({ "getauxval", false });
                qt_core_desc.config.public_.features.insert({ "forkfd_pidfd", false });
                qt_core_desc.config.public_.features.insert({ "appstore_compliant", false });
                qt_core_desc.config.public_.features.insert({ "poll_poll", true });
                qt_core_desc.config.public_.features.insert({ "poll_ppoll", false });
                qt_core_desc.config.public_.features.insert({ "poll_select", false });
                qt_core_desc.config.public_.features.insert({ "poll_pollts", false });
                qt_core_desc.config.public_.features.insert({ "process", false });
                qt_core_desc.config.private_.features.insert({ "icu", true });
            }

            if (core.getBuildSettings().TargetOS.Arch == ArchType::aarch64) {
                qt_core_desc.config.public_.features.insert({ "sse2", false });
            } else {
                qt_core_desc.config.public_.features.insert({ "sse2", true });
                qt_core_desc.config.public_.definitions.insert({ "QT_COMPILER_SUPPORTS_SSE2", "1" });
                qt_core_desc.config.public_.definitions.insert({ "QT_COMPILER_SUPPORTS_SSE3", "1" });
                qt_core_desc.config.public_.definitions.insert({ "QT_COMPILER_SUPPORTS_SSSE3", "1" });
                qt_core_desc.config.public_.definitions.insert({ "QT_COMPILER_SUPPORTS_SSE4_1", "1" });
                qt_core_desc.config.public_.definitions.insert({ "QT_COMPILER_SUPPORTS_SSE4_2", "1" });
                qt_core_desc.config.public_.definitions.insert({ "QT_COMPILER_SUPPORTS_AVX", "1" });
                // github actions ci does not support avx2?
                if (core.getBuildSettings().TargetOS.Type != OSType::Macos) {
                    qt_core_desc.config.public_.definitions.insert({ "QT_COMPILER_SUPPORTS_AVX2", "1" });
                }
            }
            qt_core_desc.config.public_.features.insert({"alloca_malloc_h", false});

            qt_desc.print(core);
            qt_core_desc.print(core);

            // see src/corelib/global/qconfig.cpp.in
            core.writeFileOnce(core.BinaryDir / "qconfig.cpp", R"xxx(
#include "private/qoffsetstringarray_p.h"

/* Installation Info */
static const char qt_configure_prefix_path_str  [12+256] = "qt_prfxpath=.";

static constexpr auto qt_configure_strs = QT_PREPEND_NAMESPACE(qOffsetStringArray)(
    R"qconfig(doc)qconfig",
    R"qconfig(include)qconfig",
    R"qconfig(bin)qconfig", // was: lib, changed for linux
    R"qconfig(bin)qconfig",
    R"qconfig(bin)qconfig",
    R"qconfig(plugins)qconfig",
    R"qconfig(qml)qconfig",
    R"qconfig(.)qconfig",
    R"qconfig(.)qconfig",
    R"qconfig(translations)qconfig",
    R"qconfig(examples)qconfig",
    R"qconfig(tests)qconfig"
);

// comment?
#define QT_CONFIGURE_SETTINGS_PATH "."
//#define QT_CONFIGURE_LIBLOCATION_TO_PREFIX_PATH "@QT_CONFIGURE_LIBLOCATION_TO_PREFIX_PATH@"_L1
#define QT_CONFIGURE_PREFIX_PATH qt_configure_prefix_path_str + 12
    )xxx");
            core += core.BinaryDir / "qconfig.cpp";

            platform_files(core);

            core -=
                "thread/qmutex_mac.cpp",
                "thread/qmutex_unix.cpp"
                ;

            //if (core.getBuildSettings().TargetOS.Arch == ArchType::aarch64) {
                core -= "global/qsimd_x86.cpp";
            //}
            if (core.getBuildSettings().TargetOS.Type == OSType::Macos)
            {
                core.Public += "kernel/qmetatype.cpp"; // not only mac?
                core += "plugin/qmachparser.cpp";

                core -= "io/qstorageinfo_unix.cpp";
                core -= "kernel/qelapsedtimer_unix.cpp";
                core -= "text/qcollator_posix.cpp";
                core -= "io/qstandardpaths_unix.cpp";
                core -= "text/qlocale_unix.cpp";
            }
            set_apple_flags(core);

            auto mocs = automoc(moc, core);
            SW_QT_ADD_MOC_DEPS(core);
            //::rcc(rcc, core, core.SourceDir / "mimetypes/mimetypes.qrc");

            // after moc
            write_tracepoints(core,
                "kernel/qcoreapplication.cpp",
                "kernel/qcoreevent.cpp",
                "kernel/qobject.cpp",
                "plugin/qfactoryloader.cpp",
                "plugin/qlibrary.cpp",
                "global/qlogging.cpp"
            );

            {
                auto c = core.addCommand(SW_VISIBLE_FUNCTION(mime_preprocess_file));
                c << cmd::in("mimetypes/mime/packages/freedesktop.org.xml");
                c << cmd::out("qmimeprovider_database.cpp", cmd::Skip);
            }

            // for declarative; for some reason not generated
            core.writeFileOnce("include/QtCore/QVector", "#include <QtCore/qvector.h>");
            core.patch("global/qsimd.cpp", "#  include \"..", "//#   include \"..");
        }

        // dbus
        {
            common_setup(dbus);

            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, dbus, { "QtDBus" });

            SwapAndRestore sr(xml.SourceDir);
            dbus.SourceDir /= "src/dbus";
            dbus += ".*"_r;
            dbus -= "qdbusmarshaller.cpp";
            dbus -= "qdbusdemarshaller.cpp";

            dbus += "DBUS_API_SUBJECT_TO_CHANGE"_def;
            dbus += "QDBUS_NO_SPECIALTYPES"_def;

            dbus.Public += core;

            auto mocs = automoc(moc, dbus);
            SW_QT_ADD_MOC_DEPS(dbus);
        }

        // gui
        {
            common_setup(gui);
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, gui, { "QtGui" });
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

            gui -= "doc/.*"_rr;
            gui -= "text/.*"_rr;
            gui += "text/.*"_r;
            gui -= "platform/.*"_rr;
            gui -= "text/qcssscanner.cpp";
            gui -= "painting/webgradients.cpp";

            gui -= "opengl/platform/egl/.*"_rr;
            gui -= "opengl/platform/unix/.*"_rr;
            gui -= "accessible/.*"_rr;
            gui += "accessible/.*"_r;

            gui -= "rhi/qrhivulkan.*"_rr;
            gui -= "rhi/qrhimetal.*"_rr;
            gui -= "rhi/qrhid3d11.*"_rr;
            if (gui.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                gui += "accessible/windows/.*"_rr;
                gui += "text/windows/.*"_rr;
                gui += "platform/windows/.*"_rr;
                gui += "rhi/qrhid3d11.*"_rr;
                gui += "d3d11.lib"_slib;
                gui += "dxgi.lib"_slib;
                gui += "dxguid.lib"_slib;
                gui += "Dwrite.lib"_slib;
                gui += "Dcomp.lib"_slib;
            }
            else if (gui.getBuildSettings().TargetOS.Type == OSType::Macos)
            {
                gui += "platform/darwin/.*"_rr;
                gui += "rhi/qrhimetal.*"_rr;
                gui += "platform/unix/qunixeventdispatcher.cpp";
            }
            else
            {
                //gui += "accessible/linux/.*"_rr;
                gui += "platform/unix/.*"_rr;
                gui += "opengl/platform/.*"_rr;
                gui += "text/unix/.*"_rr;
                //gui -= "platform/unix/qxkbcommon_3rdparty.cpp";
                //gui -= "platform/unix/qxkbcommon.cpp";
                gui.Public += "xkbcommon"_slib;
                gui.CompileOptions.push_back("-msse4.1");
                gui.CompileOptions.push_back("-mavx");
                gui.CompileOptions.push_back("-mavx2");
                //gui.CompileOptions.push_back("-march=haswell");
                gui.CompileOptions.push_back("-march=native");
            }
            if (gui.getCompilerType() == CompilerType::ClangCl) {
                gui.CompileOptions.push_back("-msse4.1");
                gui.CompileOptions.push_back("-mavx");
                gui.CompileOptions.push_back("-mavx2");
                gui.CompileOptions.push_back("-mf16c");
            }

            //gui.Public += "src"_id;

            gui.Public += "org.sw.demo.glennrp.png"_dep;
            gui += "org.sw.demo.mity.md4c"_dep;
            gui.Public += core;

            gui.Public -= dbus;
            if (gui.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                qt_gui_desc.config.public_.features.insert({ "egl", false });
                qt_gui_desc.config.public_.features.insert({ "egl_x11", false });
                qt_gui_desc.config.public_.features.insert({ "xcb", false });
                qt_gui_desc.config.public_.features.insert({ "xcb_glx_plugin", false });
                qt_gui_desc.config.public_.definitions.insert({ "QT_QPA_DEFAULT_PLATFORM_NAME", "\"windows\"" });
            }
            else
            {
                gui.Public += dbus;
            }
            if (gui.getBuildSettings().TargetOS.Type == OSType::Linux)
            {
                gui.Public += "GL"_slib;
                gui.Public += "EGL"_slib;
                gui.Public += "X11"_slib;

                qt_gui_desc.config.public_.features.insert({ "egl", true });
                qt_gui_desc.config.public_.features.insert({ "egl_x11", true });
                qt_gui_desc.config.public_.features.insert({ "egl_extension_platform_wayland", true });
                // embedded
                qt_gui_desc.config.public_.features.insert({ "eglfs", false });
                qt_gui_desc.config.public_.features.insert({ "eglfs_brcm", false });
                qt_gui_desc.config.public_.features.insert({ "eglfs_egldevice", false });
                qt_gui_desc.config.public_.features.insert({ "eglfs_gbm", false });
                qt_gui_desc.config.public_.features.insert({ "eglfs_mali", false });
                qt_gui_desc.config.public_.features.insert({ "eglfs_viv", false });
                qt_gui_desc.config.public_.features.insert({ "eglfs_viv_wl", false });

                qt_gui_desc.config.public_.features.insert({ "xcb", true });
                qt_gui_desc.config.public_.features.insert({ "xcb_xlib", true });
                qt_gui_desc.config.public_.features.insert({ "xcb_sm", false });
                qt_gui_desc.config.public_.features.insert({ "xcb_glx", true });
                qt_gui_desc.config.public_.features.insert({ "xcb_glx_plugin", true });
                qt_gui_desc.config.public_.features.insert({ "xcb_native_painting", false });
                qt_gui_desc.config.public_.features.insert({ "xkbcommon", true });
                qt_gui_desc.config.public_.features.insert({ "xkbcommon_evdev", false });
                qt_gui_desc.config.public_.definitions.insert({ "QT_QPA_DEFAULT_PLATFORM_NAME", "\"wayland\"" }); // ? other? xcb?
            }
            else if (gui.getBuildSettings().TargetOS.isApple())
            {
                qt_gui_desc.config.public_.features.insert({ "egl", false });
                qt_gui_desc.config.public_.features.insert({ "egl_x11", false });
                qt_gui_desc.config.public_.features.insert({ "egl_extension_platform_wayland", false });
                // embedded
                qt_gui_desc.config.public_.features.insert({ "eglfs", false });
                qt_gui_desc.config.public_.features.insert({ "eglfs_brcm", false });
                qt_gui_desc.config.public_.features.insert({ "eglfs_egldevice", false });
                qt_gui_desc.config.public_.features.insert({ "eglfs_gbm", false });
                qt_gui_desc.config.public_.features.insert({ "eglfs_mali", false });
                qt_gui_desc.config.public_.features.insert({ "eglfs_viv", false });
                qt_gui_desc.config.public_.features.insert({ "eglfs_viv_wl", false });

                qt_gui_desc.config.public_.features.insert({ "xcb", false });
                qt_gui_desc.config.public_.features.insert({ "xcb_glx_plugin", false });
                qt_gui_desc.config.public_.features.insert({ "xkbcommon", false });
                qt_gui_desc.config.public_.features.insert({ "xkbcommon_evdev", false });
                qt_gui_desc.config.public_.definitions.insert({ "QT_QPA_DEFAULT_PLATFORM_NAME", "\"cocoa\"" });
            }
            set_apple_flags(gui);
            qt_gui_desc.print(gui);

            gui.Protected += "Carbon"_framework;
            gui.Protected += "Metal"_framework;

            platform_files(gui);
            gui += "text/freetype/.*"_rr;

            //gui.replaceInFileOnce("text/qharfbuzzng_p.h", "#include <harfbuzz/hb.h>", "#include <hb.h>");
            //gui.replaceInFileOnce("text/qfontengine.cpp", "#  include <harfbuzz/hb-ot.h>", "#include <hb-ot.h>");
            gui.replaceInFileOnce("text/qtextmarkdownimporter.cpp", "#include \"../../3rdparty/md4c/md4c.h\"", "#include <md4c/md4c.h>");

            auto mocs = automoc(moc, gui);
            SW_QT_ADD_MOC_DEPS(gui);
            //::rcc(rcc, gui, Files{ gui.SourceDir / "painting/qpdf.qrc", });
            RccData d;
            d.files["painting/qpdfa_metadata.xml"];// = "painting/qpdfa_metadata.xml";
            d.files["../3rdparty/icc/sRGB2014.icc"];// = "icc/sRGB2014.icc";
            d.name = "qpdf";
            d.prefix = "/qpdf/";
            d.base = "painting";
            ::rcc(rcc, gui, d);

            // after moc
            write_tracepoints(gui,
                "image/qimage.cpp",
                "image/qimagereader.cpp",
                "image/qpixmap.cpp",
                "kernel/qguiapplication.cpp",
                "text/qfontdatabase.cpp"
            );

            /*RccData wg;
            wg.files[gui.SourceDir / "painting/webgradients.binaryjson"];
            wg.name = "webgradients";
            wg.base = "painting";
            wg.prefix = "qgradient";
            ::rcc(rcc, gui, wg);*/
        }

        // widgets
        {
            common_setup(widgets);

            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, widgets, { "QtWidgets" });

            SwapAndRestore sr(widgets.SourceDir);
            widgets.SourceDir /= "src/widgets";
            widgets +=
                "compat/removed_api.cpp",
                "accessible/.*"_rr,
                "dialogs/.*"_rr,
                "effects/.*"_rr,
                "graphicsview/.*"_rr,
                "itemviews/.*"_rr,
                "kernel/.*"_rr,
                "styles/.*"_rr,
                "util/.*"_rr,
                "widgets/.*"_rr;

            if (widgets.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                widgets.Public += "dwmapi.lib"_slib;
                widgets.Public += "uxtheme.lib"_slib;
            }

            widgets.Public += gui;
            widgets += "dialogs"_id;

            if (widgets.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                // WIN32 is for moc, it does not understand and use _WIN32
                qt_widgets_desc.config.public_.features.insert({"style_stylesheet", true});
                qt_widgets_desc.config.public_.features.insert({"style_windows", true, {"_WIN32", "WIN32"}});
                qt_widgets_desc.config.public_.features.insert({"style_windowsvista", true, {"_WIN32", "WIN32"}});
                qt_widgets_desc.config.public_.features.insert({"style_windowsxp", true, {"_WIN32", "WIN32"}});
            }
            else
            {
                //widgets += "QT_NO_STYLE_STYLESHEET"_def;
                qt_widgets_desc.config.public_.features.insert({"style_stylesheet", true});
                qt_widgets_desc.config.public_.features.insert({"style_windows", true});
                qt_widgets_desc.config.public_.features.insert({"style_windowsvista", false});
                qt_widgets_desc.config.public_.features.insert({"style_windowsxp", false});
            }
            qt_widgets_desc.print(widgets);

            platform_files(widgets);
            if (widgets.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                widgets += "util/qsystemtrayicon_qpa.cpp";
            }

            auto mocs = automoc(moc, widgets);
            SW_QT_ADD_MOC_DEPS(widgets);
            //::rcc(rcc, widgets, Files{ widgets.SourceDir / "styles/qstyle.qrc", widgets.SourceDir / "dialogs/qmessagebox.qrc" });
            ::uic(uic, widgets, widgets.SourceDir / "dialogs/qfiledialog.ui");

            {
                RccData d;
                for (auto &&f : {
                                    "styles/images/cdr-128.png"                                                          ,
                                    "styles/images/cdr-16.png"                                                           ,
                                    "styles/images/cdr-32.png"                                                           ,
                                    "styles/images/cleartext-16.png"                                                     ,
                                    "styles/images/cleartext-32.png"                                                     ,
                                    "styles/images/closedock-10.png"                                                     ,
                                    "styles/images/closedock-16.png"                                                     ,
                                    "styles/images/closedock-20.png"                                                     ,
                                    "styles/images/closedock-32.png"                                                     ,
                                    "styles/images/closedock-48.png"                                                     ,
                                    "styles/images/closedock-64.png"                                                     ,
                                    "styles/images/computer-16.png"                                                      ,
                                    "styles/images/computer-32.png"                                                      ,
                                    "styles/images/desktop-16.png"                                                       ,
                                    "styles/images/desktop-32.png"                                                       ,
                                    "styles/images/dirclosed-128.png"                                                    ,
                                    "styles/images/dirclosed-16.png"                                                     ,
                                    "styles/images/dirclosed-32.png"                                                     ,
                                    "styles/images/dirlink-128.png"                                                      ,
                                    "styles/images/dirlink-16.png"                                                       ,
                                    "styles/images/dirlink-32.png"                                                       ,
                                    "styles/images/diropen-128.png"                                                      ,
                                    "styles/images/diropen-16.png"                                                       ,
                                    "styles/images/diropen-32.png"                                                       ,
                                    "styles/images/down-128.png"                                                         ,
                                    "styles/images/down-16.png"                                                          ,
                                    "styles/images/down-32.png"                                                          ,
                                    "styles/images/dvd-128.png"                                                          ,
                                    "styles/images/dvd-16.png"                                                           ,
                                    "styles/images/dvd-32.png"                                                           ,
                                    "styles/images/file-128.png"                                                         ,
                                    "styles/images/file-16.png"                                                          ,
                                    "styles/images/file-32.png"                                                          ,
                                    "styles/images/filecontents-128.png"                                                 ,
                                    "styles/images/filecontents-16.png"                                                  ,
                                    "styles/images/filecontents-32.png"                                                  ,
                                    "styles/images/fileinfo-128.png"                                                     ,
                                    "styles/images/fileinfo-16.png"                                                      ,
                                    "styles/images/fileinfo-32.png"                                                      ,
                                    "styles/images/filelink-128.png"                                                     ,
                                    "styles/images/filelink-16.png"                                                      ,
                                    "styles/images/filelink-32.png"                                                      ,
                                    "styles/images/floppy-128.png"                                                       ,
                                    "styles/images/floppy-16.png"                                                        ,
                                    "styles/images/floppy-32.png"                                                        ,
                                    "styles/images/fontbitmap-16.png"                                                    ,
                                    "styles/images/fonttruetype-16.png"                                                  ,
                                    "styles/images/fusion_groupbox.png"                                                  ,
                                    "styles/images/harddrive-128.png"                                                    ,
                                    "styles/images/harddrive-16.png"                                                     ,
                                    "styles/images/harddrive-32.png"                                                     ,
                                    "styles/images/left-128.png"                                                         ,
                                    "styles/images/left-16.png"                                                          ,
                                    "styles/images/left-32.png"                                                          ,
                                    "styles/images/media-pause-16.png"                                                   ,
                                    "styles/images/media-pause-32.png"                                                   ,
                                    "styles/images/media-play-16.png"                                                    ,
                                    "styles/images/media-play-32.png"                                                    ,
                                    "styles/images/media-seek-backward-16.png"                                           ,
                                    "styles/images/media-seek-backward-32.png"                                           ,
                                    "styles/images/media-seek-forward-16.png"                                            ,
                                    "styles/images/media-seek-forward-32.png"                                            ,
                                    "styles/images/media-skip-backward-16.png"                                           ,
                                    "styles/images/media-skip-backward-32.png"                                           ,
                                    "styles/images/media-skip-forward-16.png"                                            ,
                                    "styles/images/media-skip-forward-32.png"                                            ,
                                    "styles/images/media-stop-16.png"                                                    ,
                                    "styles/images/media-stop-32.png"                                                    ,
                                    "styles/images/media-volume-16.png"                                                  ,
                                    "styles/images/media-volume-muted-16.png"                                            ,
                                    "styles/images/networkdrive-128.png"                                                 ,
                                    "styles/images/networkdrive-16.png"                                                  ,
                                    "styles/images/networkdrive-32.png"                                                  ,
                                    "styles/images/newdirectory-128.png"                                                 ,
                                    "styles/images/newdirectory-16.png"                                                  ,
                                    "styles/images/newdirectory-32.png"                                                  ,
                                    "styles/images/normalizedockup-10.png"                                               ,
                                    "styles/images/normalizedockup-16.png"                                               ,
                                    "styles/images/normalizedockup-20.png"                                               ,
                                    "styles/images/normalizedockup-32.png"                                               ,
                                    "styles/images/normalizedockup-48.png"                                               ,
                                    "styles/images/normalizedockup-64.png"                                               ,
                                    "styles/images/parentdir-128.png"                                                    ,
                                    "styles/images/parentdir-16.png"                                                     ,
                                    "styles/images/parentdir-32.png"                                                     ,
                                    "styles/images/refresh-24.png"                                                       ,
                                    "styles/images/refresh-32.png"                                                       ,
                                    "styles/images/right-128.png"                                                        ,
                                    "styles/images/right-16.png"                                                         ,
                                    "styles/images/right-32.png"                                                         ,
                                    "styles/images/standardbutton-apply-128.png"                                         ,
                                    "styles/images/standardbutton-apply-16.png"                                          ,
                                    "styles/images/standardbutton-apply-32.png"                                          ,
                                    "styles/images/standardbutton-cancel-128.png"                                        ,
                                    "styles/images/standardbutton-cancel-16.png"                                         ,
                                    "styles/images/standardbutton-cancel-32.png"                                         ,
                                    "styles/images/standardbutton-clear-128.png"                                         ,
                                    "styles/images/standardbutton-clear-16.png"                                          ,
                                    "styles/images/standardbutton-clear-32.png"                                          ,
                                    "styles/images/standardbutton-close-128.png"                                         ,
                                    "styles/images/standardbutton-close-16.png"                                          ,
                                    "styles/images/standardbutton-close-32.png"                                          ,
                                    "styles/images/standardbutton-closetab-32.png"                                       ,
                                    "styles/images/standardbutton-closetab-16.png"                                       ,
                                    "styles/images/standardbutton-closetab-down-32.png"                                  ,
                                    "styles/images/standardbutton-closetab-down-16.png"                                  ,
                                    "styles/images/standardbutton-closetab-hover-32.png"                                 ,
                                    "styles/images/standardbutton-closetab-hover-16.png"                                 ,
                                    "styles/images/standardbutton-delete-128.png"                                        ,
                                    "styles/images/standardbutton-delete-16.png"                                         ,
                                    "styles/images/standardbutton-delete-32.png"                                         ,
                                    "styles/images/standardbutton-help-128.png"                                          ,
                                    "styles/images/standardbutton-help-16.png"                                           ,
                                    "styles/images/standardbutton-help-32.png"                                           ,
                                    "styles/images/standardbutton-no-128.png"                                            ,
                                    "styles/images/standardbutton-no-16.png"                                             ,
                                    "styles/images/standardbutton-no-32.png"                                             ,
                                    "styles/images/standardbutton-ok-128.png"                                            ,
                                    "styles/images/standardbutton-ok-16.png"                                             ,
                                    "styles/images/standardbutton-ok-32.png"                                             ,
                                    "styles/images/standardbutton-open-128.png"                                          ,
                                    "styles/images/standardbutton-open-16.png"                                           ,
                                    "styles/images/standardbutton-open-32.png"                                           ,
                                    "styles/images/standardbutton-save-128.png"                                          ,
                                    "styles/images/standardbutton-save-16.png"                                           ,
                                    "styles/images/standardbutton-save-32.png"                                           ,
                                    "styles/images/standardbutton-yes-128.png"                                           ,
                                    "styles/images/standardbutton-yes-16.png"                                            ,
                                    "styles/images/standardbutton-yes-32.png"                                            ,
                                    "styles/images/stop-24.png"                                                          ,
                                    "styles/images/stop-32.png"                                                          ,
                                    "styles/images/titlebar-contexthelp-16.png"                                          ,
                                    "styles/images/titlebar-contexthelp-32.png"                                          ,
                                    "styles/images/titlebar-contexthelp-48.png"                                          ,
                                    "styles/images/titlebar-max-16.png"                                                  ,
                                    "styles/images/titlebar-max-32.png"                                                  ,
                                    "styles/images/titlebar-max-48.png"                                                  ,
                                    "styles/images/titlebar-min-16.png"                                                  ,
                                    "styles/images/titlebar-min-32.png"                                                  ,
                                    "styles/images/titlebar-min-48.png"                                                  ,
                                    "styles/images/titlebar-shade-16.png"                                                ,
                                    "styles/images/titlebar-shade-32.png"                                                ,
                                    "styles/images/titlebar-shade-48.png"                                                ,
                                    "styles/images/titlebar-unshade-16.png"                                              ,
                                    "styles/images/titlebar-unshade-32.png"                                              ,
                                    "styles/images/titlebar-unshade-48.png"                                              ,
                                    "styles/images/toolbar-ext-h-16.png"                                                 ,
                                    "styles/images/toolbar-ext-h-32.png"                                                 ,
                                    "styles/images/toolbar-ext-h-8.png"                                                  ,
                                    "styles/images/toolbar-ext-h-rtl-16.png"                                             ,
                                    "styles/images/toolbar-ext-h-rtl-32.png"                                             ,
                                    "styles/images/toolbar-ext-h-rtl-8.png"                                              ,
                                    "styles/images/toolbar-ext-v-10.png"                                                 ,
                                    "styles/images/toolbar-ext-v-20.png"                                                 ,
                                    "styles/images/toolbar-ext-v-5.png"                                                  ,
                                    "styles/images/trash-128.png"                                                        ,
                                    "styles/images/trash-16.png"                                                         ,
                                    "styles/images/trash-32.png"                                                         ,
                                    "styles/images/up-128.png"                                                           ,
                                    "styles/images/up-16.png"                                                            ,
                                    "styles/images/up-32.png"                                                            ,
                                    "styles/images/viewdetailed-128.png"                                                 ,
                                    "styles/images/viewdetailed-16.png"                                                  ,
                                    "styles/images/viewdetailed-32.png"                                                  ,
                                    "styles/images/viewlist-128.png"                                                     ,
                                    "styles/images/viewlist-16.png"                                                      ,
                                    "styles/images/viewlist-32.png"                                                      ,
                                }) {
                    d.files[f];
                }
                d.name = "qstyle";
                d.prefix = "/qt-project.org/styles/commonstyle";
                d.base = "styles";
                ::rcc(rcc, widgets, d);
            }
            {
                RccData d;
                for (auto &&f : {
                                    "styles/images/closedock-down-macstyle-10.png",
                                    "styles/images/closedock-down-macstyle-16.png",
                                    "styles/images/closedock-down-macstyle-20.png",
                                    "styles/images/closedock-down-macstyle-32.png",
                                    "styles/images/closedock-down-macstyle-48.png",
                                    "styles/images/closedock-down-macstyle-64.png",
                                    "styles/images/closedock-macstyle-10.png"     ,
                                    "styles/images/closedock-macstyle-16.png"     ,
                                    "styles/images/closedock-macstyle-20.png"     ,
                                    "styles/images/closedock-macstyle-32.png"     ,
                                    "styles/images/closedock-macstyle-48.png"     ,
                                    "styles/images/closedock-macstyle-64.png"     ,
                                    "styles/images/dockdock-down-macstyle-10.png" ,
                                    "styles/images/dockdock-down-macstyle-16.png" ,
                                    "styles/images/dockdock-down-macstyle-20.png" ,
                                    "styles/images/dockdock-down-macstyle-32.png" ,
                                    "styles/images/dockdock-down-macstyle-48.png" ,
                                    "styles/images/dockdock-down-macstyle-64.png" ,
                                    "styles/images/dockdock-macstyle-10.png"      ,
                                    "styles/images/dockdock-macstyle-16.png"      ,
                                    "styles/images/dockdock-macstyle-20.png"      ,
                                    "styles/images/dockdock-macstyle-32.png"      ,
                                    "styles/images/dockdock-macstyle-48.png"      ,
                                    "styles/images/dockdock-macstyle-64.png"      ,
                                    "styles/images/toolbar-ext-macstyle.png"      ,
                                    "styles/images/toolbar-ext-macstyle@2x.png"   ,
                                }) {
                    d.files[f];
                }
                d.name = "qstyle1";
                d.prefix = "/qt-project.org/styles/macstyle";
                d.base = "styles";
                ::rcc(rcc, widgets, d);
            }
            {
                RccData d;
                for (auto &&f : {
                                    "dialogs/images/qtlogo-64.png"   ,
                                }) {
                    d.files[f];
                }
                d.name = "qmessagebox";
                d.prefix = "/qt-project.org/qmessagebox";
                d.base = "dialogs";
                ::rcc(rcc, widgets, d);
            }

            // after moc
            write_tracepoints(widgets, "kernel/qapplication.cpp");

            widgets.patch("kernel/qwidgetsvariant.cpp", "qRegisterMetaType<QWidget*>();", "//qRegisterMetaType<QWidget*>();");
        }

        // network
        {
            common_setup(network);

            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, network, { "QtNetwork" });

            network -= "src/3rdparty/libpsl/psl_data.cpp";
            network += "src/3rdparty/libpsl"_idir;

            SwapAndRestore sr(network.SourceDir);
            network.SourceDir /= "src/network";
            network +=
                "access/.*"_rr,
                "kernel/.*"_rr,
                "socket/.*"_rr,
                "ssl/.*"_rr;

            network -=
                "socket/qlocalserver_tcp.*"_rr,
                "socket/qlocalsocket_tcp.*"_rr,
                "socket/qsctpserver.*"_rr,
                "socket/qsctpsocket.*"_rr;
            network -= "access/qnetworkreplywasmimpl.*"_rr;

            network -= ".*_libproxy.*"_rr;

            network.Private +=
                "."_id,
                "kernel"_id;

            network += "QT_USE_QSTRINGBUILDER"_d;
            if (network.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                network += "com.Microsoft.Windows.SDK.winrt"_dep;

                network.Public += "Crypt32.lib"_slib;
                network.Public += "Dnsapi.lib"_slib;
                network.Public += "Iphlpapi.lib"_slib;
            }
            else if (network.getBuildSettings().TargetOS.Type == OSType::Linux)
            {
                // add to sw
                network += "kernel/qnetworkproxy_libproxy.cpp";
                network += "proxy"_slib;
            }

            network += psl;
            network.Public += core;
            network.Public += "org.sw.demo.openssl.ssl"_dep;
            network.Public += "org.sw.demo.google.brotli"_dep;

            if (network.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                qt_network_desc.config.public_.features.insert({ "gssapi", false });
                qt_network_desc.config.private_.features.insert({ "sspi", true });
            }
            else if (network.getBuildSettings().TargetOS.Type == OSType::Linux)
            {
                qt_network_desc.config.public_.features.insert({ "gssapi", false });
                qt_network_desc.config.public_.features.insert({ "linux_netlink", true });
                qt_network_desc.config.public_.features.insert({ "ifr_index", false });
                qt_network_desc.config.private_.features.insert({ "sspi", false });
            }
            else if (network.getBuildSettings().TargetOS.isApple())
            {
                qt_network_desc.config.public_.features.insert({ "gssapi", false });
                qt_network_desc.config.public_.features.insert({ "linux_netlink", false });
                qt_network_desc.config.public_.features.insert({ "ifr_index", false });
                qt_network_desc.config.private_.features.insert({ "sspi", false });
            }
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

            if (network.getBuildSettings().TargetOS.isApple())
            {
                network.Protected += "SystemConfiguration"_framework;
            }

            auto mocs = automoc(moc, network);
            SW_QT_ADD_MOC_DEPS(network);
        }

        // opengl
        {
            common_setup(opengl);
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, opengl, { "QtOpenGL" });

            SwapAndRestore sr(opengl.SourceDir);
            opengl.SourceDir /= "src/opengl";
            opengl += ".*"_r;
            opengl -=
                "qopenglfunctions_es2.cpp",
                "qvkconvenience.cpp"
            ;

            auto mocs = automoc(moc, opengl);
            SW_QT_ADD_MOC_DEPS(opengl);

            // after moc
            write_tracepoints(opengl,
                "qopenglframebufferobject.cpp",
                "qopenglpaintengine.cpp",
                "qopengltexturecache.cpp"
            );

            opengl.Public += gui;
            opengl += "org.sw.demo.find.opengl-master"_dep;

            qt_opengl_desc.print(opengl);
        }

        // xml
        {
            common_setup(xml);

            syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, xml, { "QtXml" });

            SwapAndRestore sr(xml.SourceDir);
            xml.SourceDir /= "src/xml";
            xml +=
                "dom/qdom.*"_rr,
                "qtxmlglobal.h"
                ;
            xml.Public += core;
            qt_xml_desc.print(xml);
        }

        // sql
        auto &sql = base.addTarget<LibraryTarget>("sql");
        {
            common_setup(sql);

            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, sql, { "QtSql" });
            SwapAndRestore sr(sql.SourceDir);
            sql.SourceDir /= "src/sql";
            sql +=
                "compat/removed_api.cpp",
                "kernel/.*"_rr,
                "models/.*"_rr
                ;
            sql.Public += core;
            qt_sql_desc.print(sql);
            auto mocs = automoc(moc, sql);
            SW_QT_ADD_MOC_DEPS(sql);
        }

        auto &platformsupport = base.addDirectory("platformsupport");

        //auto &accessibility = platformsupport.addTarget<StaticLibraryTarget>("accessibility");
        //auto &eventdispatchers = platformsupport.addTarget<StaticLibraryTarget>("eventdispatchers");
        //auto &themes = platformsupport.addTarget<StaticLibraryTarget>("themes");
        //auto &windowsuiautomation = platformsupport.addTarget<StaticLibraryTarget>("windowsuiautomation");

        //auto &fontdatabases = platformsupport.addDirectory("fontdatabases");
        //auto &fontdatabases_windows = fontdatabases.addTarget<StaticLibraryTarget>("windows");

        // platformsupport
        {
            // accessibility
            /*{
                accessibility += "src/platformsupport/accessibility/.*"_rr;
                accessibility.Public += gui;

                String module = "QtAccessibilitySupport";
                auto &t = accessibility;

                syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, accessibility, { module });

                t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                #ifdef __cplusplus
                #include <QtCore/QtCore>
                #include <QtGui/QtGui>
                #endif
    )xxx");
            }*/

            // eventdispatchers
            /*{
                String module = "QtEventDispatcherSupport";
                auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, eventdispatchers, { module });

                SwapAndRestore sr(eventdispatchers.SourceDir);
                eventdispatchers.SourceDir /= "src/platformsupport/eventdispatchers";
                eventdispatchers += ".*"_rr;
                eventdispatchers -= ".*_glib.*"_rr;
                eventdispatchers -= ".*_qpa.*"_rr;
                eventdispatchers -= ".*_wasm.*"_rr;
                if (eventdispatchers.getBuildSettings().TargetOS.Type == OSType::Windows)
                    eventdispatchers -= ".*unix.*"_rr;
                else
                    eventdispatchers -= ".*windows.*"_rr;

                eventdispatchers.Public += gui;
                auto &t = eventdispatchers;

                auto mocs = automoc(moc, eventdispatchers);
                SW_QT_ADD_MOC_DEPS(eventdispatchers);

                t.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                #ifdef __cplusplus
                #include <QtCore/QtCore>
                #include <QtGui/QtGui>
                #endif
    )xxx");
            }*/

            // themes
            /*{
                themes +=
                    "src/platformsupport/themes/[^/]*"_rr;

                themes.Public +=
                    "src"_id;

                themes.Public += gui;

                String module = "QtThemeSupport";
                syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, themes, { module });
                themes.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                #ifdef __cplusplus
                #include <QtCore/QtCore>
                #include <QtGui/QtGui>
                #endif
    )xxx");
            }*/

            // windowsuiautomation
            /*{
                windowsuiautomation +=
                    "src/platformsupport/windowsuiautomation/.*"_rr
                    ;

                windowsuiautomation.Public += gui;

                String module = "QtWindowsUIAutomationSupport";
                syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, windowsuiautomation, { module });
                windowsuiautomation.writeFileOnce(module + "/" + module + "Depends", R"xxx(
                    #ifdef __cplusplus
                    #include <QtCore/QtCore>
                    #include <QtGui/QtGui>
                    #endif
    )xxx");
            }*/

            // fontdatabases
            /*{
                auto &freetype = fontdatabases.addTarget<StaticLibraryTarget>("freetype");
                {
                    String module = "QtFontDatabaseSupport";
                    syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, freetype, { module });

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
                    syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, windows, { module });

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
            }*/
        }

        auto &plugins = base.addDirectory("plugins");
        auto &platforms = plugins.addDirectory("platforms");
        auto &styles = plugins.addDirectory("styles");

        auto &windows = platforms.addTarget<LibraryTarget>("windows");
        {
            common_setup(windows);
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
                windows.Public += "Shcore.lib"_slib;
                windows.Public += "Comdlg32.lib"_slib;
                windows.Public += "D3d9.lib"_slib;
                windows.Public += "Shlwapi.lib"_slib;
                //windows += "EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE=EGL_PLATFORM_ANGLE_DEVICE_TYPE_D3D_WARP_ANGLE"_d;
            }

            //windows.Public += accessibility;
            //windows.Public += eventdispatchers;
            //windows.Public += themes;
            //windows.Public += windowsuiautomation;
            //windows.Public += fontdatabases_windows;

            automoc(moc, windows);

            {
                RccData d;
                d.files["src/plugins/platforms/windows/openglblacklists/default.json"] = "default.json";
                d.name = "openglblacklists";
                d.prefix = "/qt-project.org/windows/openglblacklists";
                ::rcc(rcc, windows, d);
            }
            {
                RccData d;
                for (auto &&f : {
                                    "images/closedhandcursor_32.png",
                                    "images/closedhandcursor_48.png",
                                    "images/closedhandcursor_64.png",
                                    "images/dragcopycursor_32.png"  ,
                                    "images/dragcopycursor_48.png"  ,
                                    "images/dragcopycursor_64.png"  ,
                                    "images/draglinkcursor_32.png"  ,
                                    "images/draglinkcursor_48.png"  ,
                                    "images/draglinkcursor_64.png"  ,
                                    "images/dragmovecursor_32.png"  ,
                                    "images/dragmovecursor_48.png"  ,
                                    "images/dragmovecursor_64.png"  ,
                                    "images/openhandcursor_32.png"  ,
                                    "images/openhandcursor_48.png"  ,
                                    "images/openhandcursor_64.png"  ,
                                    "images/splithcursor_32.png"    ,
                                    "images/splithcursor_48.png"    ,
                                    "images/splithcursor_64.png"    ,
                                    "images/splitvcursor_32.png"    ,
                                    "images/splitvcursor_48.png"    ,
                                    "images/splitvcursor_64.png"    ,
                                }) {
                    d.files["src/plugins/platforms/windows/"s + f];
                }
                d.name = "cursors";
                d.prefix = "/qt-project.org/windows/cursors";
                ::rcc(rcc, windows, d);
            }

            //windows.Public += "org.sw.demo.google.angle.egl"_dep;
            windows.Public += wintab;
            windows.Public += iaccessible2;
            windows.Public += opengl;

            make_qt_plugin(windows, "QWindowsIntegrationPlugin");
        }

        auto &xcb = platforms.addTarget<LibraryTarget>("xcb");
        {
            auto &t = xcb;
            common_setup(t, "Xcb");
            t.setOutputDir("plugins/platforms");
            t += "src/plugins/platforms/xcb/.*"_rr;
            t -= "src/plugins/platforms/xcb/nativepainting/.*"_rr;
            t.Public += "src/plugins/platforms/xcb"_id;
            t += "src/plugins/platforms/xcb/gl_integrations"_id;
            //t += "src/plugins/platforms/xcb/nativepainting"_id;

            t -= "src/plugins/platforms/xcb/.*vulkan.*"_rr;
            t -= "src/plugins/platforms/xcb/gl_integrations/xcb_egl/qxcbeglmain.cpp";
            t -= "src/plugins/platforms/xcb/gl_integrations/xcb_glx/qxcbglxmain.cpp";
            t -= "src/plugins/platforms/xcb/qxcbsessionmanager.cpp";

            t += "QT_BUILD_XCB_PLUGIN"_def;

            t += "xcb"_slib;
            t += "xcb-image"_slib;
            t += "xcb-render"_slib;
            t += "xcb-render-util"_slib;
            t += "xcb-randr"_slib;
            t += "xcb-shm"_slib;
            t += "xcb-glx"_slib;
            t += "xcb-xfixes"_slib;
            t += "xcb-xkb"_slib;
            t += "xcb-xinput"_slib;
            t += "xcb-shape"_slib;
            t += "xcb-icccm"_slib;
            t += "xcb-sync"_slib;
            t += "xcb-keysyms"_slib;
            t += "xkbcommon-x11"_slib;
            t += "X11-xcb"_slib;

            automoc(moc, t);

            t.Public += gui;
            t.Public += opengl;

            make_qt_plugin(t, "QXcbIntegrationPlugin");
        }

        // also add ios
        auto &cocoa = platforms.addTarget<LibraryTarget>("cocoa");
        {
            common_setup(cocoa);
            cocoa.setOutputDir("plugins/platforms");
            cocoa += "src/plugins/platforms/cocoa/.*"_rr;
            cocoa.Public += "src/plugins/platforms/cocoa"_id;

            cocoa -= "src/plugins/platforms/cocoa/qnsview_.*.mm"_rr;
            cocoa -= "src/plugins/platforms/cocoa/qcocoavulkaninstance.mm";

            // public?
            cocoa.Protected += "AppKit"_framework;
            cocoa.Protected += "Carbon"_framework;
            cocoa.Protected += "CoreServices"_framework;
            cocoa.Protected += "CoreVideo"_framework;
            cocoa.Protected += "IOKit"_framework;
            cocoa.Protected += "IOSurface"_framework;
            cocoa.Protected += "Metal"_framework;
            cocoa.Protected += "QuartzCore"_framework;
            cocoa.Protected += "OpenGL"_framework;

            automoc(moc, cocoa);

            RccData d;
            d.files["src/plugins/platforms/cocoa/images/sizeallcursor.png"] = "images/sizeallcursor.png";
            d.files["src/plugins/platforms/cocoa/images/spincursor.png"] = "images/spincursor.png";
            d.files["src/plugins/platforms/cocoa/images/waitcursor.png"] = "images/waitcursor.png";
            d.name = "qcocoaresources";
            d.prefix = "/qt-project.org/mac/cursors";
            ::rcc(rcc, cocoa, d);

            cocoa.Public += gui;

            make_qt_plugin(cocoa, "QCocoaIntegrationPlugin");
        }

        auto &windowsvista = styles.addTarget<LibraryTarget>("windowsvista");
        {
            common_setup(windowsvista);
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

            windowsvista.Public += widgets;

            automoc(moc, windowsvista);
            make_qt_plugin(windowsvista, "QWindowsVistaStylePlugin");
        }

        auto add_winmain = [&](auto &&name) {
            auto &winmain = base.addTarget<StaticLibraryTarget>("winmain");
            winmain -= "src/entrypoint/qtentrypoint_win.cpp";
            winmain -= "src/corelib/global/qt_windows.h";
            if (winmain.getBuildSettings().TargetOS.Type == OSType::Windows) {
                winmain += "src/entrypoint/qtentrypoint_win.cpp";
                winmain.Public += "src/corelib/global"_idir;
            } else {
                winmain.AutoDetectOptions = false;
                winmain.Empty = true;
            }
            //winmain.Public += core;
        };
        add_winmain("winmain");
        add_winmain("entrypoint"); // alias?

        auto &printsupport = base.addTarget<LibraryTarget>("printsupport");
        {
            common_setup(printsupport);
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, printsupport, { "QtPrintSupport" });

            SwapAndRestore sr(printsupport.SourceDir);
            printsupport.SourceDir /= "src/printsupport";

            printsupport +=
                "dialogs/.*"_rr,
                "kernel/.*"_rr,
                "widgets/.*"_rr;
            printsupport -=
                "platform/.*"_rr
                ;
            printsupport.Protected +=
                "."_idir,
                "dialogs"_idir,
                "kernel"_idir,
                "widgets"_idir
                ;

            if (printsupport.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                printsupport += "platform/windows/.*"_rr;
                printsupport -= "platform/windows/qwindowsprinterinfo.cpp";
                printsupport += "Winspool.lib"_slib;
                printsupport += "Comdlg32.lib"_slib;
                printsupport -= "kernel/qcups.cpp";
                printsupport -= "widgets/qcups.*"_rr;
            }

            printsupport.Public += widgets;

            if (network.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                qt_printsupport_desc.config.public_.features.insert({ "cups", false });
                qt_printsupport_desc.config.public_.features.insert({ "cupsjobwidget", false });
            }
            else
            {
                qt_printsupport_desc.config.public_.features.insert({ "cups", true });
                qt_printsupport_desc.config.public_.features.insert({ "cupsjobwidget", true });
            }
            if (printsupport.getBuildSettings().TargetOS.isApple())
            {
                printsupport += "Q_OS_MAC"_def; // for moc
            }
            qt_printsupport_desc.print(printsupport);

            platform_files(printsupport);

            if (printsupport.getBuildSettings().TargetOS.isApple())
            {
                printsupport -= "dialogs/qprintdialog_unix.*"_rr;
                printsupport -= "dialogs/qpagesetupdialog_unix.*"_rr;
                printsupport += "platform/macos/.*"_rr;
                printsupport += "cups"_slib;
            }

            auto mocs = automoc(moc, printsupport);
            SW_QT_ADD_MOC_DEPS(printsupport);

            //::rcc(rcc, printsupport, printsupport.SourceDir / "dialogs/qprintdialog.qrc");
            {
                RccData d;
                for (auto &&f : {
                                    "dialogs/images/zoom-fit-page-24.png"      ,
                                    "dialogs/images/zoom-fit-page-32.png"      ,
                                    "dialogs/images/zoom-fit-width-24.png"     ,
                                    "dialogs/images/zoom-fit-width-32.png"     ,
                                    "dialogs/images/go-first-24.png"           ,
                                    "dialogs/images/go-first-32.png"           ,
                                    "dialogs/images/go-last-24.png"            ,
                                    "dialogs/images/go-last-32.png"            ,
                                    "dialogs/images/go-next-24.png"            ,
                                    "dialogs/images/go-next-32.png"            ,
                                    "dialogs/images/go-previous-24.png"        ,
                                    "dialogs/images/go-previous-32.png"        ,
                                    "dialogs/images/layout-landscape-24.png"   ,
                                    "dialogs/images/layout-landscape-32.png"   ,
                                    "dialogs/images/layout-portrait-24.png"    ,
                                    "dialogs/images/layout-portrait-32.png"    ,
                                    "dialogs/images/page-setup-24.png"         ,
                                    "dialogs/images/page-setup-32.png"         ,
                                    "dialogs/images/printer-24.png"            ,
                                    "dialogs/images/printer-32.png"            ,
                                    "dialogs/images/view-pages-overview-24.png",
                                    "dialogs/images/view-pages-overview-32.png",
                                    "dialogs/images/view-pages-single-24.png"  ,
                                    "dialogs/images/view-pages-single-32.png"  ,
                                    "dialogs/images/view-pages-facing-24.png"  ,
                                    "dialogs/images/view-pages-facing-32.png"  ,
                                    "dialogs/images/zoom-in-24.png"            ,
                                    "dialogs/images/zoom-in-32.png"            ,
                                    "dialogs/images/zoom-out-24.png"           ,
                                    "dialogs/images/zoom-out-32.png"           ,
                                }) {
                    d.files[f];
                }
                d.name = "qprintdialog";
                d.prefix = "/qt-project.org/dialogs/qprintdialog";
                d.base = "dialogs";
                ::rcc(rcc, printsupport, d);
            }

            for (auto &f : enumerate_files_like(printsupport.SourceDir, ".*\\.ui"))
                ::uic(uic, printsupport, f);
        }

        auto &plugins_printsupport = plugins.addDirectory("printsupport");

        /*auto &plugins_printsupport_windows = plugins_printsupport.addTarget<LibraryTarget>("windows");
        {
            plugins_printsupport_windows.setOutputDir("plugins/printsupport");
            plugins_printsupport_windows +=
                "src/plugins/printsupport/platform/windows/.*"_rr;
            plugins_printsupport_windows.Public +=
                "src/plugins/printsupport/platform/windows"_id;
            plugins_printsupport_windows += "Winspool.lib"_slib;

            plugins_printsupport_windows.Public += printsupport;
            automoc(moc, plugins_printsupport_windows);
            make_qt_plugin(plugins_printsupport_windows, "QWindowsPrinterSupportPlugin");
        }*/
        auto &plugins_printsupport_cups = plugins_printsupport.addTarget<LibraryTarget>("cups");
        {
            common_setup(plugins_printsupport_cups);
            plugins_printsupport_cups.setOutputDir("plugins/printsupport");
            plugins_printsupport_cups +=
                "src/plugins/printsupport/cups/.*"_rr;
            plugins_printsupport_cups.Public +=
                "src/plugins/printsupport/cups"_id;

            plugins_printsupport_cups.Public += printsupport;
            plugins_printsupport_cups += "cups"_slib;
            automoc(moc, plugins_printsupport_cups);
            make_qt_plugin(plugins_printsupport_cups, "QCupsPrinterSupportPlugin");
        }

        //
        auto &sqldrivers = plugins.addDirectory("sqldrivers");

        auto &sqlite = sqldrivers.addTarget<LibraryTarget>("sqlite");
        {
            common_setup(sqlite);
            sqlite.setOutputDir("plugins/sqldrivers");
            sqlite += "src/plugins/sqldrivers/sqlite/.*"_rr;
            sqlite.Public += sql;
            sqlite.Private += sw::Shared, "QT_PLUGIN"_d;
            sqlite += "org.sw.demo.sqlite3"_dep;

            automoc(moc, sqlite);
        }

        auto &psql = sqldrivers.addTarget<LibraryTarget>("psql");
        {
            common_setup(psql);
            psql.setOutputDir("plugins/sqldrivers");
            psql += "src/plugins/sqldrivers/psql/.*"_rr;
            psql.Public += sql;
            psql.Private += sw::Shared, "QT_PLUGIN"_d;
            psql += "org.sw.demo.postgres.pq"_dep;

            automoc(moc, psql);
        }

        auto &odbc = sqldrivers.addTarget<LibraryTarget>("odbc");
        {
            common_setup(odbc);
            odbc.setOutputDir("plugins/sqldrivers");
            odbc += "src/plugins/sqldrivers/odbc/.*"_rr;
            odbc.Public += sql;
            odbc.Private += sw::Shared, "QT_PLUGIN"_d;
            if (odbc.getBuildSettings().TargetOS.Type == OSType::Windows)
                odbc += "odbc32.lib"_slib;

            automoc(moc, odbc);
        }
    }

    // declarative
    auto &declarative = add_subproject<Project>(qt, "declarative");
    NativeExecutedTarget *qml_imports_layouts, *qml_imports_window, *pquick;
    {
        auto &third_party = declarative.addDirectory("third_party");
        auto &qml = declarative.addLibrary("qml");

        auto &integration = qml.addLibrary("integration");
        {
            common_setup(integration);
            String module = "QtQmlIntegration";
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, integration, { module });
            integration.SourceDir /= "src/qmlintegration";
            integration.Public += core;
        }

        auto &masm = third_party.addStaticLibrary("masm");
        {
            common_setup(masm);
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
            masm.Public += integration;
            (masm + qml)->IncludeDirectoriesOnly = true;

            if (masm.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                masm += "src/3rdparty/masm/wtf/OSAllocatorWin.cpp"_rr;
            }
            else
            {
                masm += "src/3rdparty/masm/wtf/OSAllocatorPosix.cpp"_rr;
            }

            masm -= "org.sw.demo.python.exe-2"_dep;
            masm -= "org.sw.demo.python.exe-3"_dep;
            {
                auto c = masm.addCommand();
                if (masm.getBuildSettings().TargetOS.Type == OSType::Windows)
                    c << cmd::prog("org.sw.demo.python.exe-2"_dep);
                else
                    c << cmd::prog("org.sw.demo.python.exe-3"_dep);
                c
                    << cmd::wdir(masm.BinaryDir)
                    << "-B" // prevent .pyc
                    << cmd::in("src/3rdparty/masm/disassembler/udis86/itab.py")
                    << cmd::in("src/3rdparty/masm/disassembler/udis86/optable.xml")
                    << cmd::end()
                    << cmd::out("udis86_itab.c")
                    << cmd::out("udis86_itab.h")
                    ;
            }

            {
                auto c = masm.addCommand();
                if (masm.getBuildSettings().TargetOS.Type == OSType::Windows)
                    c << cmd::prog("org.sw.demo.python.exe-2"_dep);
                else
                    c << cmd::prog("org.sw.demo.python.exe-3"_dep);
                c
                    << "-B" // prevent .pyc
                    << cmd::in("src/3rdparty/masm/yarr/create_regex_tables")
                    << cmd::std_out("RegExpJitTables.h")
                    ;
            }
        }

        // qml
        {
            common_setup(qml);
            String module = "QtQml";
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, qml, { module });
            //masm += sqt; // masm needs this sqt dependency, but creates cyclic dep
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
            //qml += "qml/v8"_idir;
            qml += "compiler"_idir;
            qml += "debugger"_idir;
            qml += "memory"_idir;
            qml += "qmldirparser"_idir;
            //qml += "qml/jsapi"_idir;
            qml.Protected += "jsruntime"_idir;

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
            qml.Public += integration;
            qml.Public += network;

            qml.writeFileOnce("private/qml_compile_hash_p.h", R"xxx(
            #define QML_COMPILE_HASH "12345"
            #define QML_COMPILE_HASH_LENGTH 5
)xxx");

            platform_files(qml);
            auto mocs = automoc(moc, qml);
            SW_QT_ADD_MOC_DEPS(qml);

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
            common_setup(qml_models, "qmlmodels");
            String module = "QtQmlModels";
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, qml_models, { module });
            qml_models.SourceDir /= "src/qmlmodels";
            qml_models += ".*"_r;
            qml_models.Public += qml;
            auto mocs = automoc(moc, qml_models);
            SW_QT_ADD_MOC_DEPS(qml_models);

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
            common_setup(qml_workerscript);
            auto &t = qml_workerscript;

            String module = "QtQmlWorkerScript";
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, t, { module });
            t.SourceDir /= "src/qmlworkerscript";

            t += ".*"_r;
            t += "QT_BUILD_QMLWORKERSCRIPT_LIB"_def;

            t.Public += qml, masm;

            auto mocs = automoc(moc, t);
            SW_QT_ADD_MOC_DEPS(t);

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

        auto &quick = declarative.addLibrary("quick");
        pquick = &quick;
        {
            common_setup(quick);
            String module = "QtQuick";
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, quick, { module });
            quick.SourceDir /= "src/quick";

            quick += "[^/]*"_rr;
            quick += "util/.*"_rr;
            quick += "scenegraph/.*"_rr;
            quick += "items/.*"_rr;
            quick += "handlers/.*"_rr;
            quick += "accessible/.*"_rr;
            quick -= "qtquickplugin.cpp";

            quick += "."_idir;

            //quick += "M_PI=3.14159265358979323846"_d;

            if (quick.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                quick.Private += "UNICODE"_d;
            }

            quick.Public += qml, gui, qml_models, opengl;

            // make builtin function!
            auto make_qml_plugin = [](auto &&t, String name) {
                String text = R"xxx(// This file is autogenerated by SW. Do not edit.

#include <QtQml/qqmlextensionplugin.h>

qt_qml_plugin_intro

class qt_qml_plugin_class_name : public QQmlEngineExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)

public:
    qt_qml_plugin_class_name(QObject *parent = nullptr) : QQmlEngineExtensionPlugin(parent)
    {
        qt_qml_plugin_constructor_content
    }
};

qt_qml_plugin_outro

#include "qt_qml_plugin_moc_include_name"
                )xxx";
                String escaped_name = name; // TODO: escape name
                String generated_cpp_file_name_base = "qml_plugin_" + escaped_name;
                String register_types_function_name = "qml_register_types_" + escaped_name;
                String qt_qml_plugin_intro = "extern void " + register_types_function_name + "();\nQ_GHS_KEEP_REFERENCE(" + register_types_function_name + ");";
                String qt_qml_plugin_class_name = name;
                String qt_qml_plugin_constructor_content = "volatile auto registration = &" + register_types_function_name + ";\nQ_UNUSED(registration);";
                String qt_qml_plugin_outro;
                String qt_qml_plugin_moc_include_name = generated_cpp_file_name_base + ".moc";
                //if (building_qt) {
                    qt_qml_plugin_intro += "\n\nQT_BEGIN_NAMESPACE";
                    qt_qml_plugin_outro += "QT_END_NAMESPACE";
                //}
                boost::replace_all(text, "qt_qml_plugin_intro", qt_qml_plugin_intro);
                boost::replace_all(text, "qt_qml_plugin_class_name", qt_qml_plugin_class_name);
                boost::replace_all(text, "qt_qml_plugin_constructor_content", qt_qml_plugin_constructor_content);
                boost::replace_all(text, "qt_qml_plugin_outro", qt_qml_plugin_outro);
                boost::replace_all(text, "qt_qml_plugin_moc_include_name", qt_qml_plugin_moc_include_name);
                t.writeFileOnce(generated_cpp_file_name_base + ".cpp", text);
                t += path{generated_cpp_file_name_base + ".cpp"};
            };
            //make_qml_plugin(quick, "QtQuick");

            auto mocs = automoc(moc, quick);
            SW_QT_ADD_MOC_DEPS(quick);
            //::rcc(rcc, quick, quick.SourceDir / "items/items.qrc");
            //::rcc(rcc, quick, quick.SourceDir / "scenegraph/scenegraph.qrc");

            RccData d;
            for (auto &&f : enumerate_files(quick.SourceDir / "scenegraph" / "shaders_ng"))
                d.files[f];
            d.name = "scenegraph_shaders";
            d.prefix = "/qt-project.org/scenegraph";
            ::rcc(rcc, quick, d);

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
        }

        /*auto &imports = qml.addDirectory("imports");

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

            automoc(moc, t);

            return t;
        };

        add_import("qtquick2") += qml_workerscript;
        qml_imports_layouts = &add_import("layouts");
        qml_imports_window = &add_import("window");
        add_import("settings");
        add_import("folderlistmodel");*/
    }

    // svg
    {
        auto &svg = add_subproject<Library>(qt, "svg");
        common_setup(svg);

        auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, svg, { "QtSvg" });
        auto &t = svg;

        svg += "src/svg/[^/]*"_rr;
        svg += "QT_USE_QSTRINGBUILDER"_d;
        svg.Public += gui;
        if (svg.getBuildSettings().TargetOS.Type == OSType::Windows)
            svg.Private += "UNICODE"_d;
        auto mocs = automoc(moc, svg);
        SW_QT_ADD_MOC_DEPS(svg);
        qt_svg_desc.print(svg);
    }

    //auto &plugins = qt.addDirectory("plugins");

    //
    {
        auto &imageformats = add_subproject<Project>(qt, "imageformats");

        auto add_imageformat1 = [&common_setup, &s, &moc, &gui](auto &t, const String &name)
        {
            common_setup(t);
            t.setOutputDir("plugins/imageformats");
            t.SourceDir /= "src/plugins/imageformats";
            t.SourceDir /= name;
            t += ".*\\.cpp"_rr, ".*\\.h"_rr, ".*\\.json"_rr;
            if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
                t.Private += "UNICODE"_d;
            t.Private += sw::Static, "QT_STATICPLUGIN"_d;
            automoc(moc, t);
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
            common_setup(formats);
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
            formats += "QT_USE_QSTRINGBUILDER"_d;

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
            common_setup(convert);
            convert += "src/linguist/lconvert/main.cpp";
            if (convert.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                convert.Private += "UNICODE"_d;
            }
            convert.Public += formats;
        }

        auto &proparser = linguist.addTarget<StaticLibraryTarget>("proparser");
        {
            common_setup(proparser);
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
            proparser += "QT_USE_QSTRINGBUILDER"_d;

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
            common_setup(release);
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
            common_setup(update);
            update +=
                "src/linguist/lupdate/.*\\.cpp"_rr,
                "src/linguist/lupdate/.*\\.h"_rr;

            update -=
                "src/linguist/lupdate/qdeclarative.cpp",
                "src/linguist/lupdate/lupdatepreprocessoraction.cpp",
                "src/linguist/lupdate/clangtoolastreader.cpp",
                "src/linguist/lupdate/cpp_clang.cpp"
                ;

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
            //update.writeFileOnce("QtTools/private/qttools-config_p.h", "");
            qt_tools_desc.print(update);
        }
    }

    auto &translations = add_subproject<LibraryTarget>(qt, "translations");
    //translations.Empty = true;
    translations += "translations/.*\\.ts"_rr;

    auto &wayland = add_subproject<Project>(qt, "wayland");
    {
        auto &scanner = wayland.addExecutable("scanner");
        {
            common_setup(scanner);
            scanner += "src/qtwaylandscanner/qtwaylandscanner.cpp";
            scanner += core;
        }

        auto generate_wayland_protocol_client_sources = [&](auto &&t, const path &fn, auto &&dir) {
            auto protocol_name = fn.stem().string();

            auto waylandscanner_header_output = "wayland-" + protocol_name + "-client-protocol.h";
            auto waylandscanner_code_output = "wayland-" + protocol_name + "-protocol.c";

            auto qtwaylandscanner_header_output = "qwayland-" + protocol_name + ".h";
            auto qtwaylandscanner_code_output = "qwayland-" + protocol_name + ".cpp";

            path idir = dir;
            auto qtwaylandscanner_code_include = "<QtWaylandClient/private/wayland-wayland-client-protocol.h>"s;

            t.addCommand()
                << cmd::prog("wayland-scanner")
                << "--strict"
                << "--include-core-only"
                << "client-header"
                << cmd::std_in(fn)
                << cmd::std_out(idir / waylandscanner_header_output);
            t.addCommand()
                << cmd::prog("wayland-scanner")
                << "--strict"
                << "--include-core-only"
                << "public-code"
                << cmd::std_in(fn)
                << cmd::std_out(waylandscanner_code_output);

            t.addCommand()
                << cmd::prog(scanner)
                << "client-header"
                << cmd::in(fn)
                << idir
                << cmd::std_out(idir / qtwaylandscanner_header_output);
            t.addCommand()
                << cmd::prog(scanner)
                << "client-code"
                << cmd::in(fn)
                << "--header-path=" + idir.string() + ""
                << "--add-include=" + qtwaylandscanner_code_include + ""
                << cmd::std_out(qtwaylandscanner_code_output);
        };

        auto &client = wayland.addLibrary("client");
        {
            common_setup(client, "WaylandClient");
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, client, { "QtWaylandClient" });

            client += "src/shared/.*"_rr;
            client += "src/client/.*"_rr;
            client -= "src/client/qwaylandvulkan.*"_rr;

            client += "src/client"_idir;
            client += "src/shared"_idir;
            client += "src/client/hardwareintegration"_idir;
            client += "src/client/inputdeviceintegration"_idir;
            client += "src/client/shellintegration"_idir;

            client -= "src/client/qwaylandtextinputv4.cpp";
            client -= "src/3rdparty/protocol/.*xml"_rr;
            client -= "src/extensions/.*xml"_rr;

            client.Public += gui;

            client.Public += "wayland-client"_slib;
            client.Public += "wayland-cursor"_slib;

            auto mocs = automoc(moc, client);
            SW_QT_ADD_MOC_DEPS(client);

            auto f = [&](auto &&n) {
                QtLibrary qt_wayland_desc{n};
                qt_wayland_desc.config.public_.features.insert({"tabletevent", true});
                qt_wayland_desc.config.public_.features.insert({"wayland_text_input_v4_wip", false});
                qt_wayland_desc.config.public_.features.insert({"clipboard", true});
                //qt_wayland_desc.config.public_.features.insert({"vulkan", true});
                qt_wayland_desc.config.public_.features.insert({"cursor", true});
                qt_wayland_desc.config.public_.features.insert({"wayland_datadevice", true});
                qt_wayland_desc.config.public_.features.insert({"wayland_client_primary_selection", true});
                qt_wayland_desc.config.public_.features.insert({"draganddrop", true});
                //qt_wayland_desc.config.public_.features.insert({"xkbcommon", true});
                qt_wayland_desc.print(client);
            };
            f("QtWaylandClient");
            f("QtWaylandGlobal");

            for (auto &&f : {
                "src/3rdparty/protocol/pointer-gestures-unstable-v1.xml",
                "src/3rdparty/protocol/tablet-unstable-v2.xml",
                "src/3rdparty/protocol/text-input-unstable-v1.xml",
                "src/3rdparty/protocol/text-input-unstable-v2.xml",
                "src/3rdparty/protocol/text-input-unstable-v4-wip.xml",
                "src/3rdparty/protocol/wayland.xml",
                "src/3rdparty/protocol/wp-primary-selection-unstable-v1.xml",
                "src/3rdparty/protocol/xdg-activation-v1.xml",
                "src/3rdparty/protocol/xdg-output-unstable-v1.xml",
                "src/extensions/qt-key-unstable-v1.xml",
                "src/extensions/qt-text-input-method-unstable-v1.xml",
                "src/extensions/qt-windowmanager.xml",
                "src/extensions/surface-extension.xml",
                "src/extensions/touch-extension.xml",
                "src/extensions/hardware-integration.xml",
                "src/extensions/server-buffer-extension.xml",
            }) {
                generate_wayland_protocol_client_sources(client, f, "QtWaylandClient/private");
                generate_wayland_protocol_client_sources(client, f, "");
            }
            //client.Protected += IncludeDirectory{client.BinaryPrivateDir / "QtWaylandClient/private"};
        }

        auto &hwi_egl = wayland.addLibrary("hardwareintegration.client.wayland.egl");
        {
            common_setup(hwi_egl);
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, hwi_egl, { "QtWaylandEglClientHwIntegration" });

            hwi_egl += "src/hardwareintegration/client/wayland-egl/.*"_rr;
            hwi_egl.Public += client;
            hwi_egl.Public += opengl;

            auto mocs = automoc(moc, hwi_egl);
            SW_QT_ADD_MOC_DEPS(client);

            hwi_egl.Public += "wayland-egl"_slib;
        }

        auto &bradient = wayland.addLibrary("plugins.decorations.bradient");
        {
            common_setup(bradient);
            bradient += "src/plugins/decorations/bradient/.*"_rr;
            automoc(moc, bradient);
            bradient.Public += client;
            make_qt_plugin(bradient, "QWaylandBradientDecorationPlugin");
        }

        auto &p_egl = wayland.addLibrary("plugins.hardwareintegration.client.wayland_egl");
        {
            common_setup(p_egl);
            p_egl += "src/plugins/hardwareintegration/client/wayland-egl/.*"_rr;
            automoc(moc, p_egl);
            p_egl.Public += hwi_egl;
            make_qt_plugin(p_egl, "QWaylandEglClientBufferPlugin");
        }

        auto &xdg = wayland.addLibrary("plugins.shellintegration.xdg");
        {
            common_setup(xdg);
            xdg += "src/plugins/shellintegration/xdg-shell/.*"_rr;
            automoc(moc, xdg);
            xdg.Public += client;

            for (auto &&f : {
                "src/3rdparty/protocol/xdg-decoration-unstable-v1.xml",
                "src/3rdparty/protocol/xdg-shell.xml",
            }) {
                generate_wayland_protocol_client_sources(xdg, f, ".");
            }
            make_qt_plugin(xdg, "QWaylandXdgShellIntegrationPlugin");
        }

        auto &generic = wayland.addLibrary("plugins.platforms.qwayland.generic");
        {
            common_setup(generic);
            generic += "src/plugins/platforms/qwayland-generic/.*"_rr;
            automoc(moc, generic);
            generic.Public += client;
            make_qt_plugin(generic, "QWaylandIntegrationPlugin");
        }
        auto &egl = wayland.addLibrary("plugins.platforms.qwayland.egl");
        {
            common_setup(egl);
            egl += "src/plugins/platforms/qwayland-egl/.*"_rr;
            automoc(moc, egl);
            egl.Public += hwi_egl;
            make_qt_plugin(egl, "QWaylandEglPlatformIntegrationPlugin");
        }
    }

    auto &serialport = add_subproject<Library>(qt, "serialport");
    {
        common_setup(serialport);
        serialport += "src/.*"_rr;
        platform_files(serialport);
        auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, serialport, { "QtSerialPort" });
        serialport.Public += core;
        automoc(moc, serialport);
        if (serialport.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            serialport.Protected += "UNICODE"_d;
            serialport += "Setupapi.lib"_slib;
        }
    }

    auto &qt5compat = add_subproject<Library>(qt, "5compat");
    {
        auto &t = qt5compat;
        common_setup(t, "core5compat");
        t += "src/core5/.*"_rr;
        t -= "src/core5/doc/.*"_rr;
        t -= "src/core5/codecs/qiconvcodec.cpp";
        t -= "src/core5/codecs/qicucodec.cpp";
        //t -= "src/core5/codecs/qtextcodec.cpp";
        platform_files(t);
        t += "QT_USE_FAST_OPERATOR_PLUS"_def;
        t += "QT_USE_QSTRINGBUILDER"_def;
        auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, t, { "QtCore5Compat" });
        t.Public += gui, xml;
        automoc(moc, t);

        QtLibrary qt5compat_desc{
            "QtCore5Compat",
            // config
            {
                // public
                {
                    // features
                    {
                    }
                },
                // private
                {
                    // features
                    {
                        {"codecs", true},
                        {"iconv", false},
                        {"icu", false},
                        {"textcodec", true},
                    }
                }
            },
            // deps
            {
                "QtGui",
                "QtXml",
            },
        };
        qt5compat_desc.print(t);
        /*
        exports_setup(bootstrap, "xml");
        //bootstrap.writeFileOnce("QtXml/qtxmlexports.h");
        bootstrap.writeFileOnce("QtXml/qtxml-config.h");
        */
    }

    auto &webengine = add_subproject<Project>(qt, "webengine");
    {
        auto &core = webengine.addLibrary("core");
        common_setup(core, "webenginecore");
        core += cpp17;
        core += "src/core/.*"_rr;
        core -= "src/core/doc/.*"_rr;
        core += "BUILDING_CHROMIUM"_def;
        core.Public += "src/core"_idir;
        core.Public += "src/core/api"_idir;
        //core.Public += "d:/dev/chromium/src"_idir;
        //core.Public += "d:/dev/chromium/src/out/ninja2/gen"_idir;
        auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, core, { "QtWebEngineCore" });
        core.Public += gui, network, *pquick;
        automoc(moc, core);

        QtLibrary webengine_core_desc{
            "QtWebEngineCore",
            // config
            {
                // public
                {
                    // features
                    {
                        {"webengine_webchannel", true},
                        {"draganddrop", true},
                        {"webengine_printing_and_pdf", true},
                    }
                },
                // private
                {
                    // features
                    {
                    }
                }
            },
            // deps
            {
                "QtGui",
                "QtNetwork",
            },
        };
        webengine_core_desc.print(core);
    }

    return;

    auto &webview = add_subproject<Library>(qt, "webview");
    {
        common_setup(webview);
        webview += "src/webview/.*"_r;
        auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, webview, { "QtWebView" });
        webview.Public += gui;
        automoc(moc, webview);
    }

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
            automoc(moc, controls);

            setup_output(controls, "Controls", "src/controls");
        }

        auto &dialogs = quickcontrols.addLibrary("dialogs");
        auto &dialogs_pvt = dialogs.addLibrary("Private");
        {
            dialogs_pvt += "src/dialogs/Private/.*"_rr;
            dialogs_pvt.Public += controls;
            automoc(moc, dialogs_pvt);

            setup_output(dialogs_pvt, "Dialogs/Private", "src/dialogs/Private");
        }

        {
            dialogs += "src/dialogs/[^/]*\\.cpp"_rr;
            dialogs += "src/dialogs/[^/]*\\.h"_rr;
            dialogs += "src/dialogs/[^/]*\\.qml"_rr;
            dialogs += "src/dialogs/images/.*"_rr;
            dialogs += "src/dialogs/qml/.*"_rr;
            dialogs.Public += dialogs_pvt;
            automoc(moc, dialogs);

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
            automoc(moc, widgets);

            setup_output(widgets, "PrivateWidgets", "src/widgets");
        }
    }
}
