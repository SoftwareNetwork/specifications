#pragma sw require header org.sw.demo.ragel
#pragma sw require header org.sw.demo.lexxmark.winflexbison.bison

#pragma sw header on

static void gen_stamp(const DependencyPtr &tools_stamp_gen, NativeExecutedTarget &t)
{
    auto c = t.addCommand();
    c.c->always = true;
    c << cmd::prog(tools_stamp_gen)
        << cmd::std_out(t.BinaryPrivateDir / "stamp.h.in")
        ;
}

static void gen_sqlite2cpp(const DependencyPtr &tools_sqlite2cpp, NativeExecutedTarget &t, const path &sql_file, const path &out_file, const String &ns)
{
    auto c = t.addCommand();
    c << cmd::prog(tools_sqlite2cpp)
        << cmd::in(sql_file)
        << cmd::out(out_file)
        << ns;
}

static void embed(const DependencyPtr &embedder, NativeExecutedTarget &t, const path &in)
{
    struct EmbedCommand : ::sw::driver::Command
    {
        using Command::Command;

        std::shared_ptr<Command> clone() const override
        {
            return std::make_shared<EmbedCommand>(*this);
        }

    private:
        void postProcess1(bool ok) override
        {
            for (auto &line : split_lines(out.text))
            {
                static const auto prefix = "embedding: "s;
                boost::trim(line);
                addImplicitInput(line.substr(prefix.size()));
                //for (auto &f : outputs)
                    //File(f, *fs).addImplicitDependency(line.substr(prefix.size()));
            }
        }
    };

    if (in.is_absolute())
        throw SW_RUNTIME_ERROR("embed: in must be relative to SourceDir");

    auto f = t.SourceDir / in;
    auto out = t.BinaryDir / in.parent_path() / in.filename().stem();

    auto c = t.addCommand();
    c.c = std::make_shared<EmbedCommand>(c.c->getContext());
    SW_INTERNAL_ADD_COMMAND(c.c, t);
    c << cmd::prog(embedder)
        << cmd::wdir(f.parent_path())
        << cmd::in(in)
        << cmd::out(out)
        ;
    t += IncludeDirectory(out.parent_path()); // but remove this later
}

// compress_xz = 0x1
static void embed2(const DependencyPtr &embedder, NativeExecutedTarget &t, path in, path out = {}, int compress = 0)
{
    if (t.DryRun)
        return;

    if (in.is_absolute() && out.empty())
        throw SW_RUNTIME_ERROR("provide out file for absolute input");
    auto pp = in.parent_path();
    t.check_absolute(in);
    if (out.empty())
        out = t.BinaryPrivateDir / pp / in.filename() += ".emb";

    t.addCommand()
        << cmd::prog(embedder)
        << cmd::in(in, cmd::Skip)
        << cmd::out(out)
        << compress
        ;
}

static Files syncqt(const DependencyPtr &sqt, NativeExecutedTarget &t, const Strings &modules)
{
    if (t.DryRun)
        return {};

    Files out;
    auto i = t.BinaryDir / "include";
    auto v = t.getPackage().getVersion().toString();
    for (auto &m : modules)
    {
        fs::create_directories(i / m / v / m);

        auto c = t.addCommand();
        c << cmd::prog(sqt)
            << "-s" << t.SourceDir
            << "-b" << t.BinaryDir
            << "-m" << m
            << "-v" << v
            << cmd::end()
            << cmd::out(i / m / m)
            ;
        c.c->strict_order = 1; // run before moc
        out.insert(i / m / m);
        //t.Interface += i / m / m; makes cyclic deps

        t.Public += IncludeDirectory(i);
        t.Public += IncludeDirectory(i / m);
        t.Public += IncludeDirectory(i / m / v);
        t.Public += IncludeDirectory(i / m / v / m);
    }
    return out;
}

