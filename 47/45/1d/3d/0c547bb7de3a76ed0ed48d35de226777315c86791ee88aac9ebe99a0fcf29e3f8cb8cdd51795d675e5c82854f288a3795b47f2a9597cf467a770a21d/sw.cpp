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
    NasmCompiler(ExecutableTarget &nasm)
        : NativeCompiler(nasm.getSolution().swctx), version(nasm.getPackage().getVersion())
    {
    }

    virtual ~NasmCompiler() = default;

    using NativeCompilerOptions::operator=;

    std::shared_ptr<Program> clone() const override
    {
        return std::make_shared<NasmCompiler>(*this);
    }

    path getOutputFile() const override
    {
        return ObjectFile();
    }

    void prepareCommand1(const Target &t) override
    {
        if (InputFile)
        {
            cmd->name = normalize_path(InputFile());
            cmd->name_short = InputFile().filename().u8string();
        }
        if (ObjectFile)
            cmd->working_directory = ObjectFile().parent_path();

        if (!ObjectFormat)
        {
            if (t.getSettings().TargetOS.Type == OSType::Windows)
            {
                if (t.getSettings().TargetOS.Arch == ArchType::x86_64)
                    ObjectFormat = "win64";
                else
                    ObjectFormat = "win32";
            }
        }

        switch (t.getSettings().TargetOS.Arch)
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

    void setSourceFile(const path &input_file, path &output_file) override
    {
        InputFile = input_file.u8string();
        setOutputFile(output_file);
    }

    void setOutputFile(const path &output_file)
    {
        ObjectFile = output_file;
    }

private:
    Version version;

    Version gatherVersion() const override { return version; }
};

void build(Solution &s)
{
    auto &nasm = s.addTarget<ExecutableTarget>("nasm", "2.14.2");
    nasm += RemoteFile("https://www.nasm.us/pub/nasm/releasebuilds/{M}.{m}.{p:02}/nasm-{M}.{m}.{p:02}.tar.xz");

    nasm.setChecks("nasm");

    nasm +=
        ".*\\.h"_rr,
        "asm/.*\\.c"_rr,
        "output/.*\\.c"_rr,
        "nasmlib/.*\\.c"_rr,
        "common/.*\\.c"_rr,
        "macros/.*\\.c"_rr,
        "stdlib/.*\\.c"_rr,
        "x86/.*\\.c"_rr
        ;
    nasm +=
        "."_idir,
        "include"_idir,
        "x86"_idir,
        "asm"_idir,
        "output"_idir
        ;

    nasm.Public +=
        "include"_id;

    auto C = std::make_shared<NasmCompiler>(nasm);
    C->file = nasm.getOutputFile();
    nasm.setProgram(C);
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
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("stdbool.h");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
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
    s.checkDeclarationExists("decl");
    s.checkDeclarationExists("decl");
    s.checkDeclarationExists("decl");
    s.checkDeclarationExists("decl");
    s.checkDeclarationExists("decl");
    s.checkDeclarationExists("decl");
}
