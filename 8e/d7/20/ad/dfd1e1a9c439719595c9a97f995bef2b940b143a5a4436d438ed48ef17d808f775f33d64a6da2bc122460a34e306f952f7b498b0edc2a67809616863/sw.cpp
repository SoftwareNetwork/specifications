void build(Solution &s)
{
    auto &icu = s.addProject("unicode.icu", "68.2.0");
    icu += RemoteFile("https://github.com/unicode-org/icu/releases/download/release-{M}-{m}/icu4c-{M}_{m}-src.tgz");

    auto &stub = icu.addDirectory("stub");
    auto &s_common = stub.addTarget<LibraryTarget>("common");

    auto setup = [](auto &t)
    {
        t.CPPVersion = CPPLanguageStandard::CPP11;
        t.Public += sw::Shared, "U_EXPORT=SW_EXPORT"_d;
        t.Public += sw::Shared, "U_IMPORT=SW_IMPORT"_d;
        t.Public += sw::Static, "U_STATIC_IMPLEMENTATION"_d;
        t.Public += "U_USING_ICU_NAMESPACE=1"_d;
        if (t.getCompilerType() == CompilerType::MSVC)
            t.CompileOptions.insert("-utf-8");
    };

    auto &s_data = stub.addTarget<LibraryTarget>("data");
    {
        setup(s_data);
        s_data += "source/stubdata/stubdata.cpp", "source"_id;

        auto dc = s_data.Public + s_common;
        dc->IncludeDirectoriesOnly = true;
    }

    // s_common
    {
        s_common.setRootDirectory("source");
        s_common.CPPVersion = CPPLanguageStandard::CPP11;
        s_common.setChecks("common");

        s_common +=
            "common/.*\\.cpp"_rr,
            "common/.*\\.h"_rr;
        s_common.Public += "common"_id;

        if (s_common.getBuildSettings().TargetOS.Type == OSType::Windows)
            s_common.Public += "Advapi32.lib"_slib;
        else
            s_common += "dl"_slib;

        setup(s_common);
        s_common.Private += "U_COMMON_IMPLEMENTATION"_d;
        s_common.Public += s_data;
    }

    //
    auto &s_i18n = stub.addTarget<LibraryTarget>("i18n");
    {
        s_i18n.setRootDirectory("source");
        setup(s_i18n);

        s_i18n +=
            "i18n/.*\\.cpp"_rr,
            "i18n/.*\\.h"_rr;
        s_i18n.Public += "i18n"_id;
        s_i18n.Private += "U_I18N_IMPLEMENTATION"_d;
        s_i18n.Public += s_common;
    }

    //
    auto &s_toolutil = stub.addTarget<LibraryTarget>("toolutil");
    {
        s_toolutil.setRootDirectory("source");
        setup(s_toolutil);

        s_toolutil +=
            "tools/toolutil/.*\\.cpp"_rr,
            "tools/toolutil/.*\\.h"_rr;

        s_toolutil.Public += "tools/toolutil"_id;
        s_toolutil.Private += "U_TOOLUTIL_IMPLEMENTATION"_d;
        s_toolutil.Public += s_i18n;
    }

    //
    auto &s_genccode = stub.addTarget<ExecutableTarget>("genccode");
    {
        s_genccode.setRootDirectory("source");
        s_genccode += "tools/genccode/.*\\.c"_rr;
        s_genccode.Public += s_toolutil;
    }

    //
    auto &common = icu.addTarget<LibraryTarget>("common");

    auto &data = icu.addTarget<LibraryTarget>("data");
    {
        data += RemoteFile("https://github.com/unicode-org/icu/releases/download/release-{M}-{m}/icu4c-{M}_{m}-src.zip");
        data.setRootDirectory("source");
        data += "data/in/.*\\.dat"_rr;
#if SW_CPP_DRIVER_API_VERSION >= 2
        if (data.getLinkerType() == LinkerType::MSVC)
            data.getRule("link").getArguments().push_back("/NOENTRY");
#else
        if (auto L = data.getSelectedTool()->as<VisualStudioLinker *>())
            L->NoEntry = true;
#endif
        {
            auto dc = data.Public + common;
            dc->IncludeDirectoriesOnly = true;
        }

        const auto name = "icudt" + data.Variables["PACKAGE_VERSION_MAJOR"].toString();
        const auto namel = name + "l";
        auto obj = data.BinaryDir / (namel + "_dat.");

        if (data.getBuildSettings().TargetOS.Type == OSType::Windows)
            obj += "obj";
        else
            obj += "c";

        if (data.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            auto d = data.addDummyDependency(s_genccode);
            d->getSettings() = data.getSettings(); // use the same settings for gencode
            auto c = data.addCommand();
            c << cmd::prog(d)
                << "--name" << namel << "-e" << name << "-o" << "-d" << obj.parent_path();
            if (data.getBuildSettings().Native.LibrariesType == LibraryType::Static)
                c << "--skip-dll-export";
            c
                << cmd::in(path("data") / "in" / (namel + ".dat"))
                << cmd::end() << cmd::out(obj)
                ;
        }
        else
        {
            data.ExportAllSymbols = true;

            auto in = data.SourceDir / "data" / "in" / (namel + ".dat");
            auto out = data.BinaryDir / "data" / "in" / (name + ".dat");
            if (data.getContext().getHostOs().is(OSType::Windows))
            {
                auto c = data.addCommand(SW_VISIBLE_BUILTIN_FUNCTION(copy_file));
                c << cmd::in(in) << cmd::out(out);
            }
            else
            {
                auto c1 = data.addCommand();
                c1 << cmd::prog("cp")
                    << cmd::in(in)
                    << cmd::out(out)
                    ;
            }

            obj = data.BinaryDir / (name + "_dat.c");
            auto c = data.addCommand();
            c << cmd::prog(s_genccode)
                << "-d" << obj.parent_path()
                << cmd::in(out)
                << cmd::end() << cmd::out(obj)
                ;
        }
    }

    // common
    {
        common.setRootDirectory("source");
        common.setChecks("common");

        common +=
            "common/.*\\.cpp"_rr,
            "common/.*\\.h"_rr;
        common.Public += "common"_id;

        if (common.getBuildSettings().TargetOS.Type == OSType::Windows)
            common.Public += "Advapi32.lib"_slib;
        else
            common += "dl"_slib;

        setup(common);
        common.Private += "U_COMMON_IMPLEMENTATION"_d;
        common.Public += data;
    }

    //
    auto &i18n = icu.addTarget<LibraryTarget>("i18n");
    {
        i18n.setRootDirectory("source");
        setup(i18n);

        i18n +=
            "i18n/.*\\.cpp"_rr,
            "i18n/.*\\.h"_rr;
        i18n.Public += "i18n"_id;
        i18n.Private += "U_I18N_IMPLEMENTATION"_d;
        i18n.Public += common;
    }

    //
    auto &io = icu.addTarget<LibraryTarget>("io");
    {
        io.setRootDirectory("source");
        setup(io);

        io +=
            "io/.*\\.cpp"_rr,
            "io/.*\\.h"_rr;
        io.Public += "io"_id;
        io.Private += "U_IO_IMPLEMENTATION"_d;
        io.Public += i18n;
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("common");
    s.checkFunctionExists("dlopen");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("nl_langinfo");
    s.checkFunctionExists("popen");
    s.checkFunctionExists("pthread_mutex_lock");
    s.checkFunctionExists("strtod_l");
    s.checkFunctionExists("tzset");
    s.checkFunctionExists("_tzset");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("elf.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("wchar.h");
    s.checkTypeSize("int16_t");
    s.checkTypeSize("int32_t");
    s.checkTypeSize("int64_t");
    s.checkTypeSize("int8_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("uint16_t");
    s.checkTypeSize("uint32_t");
    s.checkTypeSize("uint64_t");
    s.checkTypeSize("uint8_t");
    s.checkTypeSize("void *");
    s.checkTypeSize("wchar_t");
    s.checkLibraryFunctionExists("pthread", "pthread_attr_init");
    s.checkLibraryFunctionExists("pthread", "pthread_mutex_init");

    for (auto &check : s.all)
        check->Prefixes.insert("U_");
}