static void generate_cl(const DependencyPtr &clgen, NativeExecutedTarget &t, const path &fn, const String &type)
{
    auto outh = t.BinaryDir / (fn.stem().u8string() + "." + type + ".h");
    auto outcpp = t.BinaryDir / (fn.stem().u8string() + "." + type + ".cpp");

    auto c = t.addCommand();
    c << cmd::prog(clgen)
        << cmd::in(fn)
        << cmd::out(outh)
        << cmd::out(outcpp)
        << type
        ;
}

static void create_git_revision(const DependencyPtr &create_git_rev, NativeExecutedTarget &t)
{
    auto c = t.addCommand();
    c << cmd::prog(create_git_rev)
        << sw::resolveExecutable("git")
        << t.SourceDir
        << cmd::out(t.BinaryPrivateDir / "gitrev.h");
    c.c->always = true;

    auto u = create_git_rev->getPackage();
    sw::UnresolvedPackage up{ u.getPath().parent().parent() / "git_rev", u.getRange() };
    t += std::make_shared<sw::Dependency>(up);
}

#pragma sw header off

void configure(Build &s)
{
    //if (s.isConfigSelected("cyg2mac"))
        //s.loadModule("utils/cc/cygwin2macos.cpp").call<void(Solution&)>("configure", s);
}

