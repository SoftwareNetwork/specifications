#pragma once

//#include <iostream>
#include <regex>

// http://doc.qt.io/qt-5/rcc.html
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

    auto outfilename = "qmake_" + d.name;
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

// http://doc.qt.io/qt-5/uic.html
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

    t.addRuleDependency("moc", moc);
    qt_rcc(rcc, t);
    qt_uic(uic, t);
}

// http://doc.qt.io/qt-5/moc.html
// detailed moc detection
// https://github.com/qt/qtbase/blob/dev/qmake/generators/makefiledeps.cpp#L894
// also see cmake automoc
// https://cmake.org/cmake/help/latest/prop_tgt/AUTOMOC.html
struct MocRule : sw::NativeRule
{
    ExecutableTarget &exe;
    Files mocked;
    Files used_files;

    MocRule(RuleProgram p, ExecutableTarget &exe)
        : NativeRule(std::move(p)), exe(exe)
    {
    }

    sw::IRulePtr clone() const override { return std::make_unique<MocRule>(*this); }

    void setup(const Target &t) override
    {
        auto c = getProgram().getCommand();

        c->setProgram(getProgram().file);
        exe.setupCommand(*c);

        c->use_response_files = true;
        c->protect_args_with_quotes = false;

        auto nt = t.as<NativeCompiledTarget *>();
        if (!nt)
            return;
        nt->getMergeObject().addDefinitionsAndIncludeDirectories(*c);
    }

    void addInputs(const Target &t, sw::RuleFiles &rfs) override
    {
        static Strings HeaderFileExtensions
        {
            ".h",
            ".hh",
            ".h++",
            ".hm",
            ".hpp",
            ".hxx",
            //".in",
            ".txx",
        };
        static StringSet HeaderFileExtensionsSet(HeaderFileExtensions.begin(), HeaderFileExtensions.end());

        auto nt = t.as<NativeCompiledTarget *>();
        if (!nt)
            return;

        sw::RuleFiles rfs_new;
        std::unordered_map<String, path> included;
        for (auto &[p, rf] : rfs)
        {
            if (p.extension() != ".cpp")
                continue;
            if (mocked.contains(p))
                continue;
            if (used_files.contains(p))
                continue;
            used_files.insert(p);

            if (File(p, t.getFs()).isGenerated() && !fs::exists(p))
                continue;

            static std::regex r(
                "(^|\n)[ \t]*#[ \t]*include[ \t]+"
                "[\"<](([^ \">]+/)?moc_[^ \">/]+\\.cpp|[^ \">]+\\.moc)[\">]");
            std::smatch m;
            auto s = read_file(p);
            while (std::regex_search(s, m, r))
            {
                auto inc_string = m[2].str();
                if (inc_string.find("moc_") == 0)
                {
                    // in this branch we just record info that file is included into cpp
                    auto fn = inc_string.substr(4);
                    // remove .cpp ext
                    included[fn.substr(0, fn.size() - 4)] = p;
                }
                else
                {
                    auto c = getProgram().clone()->getCommand();
                    c->setContext(t.getMainBuild());

                    c->push_back(p);
                    c->addInput(p);
                    c->push_back("-i"); // from cpp
                    c->push_back("-o");
                    auto o = t.BinaryPrivateDir / inc_string;
                    if (!rfs.contains(o))
                    {
                        c->push_back(o);
                        c->addOutput(o);
                        rf.addDependency(o);
                        auto &rf = rfs_new.addFile(o);
                        rf.setCommand(c);
                    }
                }
                s = m.suffix().str();
            }
        }

        for (auto &[p, rf] : rfs)
        {
            auto ext = p.extension().string();
            if (HeaderFileExtensionsSet.find(ext) == HeaderFileExtensionsSet.end())
                continue;
            if (mocked.contains(p))
                continue;
            if (used_files.contains(p))
                continue;
            used_files.insert(p);

            if (File(p, t.getFs()).isGenerated() && !fs::exists(p))
                continue;

            auto s = read_file(p);
            if (0
                || s.find("Q_OBJECT") != s.npos
                || s.find("Q_GADGET") != s.npos
                || s.find("Q_NAMESPACE") != s.npos
                // cmake does not include this one
                //|| s.find("Q_NAMESPACE_EXPORT") != s.npos
                )
            {
                auto c = getProgram().clone()->getCommand();
                c->setContext(t.getMainBuild());

                c->push_back(p);
                c->addInput(p);
                c->push_back("-o");
                auto o = t.BinaryPrivateDir;
                auto build = !included.contains(p.stem().string());
                if (build)
                    o /= "moc"; // make hash
                o /= "moc_" + p.stem().string() + ".cpp";
                if (rfs.contains(o))
                    continue;
                c->push_back(o);
                c->addOutput(o);
                if (build)
                {
                    auto &rf = rfs_new.addFile(o);
                    rf.setCommand(c);
                }
                else
                {
                    auto fn = included[p.stem().string()];
                    auto &rf = (rfs.contains(fn) ? rfs : rfs_new).addFile(fn);
                    rf.addDependency(o);
                    rfs.addCommand(o, c);
                }
                mocked.insert(o);
            }
        }
        rfs.merge(rfs_new);
    }

    //sw::Commands getCommands() const override { return commands; }

protected:
    //sw::Commands commands;
};

struct MocCompiler : sw::CompilerBaseProgram
{
    std::unique_ptr<Program> clone() const override
    {
        return std::make_unique<MocCompiler>(*this);
    }
    void prepareCommand1(const Target &t) {}
};
