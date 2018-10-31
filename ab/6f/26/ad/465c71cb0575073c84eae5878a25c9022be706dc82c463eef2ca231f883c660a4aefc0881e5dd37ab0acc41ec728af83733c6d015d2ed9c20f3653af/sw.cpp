#pragma sw require header org.sw.demo.re2c.re2c-1

void build(Solution &s)
{
    auto &yasm = s.addExecutable("yasm", "master");
    yasm += Git("https://github.com/yasm/yasm", "", "{v}");

    auto &cfg = yasm.addLibrary("config");
    cfg += ".*\\.h"_rr;
    cfg += "libyasm-stdint.h.cmake";
    cfg.configureFile("libyasm-stdint.h.cmake", "libyasm-stdint.h");

    auto setup = [&cfg](auto &t)
    {
        t.PackageDefinitions = true;
        t.setChecks("libyasm");
        t -= ".*/tests/.*"_rr;
        t += cfg;
    };

    StringMap<StringSet> yasm_modules;
    std::vector<NativeExecutedTarget*> deps;
    auto &modules = yasm.addDirectory("modules");
    auto add_modules_child = [&modules, &setup, &deps, &yasm_modules](const String &m, const String &n) -> decltype(auto)
    {
        auto &t = modules.addStaticLibrary(m + "." + n);
        t += FileRegex("modules/" + m + "/" + n + "/.*", true);
        setup(t);
        deps.push_back(&t);
        yasm_modules[m.back() == 's' ? m.substr(0, m.size() - 1) : m].insert(n);
        return t;
    };

    auto &tools = yasm.addDirectory("tools");
    auto &genmacro = tools.addExecutable("genmacro");
    genmacro += "tools/genmacro/genmacro.c";
    auto gen_macro = [&genmacro](auto &t, const path &in, const path &out, const String &var)
    {
        auto c = t.addCommand();
        c << cmd::prog(genmacro)
            << cmd::out(out)
            << var
            << cmd::in(in)
            ;
        t[out].skip = true;
    };

    auto &genversion = tools.addExecutable("genversion");
    genversion += "modules/preprocs/nasm/genversion.c";
    genversion.writeFileOnce(genversion.BinaryPrivateDir / "config.h",
        "#define PACKAGE_VERSION \"1.13.0\"");

    auto &preprocs = modules.addDirectory("preprocs");
    auto add_preproc = [&add_modules_child](const String &n) -> decltype(auto)
    {
        return add_modules_child("preprocs", n);
    };

    auto &pp_cpp = add_preproc("cpp");
    pp_cpp += "CPP_PROG=\"cpp\""_def;

    auto &pp_raw = add_preproc("raw");
    auto &pp_gas = add_preproc("gas");

    auto &pp_nasm = add_preproc("nasm");
    {
        pp_nasm -= "modules/preprocs/nasm/genversion.c";

        {
            auto c = pp_nasm.addCommand();
            c << cmd::prog(genversion)
                << cmd::out("version.mac");
        }

        gen_macro(pp_nasm, "version.mac", "nasm-version.c", "nasm_version_mac");
    }

    auto &parsers = modules.addDirectory("parsers");

    auto &pa_gas = add_modules_child("parsers", "gas");
    re2c(pa_gas, "modules/parsers/gas/gas-token.re", "c");

    auto &pa_nasm = add_modules_child("parsers", "nasm");
    re2c(pa_nasm, "modules/parsers/nasm/nasm-token.re", "c");
    gen_macro(pa_nasm, "modules/parsers/nasm/nasm-std.mac", "nasm-macros.c", "nasm_standard_mac");

    auto &objfmts = modules.addDirectory("objfmts");
    auto add_objfmt = [&add_modules_child](const String &n) -> decltype(auto)
    {
        return add_modules_child("objfmts", n);
    };

    auto &ob_bin = add_objfmt("bin");
    auto &ob_dbg = add_objfmt("dbg");
    auto &ob_elf = add_objfmt("elf");
    auto &ob_macho = add_objfmt("macho");
    auto &ob_rdf = add_objfmt("rdf");
    auto &ob_xdf = add_objfmt("xdf");

    auto &ob_coff = add_objfmt("coff");
    gen_macro(ob_coff, "modules/objfmts/coff/win64-nasm.mac", "win64-nasm.c", "win64_nasm_stdmac");
    gen_macro(ob_coff, "modules/objfmts/coff/win64-gas.mac", "win64-gas.c", "win64_gas_stdmac");

    auto &lf_nasm = add_modules_child("listfmts", "nasm");
    auto &dbg_codeview = add_modules_child("dbgfmts", "codeview");
    auto &dbg_dwarf2 = add_modules_child("dbgfmts", "dwarf2");
    auto &dbg_null = add_modules_child("dbgfmts", "null");
    auto &dbg_stabs = add_modules_child("dbgfmts", "stabs");

    auto &a_lc3b = add_modules_child("arch", "lc3b");
    re2c(pa_nasm, "modules/arch/lc3b/lc3bid.re", "c");

    auto &a_x86 = add_modules_child("arch", "x86");
    {
        for (auto f : { "modules/arch/x86/x86cpu.gperf", "modules/arch/x86/x86regtmod.gperf" })
        {
            a_x86.pushFrontToFileOnce(f, "/*");
            a_x86.replaceInFileOnce(f, "%{", "*/\n%{");
        }

        auto gperf = [&a_x86](const path &in, const path &out)
        {
            auto c = a_x86.addCommand();
            c << cmd::prog("org.sw.demo.gnu.gperf-3"_dep)
                << cmd::std_out(out)
                << cmd::in(in)
                ;
        };

        gperf("modules/arch/x86/x86cpu.gperf", "x86cpu.c");
        gperf("modules/arch/x86/x86regtmod.gperf", "x86regtmod.c");

        {
            auto c = a_x86.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << cmd::in("modules/arch/x86/gen_x86_insn.py")
                << cmd::out("x86insns.c")
                << cmd::out("x86insn_gas.gperf")
                << cmd::out("x86insn_nasm.gperf")
                ;
            a_x86["x86insns.c"].skip = true;
        }

        gperf("x86insn_gas.gperf", "x86insn_gas.c");
        a_x86["x86insn_gas.c"].skip = true;
        gperf("x86insn_nasm.gperf", "x86insn_nasm.c");
        a_x86["x86insn_nasm.c"].skip = true;
    }

    auto &libyasm = yasm.addTarget<LibraryTarget>("libyasm");
    {
        setup(libyasm);
        libyasm.ApiName = "YASM_LIB_DECL";
        libyasm += "libyasm/[^/]*\\.c"_rr;
        libyasm -= "libyasm/cmake-module.c";

        for (auto t : deps)
            libyasm.Public += *t;

        // fix names
        yasm_modules["dbgfmt"].erase("codeview");
        yasm_modules["dbgfmt"].insert("cv8");

        yasm_modules["parser"].insert("gnu");
        yasm_modules["parser"].insert("tasm");

        yasm_modules["preproc"].insert("tasm");

        yasm_modules["objfmt"].insert("dosexe");
        yasm_modules["objfmt"].insert("win32");
        yasm_modules["objfmt"].insert("win64");
        yasm_modules["objfmt"].insert("x64");

        yasm_modules["objfmt"].insert("elf32");
        yasm_modules["objfmt"].insert("elf64");
        yasm_modules["objfmt"].insert("elfx32");

        yasm_modules["objfmt"].insert("macho32");
        yasm_modules["objfmt"].insert("macho64");

        String modules = R"(
#define ADD_MODULE(x, t) {#x, &yasm_ ## x ## _LTX_ ## t,},
#define EXTERN_MODULE(m, n) extern yasm_ ## m ## _module yasm_ ## n ## _LTX_ ## m
)";
        modules += "\n";

        for (auto &[mn, d] : yasm_modules)
        {
            String module;
            for (auto &n : d)
            {
                module += "ADD_MODULE(" + n + ", " + mn + ")\n";
                modules += "EXTERN_MODULE(" + mn + ", " + n + ");\n";
            }
            modules += "\n";
            libyasm.replaceInFileOnce("libyasm/module.in", "MODULES_" + mn + "_", module);
        }
        libyasm.replaceInFileOnce("libyasm/module.in", "EXTERN_LIST", modules);

        libyasm.configureFile("libyasm/module.in", "module.c");
        libyasm += "module.c";
    }

    setup(yasm);
    yasm += "frontends/yasm/.*\\.[hc]"_rr;
    yasm += libyasm;
    yasm.writeFileOnce("license.c", "const char *license_msg[] = { \"\" };");
}

void check(Checker &c)
{
    auto &s = c.addSet("libyasm");
    s.checkFunctionExists("mergesort");
    s.checkFunctionExists("popen");
    s.checkFunctionExists("strcasecmp");
    s.checkFunctionExists("strcmpi");
    s.checkFunctionExists("stricmp");
    s.checkFunctionExists("strsep");
    s.checkFunctionExists("_popen");
    s.checkFunctionExists("_stricmp");
    s.checkIncludeExists("libgen.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("strings.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
}
