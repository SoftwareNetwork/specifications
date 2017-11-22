void build(Solution &sln)
{
    auto &d = sln.addDirectory("demo");
    auto &icu = d.addProject("unicode.icu", "60.1.0");
    icu.Source = RemoteFile("http://download.icu-project.org/files/icu4c/60.1/icu4c-60_1-src.tgz");
    auto &stub = icu.addDirectory("stub");

    auto &s_data = stub.addTarget<LibraryTarget>("data");
    auto &s_common = stub.addTarget<LibraryTarget>("common");

    s_data.CPPVersion = CPPLanguageStandard::CPP11;
    s_data += "source/stubdata/stubdata.cpp", "source"_id;

    s_data.Public += "U_USING_ICU_NAMESPACE=1"_d;
    s_data.Public += sw::Shared, "U_EXPORT=SW_EXPORT"_d;
    s_data.Public += sw::Shared, "U_IMPORT=SW_IMPORT"_d;
    s_data.Public += sw::Static, "U_STATIC_IMPLEMENTATION"_d;

    auto dc = s_data.Public + s_common;
    dc->IncludeDirectoriesOnly = true;

    //
    s_common.setRootDirectory("source");
    s_common.CPPVersion = CPPLanguageStandard::CPP11;
    s_common.setChecks("common");

    s_common +=
        "common/.*\\.c"_rr,
        "common/.*\\.cpp"_rr,
        "common/.*\\.h"_rr;
    s_common.Public += "common"_id;

    if (sln.Settings.TargetOS.Type == OSType::Windows)
        s_common += "Advapi32.lib"_l;

    s_common.Private += "U_COMMON_IMPLEMENTATION"_d;
    s_common.Public += "U_USING_ICU_NAMESPACE=1"_d;
    s_common.Public += sw::Shared, "U_EXPORT=SW_EXPORT"_d;
    s_common.Public += sw::Shared, "U_IMPORT=SW_IMPORT"_d;
    s_common.Public += sw::Static, "U_STATIC_IMPLEMENTATION"_d;

    s_common.Public += s_data;

    //
    auto &s_i18n = stub.addTarget<LibraryTarget>("i18n");
    s_i18n.setRootDirectory("source");
    s_i18n.CPPVersion = CPPLanguageStandard::CPP11;

    s_i18n +=
        "i18n/.*\\.c"_rr,
        "i18n/.*\\.cpp"_rr,
        "i18n/.*\\.h"_rr;
    s_i18n.Public += "i18n"_id;
    s_i18n.Private += "U_I18N_IMPLEMENTATION"_d;
    s_i18n.Public += s_common;

    //
    auto &s_toolutil = stub.addTarget<LibraryTarget>("toolutil");
    s_toolutil.setRootDirectory("source");
    s_toolutil.CPPVersion = CPPLanguageStandard::CPP11;

    s_toolutil +=
        "tools/toolutil/.*\\.c"_rr,
        "tools/toolutil/.*\\.cpp"_rr,
        "tools/toolutil/.*\\.h"_rr;

    s_toolutil.Public += "tools/toolutil"_id;
    s_toolutil.Private += "U_TOOLUTIL_IMPLEMENTATION"_d;
    s_toolutil.Public += s_i18n;

    //
    auto &s_genccode = stub.addTarget<ExecutableTarget>("genccode");
    s_genccode.setRootDirectory("source");
    s_genccode += "tools/genccode/.*\\.c"_rr;
    s_genccode.Public += s_toolutil;

    //
    auto &data = icu.addTarget<LibraryTarget>("data");
    data.setRootDirectory("source");
    data += "data/in/.*\\.dat"_rr;
    auto dg = data + s_genccode;
    dg->Dummy = true;

    const auto name = "icudt" + data.Variables["PACKAGE_VERSION_MAJOR"] + "l";
    auto obj = data.BinaryDir / (name + "_dat.");
    const auto in = data.SourceDir / "data" / "in" / (name + ".dat");

    if (sln.Settings.TargetOS.Type == OSType::Windows)
        obj += "obj";
    else
        obj += "o";

    auto c2 = std::make_shared<Command>();
    c2->program = s_genccode.getOutputFile();
    c2->args = { "-e", name.substr(0, name.size() - 1), "-o", in.string() };
    c2->working_directory = data.BinaryDir;
    c2->addInput(in);
    c2->addOutput(obj);

    data += obj;

    if (auto L = data.getSelectedTool()->as<VisualStudioLinker>())
        L->NoEntry = true;

    //
    auto &common = icu.addTarget<LibraryTarget>("common");
    common.setRootDirectory("source");
    common.CPPVersion = CPPLanguageStandard::CPP11;
    common.setChecks("common");

    common +=
        "common/.*\\.c"_rr,
        "common/.*\\.cpp"_rr,
        "common/.*\\.h"_rr;
    common.Public += "common"_id;

    if (sln.Settings.TargetOS.Type == OSType::Windows)
        common += "Advapi32.lib"_l;

    common.Private += "U_COMMON_IMPLEMENTATION"_d;
    common.Public += "U_USING_ICU_NAMESPACE=1"_d;
    common.Public += sw::Shared, "U_EXPORT=SW_EXPORT"_d;
    common.Public += sw::Shared, "U_IMPORT=SW_IMPORT"_d;
    common.Public += sw::Static, "U_STATIC_IMPLEMENTATION"_d;

    common.Public += data;

    //
    auto &i18n = icu.addTarget<LibraryTarget>("i18n");
    i18n.setRootDirectory("source");
    i18n.CPPVersion = CPPLanguageStandard::CPP11;

    i18n +=
        "i18n/.*\\.c"_rr,
        "i18n/.*\\.cpp"_rr,
        "i18n/.*\\.h"_rr;
    i18n.Public += "i18n"_id;
    i18n.Private += "U_I18N_IMPLEMENTATION"_d;
    i18n.Public += common;

    //
    auto &io = icu.addTarget<LibraryTarget>("io");
    io.setRootDirectory("source");
    io.CPPVersion = CPPLanguageStandard::CPP11;

    io +=
        "io/.*\\.c"_rr,
        "io/.*\\.cpp"_rr,
        "io/.*\\.h"_rr;
    io.Public += "io"_id;
    io.Private += "U_IO_IMPLEMENTATION"_d;
    io.Public += i18n;
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

    for (auto &[s, check] : s.checks)
        check->Prefixes.insert("U_");
}
