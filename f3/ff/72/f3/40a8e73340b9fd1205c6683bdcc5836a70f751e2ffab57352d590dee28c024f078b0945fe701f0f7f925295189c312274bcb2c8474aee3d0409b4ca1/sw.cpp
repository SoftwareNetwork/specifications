#pragma sw require header pub.egorpugin.primitives.tools.syncqt

#pragma sw header on

#pragma sw require pub.egorpugin.primitives.hash-master

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

    std::vector<moc_file> files;
    for (auto &[p, f] : t)
    {
        if (f->skip)
            continue;
        if (p.extension() == ".cpp")
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
        auto ext = to_string(p.extension().u8string());
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
    Files mocs;
    for (auto &f : files)
    {
#ifdef _WIN32
        f.p = normalize_path_windows(f.p);
#endif

        auto dir = sha256(to_string(f.p.parent_path().u8string())).substr(0, 8);
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
        s += "\">" + to_string(normalize_path(fn)) + "</file>";
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
    t -= ".*_x11.*"_rr;
    t -= ".*_wasm.*"_rr;
    t -= ".*_coretext.*"_rr;
    if (t.getBuildSettings().TargetOS.Type == OSType::Windows) {
        t += ".*_win.*"_rr;
    } else {
        t += ".*_unix.*"_rr;
        t += ".*_posix.*"_rr;
        t += ".*_systemv.*"_rr;
        t += ".*_tz.*"_rr;
        t += ".*_qpa.*"_rr;
        t += ".*_stub.*"_rr;
        //t += ".*_generic.*"_rr;
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
        if (sw::isCppHeaderFileExtension(to_printable_string(p)) || sw::isCppSourceFileExtensions(to_printable_string(p)))
            ;
        else if (f->skip)
            continue;
        if (p.extension() == ".ts")
            ts_files.insert(p);
        else
        {
            if (sw::isCppHeaderFileExtension(to_printable_string(p)) || sw::isCppSourceFileExtensions(to_printable_string(p)))
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
    for (auto &f : sources)
        c << cmd::in(sources);

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
    d.outfilename_prefix = "qmake_";
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

        t.writeFileOnce(actual_name + "/" + config_fn, config.public_.print());
        t.writeFileOnce(actual_name + "/private/" + private_config_fn, config.private_.print());
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

static QtLibrary qt_desc{
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
    {"c__1z", false},
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
                {"QT_COMPILER_SUPPORTS_SSE2", "1"},
    {"QT_COMPILER_SUPPORTS_SSE3", "1"},
    {"QT_COMPILER_SUPPORTS_SSSE3", "1"},
    {"QT_COMPILER_SUPPORTS_SSE4_1", "1"},
    {"QT_COMPILER_SUPPORTS_SSE4_2", "1"},
    {"QT_COMPILER_SUPPORTS_AVX", "1"},
    {"QT_COMPILER_SUPPORTS_AVX2", "1"},
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
    {"sse2", true},
    {"system_zlib", false},
    {"widgets", true},
            },
        },
},
};

static QtLibrary qt_core_desc{
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
    {"process", true},
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
    {"icu", false},
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

static QtLibrary qt_gui_desc{
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
    {"eglfs_brcm", false},
    {"eglfs_egldevice", false},
    {"eglfs", false},
    {"eglfs_gbm", false},
    {"eglfs_mali", false},
    {"eglfs_viv", false},
    {"eglfs_viv_wl", false},
    {"egl", false},
    {"egl_x11", false},
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
    {"QT_QPA_DEFAULT_PLATFORM_NAME", "\"windows\""},
    {"QT_NO_ACCESSIBILITY_ATSPI_BRIDGE", ""},
    {"QT_OPENGL_DYNAMIC", "true"},
    {"QT_OPENGL_ES_2_ANGLE", "true"},
    {"QT_NO_FONTCONFIG", ""},
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
    {"fontconfig", false},
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
    {"xkbcommon_evdev", false},
    {"xkb", false},
    {"xlib", false},
    {"xrender", false},

    {"raster_64bit", true},
            },
        },
},
// deps
    {"QtCore"},
};

