struct NasmAssemblerOptions
{
    COMMAND_LINE_OPTION(ObjectFile, path)
    {
        cl::CommandFlag{ "o" },
            cl::OutputDependency{},
    };

    COMMAND_LINE_OPTION(ObjectFormat, String)
    {
        cl::CommandFlag{ "f" },
    };

    // goes last
    COMMAND_LINE_OPTION(InputFile, path)
    {
        cl::InputDependency{},
    };
};
DEFINE_OPTION_SPECIALIZATION_DUMMY(NasmAssemblerOptions);

struct NasmCompiler : sw::NativeCompiler,
    sw::CommandLineOptions<NasmAssemblerOptions>
{
    using sw::NativeCompiler::NativeCompiler;

    virtual ~NasmCompiler() = default;

    using NativeCompilerOptions::operator=;

    std::unique_ptr<Program> clone() const override
    {
        return std::make_unique<NasmCompiler>(*this);
    }

    path getOutputFile() const override
    {
        return ObjectFile();
    }

    void prepareCommand1(const Target &t) override
    {
        if (InputFile)
        {
            //cmd->name = normalize_path(InputFile());
            //cmd->name_short = InputFile().filename().u8string();
        }
        if (ObjectFile)
            cmd->working_directory = ObjectFile().parent_path();

        if (!ObjectFormat)
        {
            if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            {
                if (t.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
                    ObjectFormat = "win64";
                else
                    ObjectFormat = "win32";
            }
        }

        switch (t.getBuildSettings().TargetOS.Arch)
        {
        case ArchType::x86_64:
            add("ARCH_X86_32=0"_def);
            add("ARCH_X86_64=1"_def);
            break;
        case ArchType::x86:
            add("ARCH_X86_32=1"_def);
            add("ARCH_X86_64=0"_def);
            break;
        default:
            SW_UNIMPLEMENTED;
        }

        sw::getCommandLineOptions<NasmAssemblerOptions>(cmd.get(), *this);
        addEverything(*cmd);
    }

    void setSourceFile(const path &input_file, const path &output_file) override
    {
        InputFile = input_file.u8string();
        setOutputFile(output_file);
    }

    void setOutputFile(const path &output_file)
    {
        ObjectFile = output_file;
    }
};

void build(Solution &s)
{
    auto &nasm = s.addTarget<ExecutableTarget>("nasm", "3.1");
    //nasm += RemoteFile("https://www.nasm.us/pub/nasm/releasebuilds/{M}.{m:02}/nasm-{M}.{m:02}rc2.tar.gz"); // with generated files (no perl)
    nasm += Git("https://github.com/netwide-assembler/nasm", "nasm-{M}.{m:02}"); // requires perl to generate files
    //nasm += Git("https://github.com/netwide-assembler/nasm", "nasm-{M}.{m:02}rc2"); // requires perl to generate files

    //nasm += c23; // after sw update is pushed
    nasm.CVersion = CLanguageStandard::C23;

    nasm.setChecks("nasm", true);

    nasm +=
        ".*\\.h"_rr,
        "asm/.*\\.c"_rr,
        "nasmlib/.*\\.c"_rr,
        "common/.*\\.c"_rr,
        "stdlib/.*\\.c"_rr//,
        //"x86/.*\\.c"_rr
        ;
    nasm -=
        "macros/.*"_rr,
        "output/.*"_rr,
        "perllib/.*"_rr,
        "x86/.*"_rr
        ;
    nasm +=
        "."_idir,
        "include"_idir,
        "x86"_idir,
        "asm"_idir,
        "output"_idir
        ;
    nasm += "output/.*\\.c"_rr;

    auto zlib = (nasm + "org.sw.demo.madler.zlib"_dep);

    auto add_perl_dep = [&](auto &&c, auto &&dep) {
        auto d = nasm.addProgDependency(dep);
        auto fn = nasm.getObjFile(d, "bin");
        c << "-I" + fn.string();
        std::dynamic_pointer_cast<::sw::driver::Command>(c.getCommand())->addProgramDependency(d);
        return d;
    };
    auto perl_command = [&]() {
        auto c = nasm.addCommand();
        c << cmd::prog("org.sw.demo.perl.perl"_dep);
        c << "-I" + (nasm.SourceDir / "perllib").string();
        c << "-I" + nasm.SourceDir.string();
        c << "-I" + nasm.BinaryDir.string();
        add_perl_dep(c, "org.sw.demo.perl.packages.dist.PathTools.Cwd"_dep);
        add_perl_dep(c, "org.sw.demo.perl.packages.ext.Fcntl"_dep);
        add_perl_dep(c, "org.sw.demo.perl.packages.dist.IO"_dep);
        add_perl_dep(c, "org.sw.demo.perl.packages.cpan.Util"_dep);
        add_perl_dep(c, "org.sw.demo.perl.packages.ext.File.Glob"_dep);
        add_perl_dep(c, "org.sw.demo.perl.packages.cpan.Compress.Raw.Zlib"_dep);
        return c;
    };

    perl_command()
        << cmd::in("x86/preinsns.pl")
        << cmd::in("x86/insns.dat")
        << cmd::out("x86/insns.xda")
        ;
    //
    struct pair {const char *k,*f;};
    for (auto &&[k,f] : {
        pair{"-fc","x86/iflag.c"},
        pair{"-fh","x86/iflaggen.h"},
        pair{"-b","x86/insnsb.c"},
        pair{"-a","x86/insnsa.c"},
        pair{"-d","x86/insnsd.c"},
        pair{"-i","x86/insnsi.h"},
        pair{"-n","x86/insnsn.c"},
        })
    perl_command()
        << cmd::in("x86/insns.pl")
        << k
        << cmd::in("x86/insns.xda")
        << cmd::out(f)
        ;

    //
    for (auto &&[k,f] : {
        pair{"h","asm/pptok.h"},
        pair{"c","asm/pptok.c"},
        pair{"ph","asm/pptok.ph"},
        })
    perl_command()
        << cmd::in("asm/pptok.pl")
        << k
        << cmd::in("asm/pptok.dat")
        << cmd::out(f)
        ;

    for (auto &&[k,f] : {
        pair{"h","asm/directiv.h"},
        pair{"c","asm/directbl.c"},
        })
    perl_command()
        << cmd::in("nasmlib/perfhash.pl")
        << k
        << cmd::in("asm/directiv.dat")
        << cmd::out(f)
        ;

    for (auto &&[k,f] : {
        pair{"c","x86/regs.c"},
        pair{"fc","x86/regflags.c"},
        pair{"dc","x86/regdis.c"},
        pair{"dh","x86/regdis.h"},
        pair{"vc","x86/regvals.c"},
        pair{"h","x86/regs.h"},
        })
    perl_command()
        << cmd::in("x86/regs.pl")
        << k
        << cmd::in("x86/regs.dat")
        << cmd::std_out(f)
        ;

    for (auto &&[k,f] : {
        pair{"h","asm/warnings.h"},
        pair{"c","asm/warnings_c.h"},
        })
    perl_command()
        << cmd::in("asm/warnings.pl")
        << k
        << cmd::out(f)
        << cmd::in("asm/warnings.dat")
        ;

    for (auto &&[k,f] : {
        pair{"h","version.h"},
        pair{"mac","version.mac"},
        })
    perl_command()
        << cmd::in("version.pl")
        << k
        << cmd::std_in("version")
        << cmd::std_out(f)
        ;

    for (auto &&[k,f] : {
        pair{"h","asm/tokens.h"},
        pair{"c","asm/tokhash.c"},
        })
    perl_command()
        << cmd::in("asm/tokhash.pl")
        << k
        << cmd::in("x86/insnsn.c")
        << cmd::in("x86/regs.dat")
        << cmd::in("asm/tokens.dat")
        << cmd::std_out(f)
        ;

    auto m = perl_command()
        << cmd::wdir(nasm.BinaryDir)
        << cmd::in("macros/macros.pl")
        << cmd::in("version.mac")
        ;
    for (auto &&f : {
        "macros/altreg.mac",
        "macros/fp.mac",
        "macros/ifunc.mac",
        "macros/masm.mac",
        "macros/smartalign.mac",
        "macros/standard.mac",
        "macros/vtern.mac",
    })
        m << cmd::in(f);
    for (auto &&f : {
        "output/outaout.mac",
        "output/outas86.mac",
        "output/outbin.mac",
        "output/outcoff.mac",
        "output/outdbg.mac",
        "output/outelf.mac",
        "output/outmacho.mac",
        "output/outobj.mac",
    })
        m << cmd::in(f);
    m
        << cmd::end()
        << cmd::out("macros/macros.c")
        ;

    nasm += IncludeDirectory{nasm.BinaryDir / "asm"};
    nasm += IncludeDirectory{nasm.BinaryDir / "x86"};

    //if (mp.getCompilerType() == CompilerType::GNU) {
    if (nasm.getBuildSettings().TargetOS.Type != OSType::Windows) {
        nasm += "HAVE_STDNORETURN_H"_def;
        //nasm += "HAVE_STDC_INLINE"_def;
        nasm += "HAVE_STDC_LEADING_ZEROS"_def;
        nasm += "HAVE___BUILTIN_CLZ"_def;
        nasm += "HAVE__BUILTIN_CLZLL"_def;

        nasm += "_GNU_SOURCE"_def;
        // in endian.h
        nasm += "HAVE_HTOLE16"_def;
        nasm += "HAVE_HTOLE32"_def;
        nasm += "HAVE_HTOLE64"_def;
    }

    nasm.Public += "include"_id;

    auto C = std::make_shared<NasmCompiler>();
    C->file = nasm.getOutputFile();
    nasm.setProgram(C);

    nasm.pushFrontToFileOnce("include/compiler.h", "#include <stdint.h>");
}

void check(Checker &c)
{
    auto &s = c.addSet("nasm");
    s.checkFunctionExists("canonicalize_file_name");
    s.checkFunctionExists("getgid");
    s.checkFunctionExists("getuid");
    s.checkFunctionExists("pathconf");
    s.checkFunctionExists("realpath");
    s.checkFunctionExists("strcasecmp");
    s.checkFunctionExists("strcspn");
    s.checkFunctionExists("stricmp");
    s.checkFunctionExists("strlcpy");
    s.checkFunctionExists("strncasecmp");
    s.checkFunctionExists("strnicmp");
    s.checkFunctionExists("strsep");
    s.checkFunctionExists("strspn");
    s.checkFunctionExists("vsnprintf");
    s.checkFunctionExists("_fullpath");
    s.checkFunctionExists("_snprintf");
    s.checkFunctionExists("_vsnprintf");
    s.checkFunctionExists("mempcpy");
    s.checkFunctionExists("mempset");

    //s.checkIncludeExists("stdnoreturn.h"); // causes build errors on msvc
    //s.checkIncludeExists("windows.h");
    s.checkIncludeExists("endian.h");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("stdbit.h");
    s.checkIncludeExists("stdbool.h");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdio.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("unistd.h");

    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
}