void build(Solution &s)
{
    auto &p = s.addProject("primitives", "master");
    p += "https://github.com/egorpugin/primitives"_git;

    auto setup_primitives_no_all_sources = [](auto &t)
    {
        path p = "src";
        auto n = t.getPackage().getPath().slice(t.getPackage().getPath().isAbsolute() ? 3 : 1);
        auto n2 = n;
        while (!n.empty())
        {
            p /= n.front();
            n = n.slice(1);
        }
        t.ApiName = "PRIMITIVES_" + boost::to_upper_copy(n2.toString("_")) + "_API";
        t += cpp17;
        t.PackageDefinitions = true;
        // not all code works with this yet (e.g. hh.date)
        //t.Public.CompileOptions.push_back("-Zc:__cplusplus");
        if (t.getCompilerType() != CompilerType::MSVC)
        {
            //t.CompileOptions.push_back("-Werror");
            t.CompileOptions.push_back("-Wall");
            t.CompileOptions.push_back("-Wextra");
        }
        else
        {
            t.Public += "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS"_def;
            t.Public += "_CRT_SECURE_NO_WARNINGS"_def;
            t.Public.CompileOptions.push_back("-wd4251");
        }
        return p;
    };

    auto setup_primitives = [&setup_primitives_no_all_sources](auto &t)
    {
        auto p = setup_primitives_no_all_sources(t);
        t.setRootDirectory(p);
        // explicit!
        t += "include/.*"_rr;
        t += "src/.*"_rr;
    };

#define ADD_LIBRARY_WITH_NAME(var, name)          \
    auto &var = p.addTarget<LibraryTarget>(name); \
    setup_primitives(var)

#define ADD_LIBRARY(x) ADD_LIBRARY_WITH_NAME(x, #x)

    ADD_LIBRARY(error_handling);

    auto &templates = p.addTarget<StaticLibraryTarget>("templates");
    setup_primitives(templates);
    templates -= "org.sw.demo.boost.stacktrace"_dep;
    if (templates.getBuildSettings().Native.ConfigurationType != ConfigurationType::Release &&
        templates.getBuildSettings().Native.ConfigurationType != ConfigurationType::MinimalSizeRelease
        )
    {
        templates += "USE_STACKTRACE"_def;
        templates += "org.sw.demo.boost.stacktrace"_dep;
    }
    if (templates.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        templates += "dbgeng.lib"_slib;
        templates += "Ole32.lib"_slib;
    }
    else
    {
        templates += "dl"_slib;
    }

    ADD_LIBRARY(string);
    string.Public += "org.sw.demo.boost.algorithm"_dep;

    ADD_LIBRARY(filesystem);
    filesystem.Public += string, templates,
        "org.sw.demo.boost.filesystem"_dep,
        "org.sw.demo.boost.thread"_dep
        ;
    if (filesystem.getBuildSettings().TargetOS.Type != OSType::Windows)
        filesystem += "m"_slib;

    ADD_LIBRARY(file_monitor);
    file_monitor.Public += filesystem,
        "pub.egorpugin.libuv"_dep;

    ADD_LIBRARY(emitter);
    emitter.Public += filesystem;

    ADD_LIBRARY(executor);
    executor.Public += templates,
        "org.sw.demo.boost.asio"_dep,
        "org.sw.demo.boost.system"_dep;
    if (executor.getBuildSettings().TargetOS.Type != OSType::Windows)
        executor += "pthread"_slib;

    ADD_LIBRARY(command);
    command.Public += file_monitor,
        "org.sw.demo.boost.process"_dep;

    ADD_LIBRARY(date_time);
    date_time.Public += string,
        "org.sw.demo.boost.date_time"_dep;

    ADD_LIBRARY(lock);
    lock.Public += filesystem,
        "org.sw.demo.boost.interprocess"_dep;
    if (lock.getBuildSettings().TargetOS.Type == OSType::Windows)
        lock += "Advapi32.lib"_slib;

    ADD_LIBRARY(log);
    log.Public += "org.sw.demo.boost.log"_dep;

    ADD_LIBRARY(grpc_helpers);
    grpc_helpers.Public += "org.sw.demo.google.grpc.cpp"_dep, templates, log;

    ADD_LIBRARY(cron);
    cron.Public += executor, log;

    ADD_LIBRARY(yaml);
    yaml.Public += string, templates,
        "org.sw.demo.jbeder.yaml_cpp-master"_dep;

    ADD_LIBRARY(pack);
    pack.Public += filesystem, templates,
        "org.sw.demo.libarchive.libarchive"_dep;

    ADD_LIBRARY(patch);
    patch.Public += filesystem, templates;

    ADD_LIBRARY(http);
    http.Public += filesystem, templates,
        "org.sw.demo.badger.curl.libcurl-7"_dep;
    if (http.getBuildSettings().TargetOS.Type == OSType::Windows)
        http += "Winhttp.lib"_slib;

    ADD_LIBRARY(hash);
    hash.Public += filesystem,
        "org.sw.demo.aleksey14.rhash"_dep,
        "org.sw.demo.openssl.crypto"_dep;

    ADD_LIBRARY(win32helpers);
    if (!win32helpers.getBuildSettings().TargetOS.is(OSType::Windows))
        win32helpers.DryRun = true;
    win32helpers.Public += "BOOST_DLL_USE_STD_FS"_def;
    win32helpers.Public += filesystem,
        "org.sw.demo.boost.dll"_dep,
        "org.sw.demo.boost.algorithm"_dep;
    if (win32helpers.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        win32helpers.Public += "UNICODE"_d;
        win32helpers += "Shell32.lib"_slib, "Ole32.lib"_slib, "Advapi32.lib"_slib, "user32.lib"_slib, "uuid.lib"_slib;
    }

    ADD_LIBRARY_WITH_NAME(db_common, "db.common");
    db_common.Public += filesystem, templates,
        "org.sw.demo.imageworks.pystring"_dep;

    ADD_LIBRARY_WITH_NAME(db_sqlite3, "db.sqlite3");
    db_sqlite3.Public += db_common, "org.sw.demo.sqlite3"_dep;

    ADD_LIBRARY_WITH_NAME(db_postgresql, "db.postgresql");
    db_postgresql.Public += db_common, "org.sw.demo.jtv.pqxx-*"_dep;

    auto &main = p.addTarget<StaticLibraryTarget>("main");
    setup_primitives(main);
    main.Public += error_handling;

    auto &settings = p.addTarget<StaticLibraryTarget>("settings");
    setup_primitives(settings);
    settings.Public += "include"_idir;
    settings.Public += yaml, filesystem, templates,
        "pub.egorpugin.llvm_project.llvm.support_lite-master"_dep;
    gen_flex_bison_pair("org.sw.demo.lexxmark.winflexbison"_dep, settings, "LALR1_CPP_VARIANT_PARSER", "src/settings");
    gen_flex_bison_pair("org.sw.demo.lexxmark.winflexbison"_dep, settings, "LALR1_CPP_VARIANT_PARSER", "src/path");
    if (settings.getBuildSettings().TargetOS.Type != OSType::Windows)
        settings += "dl"_slib;

    ADD_LIBRARY(symbol);
    symbol.Public += "BOOST_DLL_USE_STD_FS"_def;
    symbol.Public += filesystem, "org.sw.demo.boost.dll"_dep;

    ADD_LIBRARY(xml);
    xml.Public += string, templates,
        "org.sw.demo.xmlsoft.libxml2"_dep;

    auto &sw_settings = p.addTarget<StaticLibraryTarget>("sw.settings");
    setup_primitives(sw_settings);
    sw_settings.Public += settings;
    sw_settings -= "src/sw.settings.program_name.cpp";
    //sw_settings.Interface += "src/sw.settings.program_name.cpp";
    if (sw_settings.getBuildSettings().TargetOS.Type != OSType::Windows)
        sw_settings += "dl"_slib;

    ADD_LIBRARY(version);
    version.Public += "include"_idir;
    version.Public += "src/version.natvis";
    version.Public += string, templates,
        "org.sw.demo.fmt-*"_dep,
        "org.sw.demo.boost.container_hash"_dep,
        "org.sw.demo.imageworks.pystring"_dep;
    gen_ragel("org.sw.demo.ragel-6"_dep, version, "src/version.rl");
    gen_flex_bison_pair("org.sw.demo.lexxmark.winflexbison"_dep, version, "GLR_CPP_PARSER", "src/range");

    ADD_LIBRARY(source);
    source.Public += command, hash, http, pack, version, yaml,
        "org.sw.demo.nlohmann.json"_dep;

    // experimental
    /*ADD_LIBRARY_WITH_NAME(cl, "experimental.cl");
    cl.Public += templates, string;
    gen_flex_bison_pair("org.sw.demo.lexxmark.winflexbison"_dep, cl, "GLR_CPP_PARSER", "src/cl");*/

    //ADD_LIBRARY(object_path);
    //object_path.Public += string, templates;

    //
    auto &cl_generator = p.addTarget<ExecutableTarget>("tools.cl_generator");
    setup_primitives_no_all_sources(cl_generator);
    cl_generator += "src/tools/cl_generator.cpp";
    cl_generator += filesystem, yaml, emitter, main;
    //

    auto &sw_main = p.addTarget<StaticLibraryTarget>("sw.main");
    {
        setup_primitives(sw_main);
        sw_main.Public += "BOOST_DLL_USE_STD_FS"_def;
        sw_main.Public += main, sw_settings,
            "org.sw.demo.boost.dll"_dep;
        //sw_main.Interface.LinkLibraries.push_back(main.getImportLibrary()); // main itself
        //sw_main.Interface.LinkLibraries.push_back(sw_main.getImportLibrary()); // then me (self, sw.main)
        //sw_main.Interface.LinkLibraries.push_back(sw_settings.getImportLibrary()); // then sw.settings
        if (sw_main.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            sw_main.Public +=
                "org.sw.demo.google.breakpad.client.windows.handler-master"_dep,
                "org.sw.demo.google.breakpad.client.windows.crash_generation.client-master"_dep,
                "org.sw.demo.google.breakpad.client.windows.crash_generation.server-master"_dep
                ;
        }
        generate_cl(std::make_shared<Dependency>(cl_generator), sw_main, "src/cl.yml", "llvm");
    }

    ADD_LIBRARY(wt);
    wt.Public += string, "org.sw.demo.emweb.wt.wt"_dep;

    // tools

    auto &tools_embedder = p.addTarget<ExecutableTarget>("tools.embedder");
    setup_primitives_no_all_sources(tools_embedder);
    tools_embedder += "src/tools/embedder.cpp";
    tools_embedder += filesystem, sw_main;

    auto &tools_embedder2 = p.addTarget<ExecutableTarget>("tools.embedder2");
    setup_primitives_no_all_sources(tools_embedder2);
    tools_embedder2 += "src/tools/embedder2.cpp";
    tools_embedder2 += filesystem, sw_main;
    tools_embedder2 += "org.sw.demo.xz_utils.lzma"_dep;

    auto &tools_sqlite2cpp = p.addTarget<ExecutableTarget>("tools.sqlpp11.sqlite2cpp");
    setup_primitives_no_all_sources(tools_sqlite2cpp);
    tools_sqlite2cpp += "src/tools/sqlpp11.sqlite2cpp.cpp";
    tools_sqlite2cpp += filesystem, emitter, sw_main, "org.sw.demo.sqlite3"_dep;

    auto &tools_syncqt = p.addTarget<ExecutableTarget>("tools.syncqt");
    setup_primitives_no_all_sources(tools_syncqt);
    tools_syncqt += "src/tools/syncqt.cpp";
    tools_syncqt += filesystem, sw_main;

    auto &stamp_gen = p.addTarget<ExecutableTarget>("tools.stamp_gen");
    setup_primitives_no_all_sources(stamp_gen);
    stamp_gen += "src/tools/stamp_gen.cpp";

    {
        auto &git_rev = p.addTarget<StaticLibraryTarget>("git_rev");
        auto r = setup_primitives_no_all_sources(git_rev);
        git_rev.setRootDirectory(r);
        git_rev -= ".*"_rr;
        git_rev.Interface += "src/.*"_rr;

        auto &create_git_rev = p.addTarget<ExecutableTarget>("tools.create_git_rev");
        setup_primitives_no_all_sources(create_git_rev);
        create_git_rev += "src/tools/create_git_rev.cpp";
        create_git_rev += command, sw_main;
    }

    auto &test = p.addDirectory("test");
    test.Scope = TargetScope::Test;

    auto add_test = [&test, &s, &sw_main](const String &name) -> decltype(auto)
    {
        auto &t = test.addTarget<ExecutableTarget>(name);
        t += cpp17;
        t += path("src/" + name + ".cpp");
        t += "org.sw.demo.catchorg.catch2-*"_dep;
        if (t.getCompilerType() == CompilerType::MSVC)
            t.CompileOptions.push_back("-bigobj");
        //else if (t.getCompilerType() == CompilerType::GNU)
            //t.CompileOptions.push_back("-Wa,-mbig-obj");
        t += sw_main;
        return t;
    };

    auto &test_main = add_test("main");
    if (test_main.getCompilerType() == CompilerType::MSVC)
        test_main.CompileOptions.push_back("/utf-8"); // path tests
    test_main += command, date_time,
        executor, hash, yaml, emitter, http,
        "org.sw.demo.nlohmann.json-*"_dep;

    auto &test_db = add_test("db");
    test_db += command, db_sqlite3, db_postgresql, date_time,
        executor, hash, yaml;

    auto &test_settings = add_test("settings");
    test_settings.PackageDefinitions = true;
    test_settings += settings;

    auto &test_version = add_test("version");
    test_version += version;

    auto &test_source = add_test("source");
    test_source += source;

    auto &test_patch = add_test("patch");
    test_patch += patch;

    /*auto &test_cl = add_test("cl");
    test_cl += cl;*/

    auto tm = test_main.addTest(test_main);
    tm.c->addPathDirectory(getenv("PATH"));
    /*auto tdb = s.addTest(test_db);
    if (test_db.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        tdb.c->addLazyAction([&s, c = tdb.c]
        {
            auto &pq = s.getTarget<LibraryTarget>(PackageId{"org.sw.demo.find.libpq", "master"});
            c->addPathDirectory(pq.LinkDirectories.begin()->parent_path() / "bin");
        });
    }*/
    patch.addTest(test_patch);
    settings.addTest(test_settings);
    version.addTest(test_version);
}