static QtLibrary qt_widgets_desc{
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

static QtLibrary qt_network_desc{
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

static QtLibrary qt_opengl_desc{
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

static QtLibrary qt_xml_desc{
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

static QtLibrary qt_sql_desc{
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

static QtLibrary qt_printsupport_desc{
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

static QtLibrary qt_svg_desc{
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

static QtLibrary qt_tools_desc{
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
    auto &qt = s.addProject("qtproject.qt", "6.2.3");

    auto &base = add_subproject<Project>(qt, "base");

    auto &base_tools = base.addDirectory("tools");
    auto &moc = base_tools.addTarget<ExecutableTarget>("moc");
    auto &rcc = base_tools.addTarget<ExecutableTarget>("rcc");
    auto &uic = base_tools.addTarget<ExecutableTarget>("uic");
    auto &tracegen = base_tools.addTarget<ExecutableTarget>("tracegen");

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

    auto write_tracepoints = [&tracegen](auto &t)
    {
        auto c = t.addCommand();
        c << cmd::prog(tracegen);
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows) {
            c
                << "etw"
                << cmd::in("qt" + t.getPackage().getPath().back() + ".tracepoints")
                << cmd::out("qt" + t.getPackage().getPath().back() + "_tracepoints_p.h")
            ;
        } else {
            c
                << "lttng"
                << cmd::in("qt" + t.getPackage().getPath().back() + ".tracepoints")
                << cmd::out("qt" + t.getPackage().getPath().back() + "_tracepoints_p.h")
            ;
        }
    };

    auto common_setup = [](auto &t)
    {
        t += cpp17;
        if (t.getCompilerType() == CompilerType::MSVC)
            t.Public.CompileOptions.push_back("/Zc:__cplusplus");
        if (t.getBuildSettings().TargetOS.Type != OSType::Windows) {
            t.ExportAllSymbols = true;
        }

    };

    // base
    {
        auto &third_party = base.addDirectory("third_party");

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

        auto &forkfd = third_party.addTarget<StaticLibraryTarget>("forkfd");
        {
            forkfd += c99;
            forkfd -= "src/3rdparty/forkfd/.*"_rr;
            forkfd += "src/3rdparty/forkfd/forkfd.c";
            forkfd += "FORKFD_DISABLE_FORK_FALLBACK"_def;
            forkfd.Public += "src/3rdparty/forkfd"_idir;
        }

        auto &tinycbor = third_party.addTarget<StaticLibraryTarget>("tinycbor");
        {
            tinycbor.setRootDirectory("src/3rdparty/tinycbor");
        }

        auto &bootstrap = base_tools.addTarget<StaticLibraryTarget>("bootstrap");
        {
            common_setup(bootstrap);
            {
                bootstrap +=
                    "mkspecs/.*\\.h"_rr,
                    "src/3rdparty/freebsd/strtoll.c",
                    "src/3rdparty/freebsd/strtoull.c",
                    "src/3rdparty/sha1/sha1.cpp",
                    "src/corelib/.*\\.h"_rr,
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
                    "src/corelib/serialization/qxmlstreamgrammar.cpp",
                    "src/corelib/serialization/qxmlutils.cpp",
                    "src/corelib/text/qbytearray.cpp",
                    "src/corelib/text/qbytearraylist.cpp",
                    "src/corelib/text/qbytearraymatcher.cpp",
                    "src/corelib/text/qchar.cpp",
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
                    "src/corelib/time/qgregorian.*"_rr,
                    "src/corelib/time/qroman.*"_rr,
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
            bootstrap -=
                "src/corelib/kernel/qcoreapplication_mac.cpp",
                "src/corelib/global/qoperatingsystemversion_darwin.mm",
                "src/corelib/io/qstandardpaths_mac.mm"
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
            if (bootstrap.getBuildSettings().TargetOS.Type == OSType::Macos)
            {
                bootstrap +=
                    "src/corelib/kernel/qcoreapplication_mac.cpp",
                    "src/corelib/global/qoperatingsystemversion_darwin.mm",
                    "src/corelib/io/qstandardpaths_mac.mm"
                    ;
            }

            //auto c = bootstrap.addCommand(SW_VISIBLE_BUILTIN_FUNCTION(copy_file));
            //c << cmd::in("src/xml/sax/qxml_p.h") << cmd::out("private/qxml_p.h");
            syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, bootstrap, { "QtCore", "QtXml" });

            if (bootstrap.getCompilerType() == CompilerType::MSVC)
                bootstrap.Public += "mkspecs/win32-msvc"_idir;
            else if (bootstrap.getCompilerType() == CompilerType::GNU)
                bootstrap.Public += "mkspecs/linux-g++-64"_idir;

            bootstrap.patch("src/corelib/global/qconfig-bootstrapped.h", "#define QT_NO_DEPRECATED", "//#define  QT_NO_DEPRECATED");
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

        // core
        {
            common_setup(core);
            core -=
                "mkspecs/.*\\.h"_rr;
            core +=
                "src/3rdparty/easing/easing.cpp",
                "src/3rdparty/freebsd/.*\\.c"_rr,
                "src/3rdparty/md[45]/.*"_rr,
                "src/3rdparty/rfc6234/.*\\.[hc]"_rr,
                "src/3rdparty/sha1/sha1.cpp",
                "src/3rdparty/sha3/.*"_rr,
                "src/3rdparty/tinycbor/src/.*"_rr
                ;
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
            else if (core.getCompilerType() == CompilerType::GNU)
                core.Public += "mkspecs/linux-g++-64"_idir;

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
                core += "plugin/qsystemlibrary.cpp";
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

            if (core.getBuildSettings().TargetOS.Type == OSType::Macos)
            {
                core += "kernel/qcoreapplication_mac.cpp";
                core += "kernel/qcore_mac_objc.cpp";
                core += "kernel/qcore_foundation.cpp";
                core += "kernel/qcore_foundation.cpp";
                core += "io/qstandardpaths_mac.mm";
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

            core.Private += "QT_BUILD_CORE_LIB"_d;
            core.Public += "QT_NO_VERSION_TAGGING"_d;
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
            }
            else
            {
                qt_core_desc.config.public_.features.insert({ "alloca", true });
                qt_core_desc.config.public_.features.insert({ "alloca_h", true });
                qt_core_desc.config.public_.features.insert({ "getauxval", true });
                qt_core_desc.config.public_.features.insert({ "linkat", true });
                qt_core_desc.config.public_.features.insert({ "renameat2", true });
                qt_core_desc.config.public_.features.insert({ "statx", true });
                qt_core_desc.config.public_.features.insert({ "futimens", true });
                qt_core_desc.config.public_.features.insert({ "poll_poll", false });
                qt_core_desc.config.public_.features.insert({ "poll_pollts", false });
                qt_core_desc.config.public_.features.insert({ "poll_ppoll", true });
                qt_core_desc.config.public_.features.insert({ "poll_select", true });
                qt_core_desc.config.public_.features.insert({ "dlopen", true });
                qt_core_desc.config.public_.features.insert({ "forkfd_pidfd", true });
            }
            if (core.getBuildSettings().TargetOS.Type == OSType::Linux) {
                qt_core_desc.config.public_.features.insert({ "glibc", true });
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

            auto mocs = automoc(moc, core);
            SW_QT_ADD_MOC_DEPS(core);
            ::rcc(rcc, core, core.SourceDir / "mimetypes/mimetypes.qrc");

            // after moc
            write_tracepoints(core);

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

            dbus.Private += "QT_BUILD_DBUS_LIB"_d;
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
            }
            else if (gui.getBuildSettings().TargetOS.Type == OSType::Macos)
            {
                gui += "platform/darwin/.*"_rr;
                gui += "rhi/qrhimetal.*"_rr;
            }
            else
            {
                //gui += "accessible/linux/.*"_rr;
                gui += "platform/unix/.*"_rr;
                gui += "text/freetype/.*"_rr;
                gui += "text/unix/.*"_rr;
                gui -= "platform/unix/qxkbcommon_3rdparty.cpp";
                gui -= "platform/unix/qxkbcommon.cpp";
                gui.CompileOptions.push_back("-msse4.1");
                gui.CompileOptions.push_back("-mavx");
                gui.CompileOptions.push_back("-mavx2");
                gui.CompileOptions.push_back("-march=native");
            }

            //gui.Public += "src"_id;
            gui.Private += "QT_BUILD_GUI_LIB"_d;

            gui.Public += "org.sw.demo.glennrp.png"_dep;
            gui += "org.sw.demo.mity.md4c"_dep;
            gui.Public += core;

            gui.Public -= dbus;
            if (gui.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                qt_gui_desc.config.public_.features.insert({ "xcb", false });
                qt_gui_desc.config.public_.features.insert({ "xcb_glx_plugin", false });
            }
            else
            {
                gui.Public += dbus;
                qt_gui_desc.config.public_.features.insert({ "xcb", true });
                qt_gui_desc.config.public_.features.insert({ "xcb_glx_plugin", true });
            }
            qt_gui_desc.print(gui);

            platform_files(gui);

            //gui.replaceInFileOnce("text/qharfbuzzng_p.h", "#include <harfbuzz/hb.h>", "#include <hb.h>");
            //gui.replaceInFileOnce("text/qfontengine.cpp", "#  include <harfbuzz/hb-ot.h>", "#include <hb-ot.h>");
            gui.replaceInFileOnce("text/qtextmarkdownimporter.cpp", "#include \"../../3rdparty/md4c/md4c.h\"", "#include <md4c/md4c.h>");

            auto mocs = automoc(moc, gui);
            SW_QT_ADD_MOC_DEPS(gui);
            ::rcc(rcc, gui, Files{ gui.SourceDir / "painting/qpdf.qrc", });

            // after moc
            write_tracepoints(gui);

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
                "accessible/.*"_rr,
                "dialogs/.*"_rr,
                "effects/.*"_rr,
                "graphicsview/.*"_rr,
                "itemviews/.*"_rr,
                "kernel/.*"_rr,
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

            if (network.getBuildSettings().TargetOS.Type == OSType::Windows)
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

            auto mocs = automoc(moc, widgets);
            SW_QT_ADD_MOC_DEPS(widgets);
            ::rcc(rcc, widgets, Files{ widgets.SourceDir / "styles/qstyle.qrc", widgets.SourceDir / "dialogs/qmessagebox.qrc" });
            ::uic(uic, widgets, widgets.SourceDir / "dialogs/qfiledialog.ui");

            // after moc
            write_tracepoints(widgets);

            widgets.patch("kernel/qwidgetsvariant.cpp", "qRegisterMetaType<QWidget*>();", "//qRegisterMetaType<QWidget*>();");
        }

        // network
        {
            common_setup(network);

            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, network, { "QtNetwork" });

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

            network.Private += "QT_BUILD_NETWORK_LIB"_d;
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

            network.Public += core;
            network.Public += "org.sw.demo.openssl.ssl"_dep;
            network.Public += "org.sw.demo.google.brotli"_dep;

            if (network.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                qt_network_desc.config.public_.features.insert({ "gssapi", false });
                qt_network_desc.config.private_.features.insert({ "sspi", true });
            }
            else
            {
                qt_network_desc.config.public_.features.insert({ "gssapi", false });
                qt_network_desc.config.public_.features.insert({ "linux_netlink", true });
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

            opengl.Private += "QT_BUILD_OPENGL_LIB"_d;

            auto mocs = automoc(moc, opengl);
            SW_QT_ADD_MOC_DEPS(opengl);

            // after moc
            write_tracepoints(opengl);

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

            xml.Private += "QT_BUILD_XML_LIB"_d;
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
                "kernel/.*"_rr,
                "models/.*"_rr
                ;

            sql.Private += "QT_BUILD_SQL_LIB"_d;
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
                //windows += "EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE=EGL_PLATFORM_ANGLE_DEVICE_TYPE_D3D_WARP_ANGLE"_d;
            }
            windows.Private += sw::Static, "QT_STATICPLUGIN"_d;

            //windows.Public += accessibility;
            //windows.Public += eventdispatchers;
            //windows.Public += themes;
            //windows.Public += windowsuiautomation;
            //windows.Public += fontdatabases_windows;

            automoc(moc, windows);
            ::rcc(rcc, windows, Files{
                windows.SourceDir / "src/plugins/platforms/windows/cursors.qrc",
                windows.SourceDir / "src/plugins/platforms/windows/openglblacklists.qrc" });

            //windows.Public += "org.sw.demo.google.angle.egl"_dep;
            windows.Public += wintab;
            windows.Public += iaccessible2;
            windows.Public += opengl;

            make_qt_plugin(windows, "QWindowsIntegrationPlugin");
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
            windowsvista.Private += sw::Static, "QT_STATICPLUGIN"_d;

            windowsvista.Public += widgets;

            automoc(moc, windowsvista);
            make_qt_plugin(windowsvista, "QWindowsVistaStylePlugin");
        }

        auto &winmain = base.addTarget<StaticLibraryTarget>("winmain");
        {
            winmain += "src/entrypoint/qtentrypoint_win.cpp";
            if (winmain.getBuildSettings().TargetOS.Type == OSType::Windows)
                winmain += "src/entrypoint/qtentrypoint_win.cpp";
            else {
                winmain.AutoDetectOptions = false;
                winmain.Empty = true;
            }
            //winmain.Public += core;
        }
        // alias?
        auto &entrypoint = base.addTarget<StaticLibraryTarget>("entrypoint");
        {
            entrypoint -= "src/entrypoint/qtentrypoint_win.cpp";
            if (entrypoint.getBuildSettings().TargetOS.Type == OSType::Windows)
                entrypoint += "src/entrypoint/qtentrypoint_win.cpp";
            else {
                entrypoint.AutoDetectOptions = false;
                entrypoint.Empty = true;
            }
            //winmain.Public += core;
        }

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

            printsupport.Private += "QT_BUILD_PRINTSUPPORT_LIB"_d;
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
            qt_printsupport_desc.print(printsupport);

            platform_files(printsupport);
            auto mocs = automoc(moc, printsupport);
            SW_QT_ADD_MOC_DEPS(printsupport);

            ::rcc(rcc, printsupport, printsupport.SourceDir / "dialogs/qprintdialog.qrc");

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

            plugins_printsupport_windows.Private += sw::Static, "QT_STATICPLUGIN"_d;
            plugins_printsupport_windows.Public += printsupport;
            automoc(moc, plugins_printsupport_windows);
            make_qt_plugin(plugins_printsupport_windows, "QWindowsPrinterSupportPlugin");
        }*/
        auto &plugins_printsupport_cups = plugins_printsupport.addTarget<LibraryTarget>("cups");
        {
            plugins_printsupport_cups.setOutputDir("plugins/printsupport");
            plugins_printsupport_cups +=
                "src/plugins/printsupport/cups/.*"_rr;
            plugins_printsupport_cups.Public +=
                "src/plugins/printsupport/cups"_id;

            plugins_printsupport_cups.Private += sw::Static, "QT_STATICPLUGIN"_d;
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
            common_setup(qml_models);
            String module = "QtQmlModels";
            auto sqt = syncqt("pub.egorpugin.primitives.tools.syncqt"_dep, qml_models, { module });
            qml_models.SourceDir /= "src/qmlmodels";

            qml_models += ".*"_r;
            qml_models += "QT_BUILD_QMLMODELS_LIB"_def;

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

            quick += "QT_BUILD_QUICK_LIB"_d;
            quick += "M_PI=3.14159265358979323846"_d;

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
        svg.Private += "QT_BUILD_SVG_LIB"_d;
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

    return;

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
