void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("libffi", "3.4.1");
    t += Git("https://github.com/libffi/libffi", "v{v}");

    t.setChecks("libffi", true);

    t += "include/.*\\.h.*"_rr;
    t -= "src/.*"_rr;
    t += "src/.*"_r;
    t -= "src/dlmalloc.c";

    t.Public += "include"_idir;

    // ffi has incorrect dllexport/dllimport defs
    // so we use ours
    //t += "FFI_BUILDING"_def;
    //t += sw::Shared, "FFI_BUILDING_DLL"_def;
    t.ApiName = "SW_FFI_API";
    t.patch("include/ffi.h.in", "define FFI_API", "define FFI_API SW_FFI_API//");

    String arch, arch_dir;
    switch (t.getBuildSettings().TargetOS.Arch)
    {
    case ArchType::x86_64:
        arch_dir = "x86";
        arch = "X86_64";
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            arch = "X86_WIN64";
        break;
    case ArchType::x86:
        arch_dir = "x86";
        arch = "X86";
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            arch = "X86_WIN32";
        break;
    default:
        throw SW_RUNTIME_ERROR("Not implemented");
    }

    t += FileRegex("src/" + arch_dir, ".*", false);
    t.Public += IncludeDirectory("src/" + arch_dir);

    if (t.getCompilerType() == CompilerType::MSVC && t.getBuildSettings().TargetOS.Arch == ArchType::x86)
    {
        t -= path("src/" + arch_dir + "/ffiw64.c");
    }
    if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
    {
        if (t.getBuildSettings().TargetOS.Type != OSType::Mingw)
            t += "HAVE_HIDDEN_VISIBILITY_ATTRIBUTE"_def;
        t -= "src/x86/win64_intel.S";
        t -= "src/x86/sysv_intel.S";
    }

    t.Variables["TARGET"] = arch;
    t.Variables["FFI_EXEC_TRAMPOLINE_TABLE"] = "0";

    t.configureFile("include/ffi.h.in", "ffi.h");
    // for asm
    t.writeFileOnce("fficonfig.h", R"(
#ifdef HAVE_HIDDEN_VISIBILITY_ATTRIBUTE
    #ifdef LIBFFI_ASM
        #ifdef __APPLE__
            #define FFI_HIDDEN(name) .private_extern name
        #else
            #define FFI_HIDDEN(name) .hidden name
        #endif
    #else
        #define FFI_HIDDEN __attribute__ ((visibility ("hidden")))
    #endif
#else
    #ifdef LIBFFI_ASM
        #define FFI_HIDDEN(name)
    #else
        #define FFI_HIDDEN
    #endif
#endif

// x86 only
// see https://github.com/libffi/libffi/blob/master/configure.ac#L147
// TODO: add check
#define HAVE_AS_X86_PCREL 1
)");

    if (t.DryRun)
        return;

    if (t.getCompilerType() == CompilerType::MSVC)
    {
        String f = t.getBuildSettings().TargetOS.Arch == ArchType::x86_64 ? "win64" : "sysv";
        f += "_intel";
        auto out = t.BinaryDir / (f + ".i");
#if SW_CPP_DRIVER_API_VERSION >= 2
        {
            auto c = t.addCommand();
            c << cmd::prog(t.getRule("c"))
                << cmd::wdir(t.BinaryDir)
                << "-EP"
                << "-P"
                << cmd::out(out, cmd::Prefix{ "-Fi" })
                << cmd::in("src/x86/" + f + ".S")
                ;
            for (auto &i : t.Public.IncludeDirectories)
                c << "-I" << i;
            c << "-I" << t.BinaryDir;
        }
        {
            auto c = t.addCommand();
            c << cmd::prog(t.getRule("asm"))
                << "-c"
                << "-Cx" // PreserveSymbolCase
                << cmd::out(t.BinaryDir / "pre.obj", cmd::Prefix{ "-Fo" })
                << cmd::in(out)
                ;
            if (t.getBuildSettings().TargetOS.Arch != ArchType::x86_64)
                c << "-safeseh";
        }
#elif SW_CPP_DRIVER_API_VERSION == 1
        {
            auto p = t.findProgramByExtension(".c");
            if (!p)
                throw std::runtime_error("No c compiler found");
            std::shared_ptr<sw::Program> ch1(p->clone());
            auto ch = std::static_pointer_cast<sw::NativeCompiler>(ch1);
            t.Storage.push_back(ch);
            auto c = dynamic_cast<VisualStudioCompiler*>(ch.get());
            c->PreprocessSupressLineDirectives = true;
            c->PreprocessToFile = true;
            c->PreprocessFileName = out;
            c->CSourceFile = t.SourceDir / "src" / "x86" / (f + ".S");
            auto cmd = c->createCommand(t.getMainBuild());
            cmd->working_directory = t.BinaryDir;
            t.registerCommand(*cmd);
            t.add(sw::CallbackType::EndPrepare, [ch, &t]()
            {
                ch->merge(t);
                ch->getCommand(t);
            });
        }
        {
            auto p = t.findProgramByExtension(".asm");
            if (!p)
                throw std::runtime_error("No asm compiler found");
            std::shared_ptr<sw::Program> ch1(p->clone());
            auto ch = std::static_pointer_cast<sw::NativeCompiler>(ch1);
            t.Storage.push_back(ch);
            auto c = dynamic_cast<VisualStudioASMCompiler*>(ch.get());
            c->PreserveSymbolCase = true;
            c->SafeSEH = true;
            const auto o = t.BinaryDir / "pre.obj";
            c->Output = o;
            c->InputFile = out;
            auto cmd = c->createCommand(t.getMainBuild());
            cmd->addOutput(o);
            t += o;
            t.add(sw::CallbackType::EndPrepare, [ch, &t]()
            {
                ch->merge(t);
                ch->getCommand(t);
            });
        }
#else
#error "too old sw"
#endif
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("libffi");
    s.checkFunctionExists("alloca");
    s.checkFunctionExists("memcpy");
    s.checkFunctionExists("mkostemp");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("mmap_blacklist");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkTypeSize("double");
    s.checkTypeSize("long");
    s.checkTypeSize("long double");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkSourceCompiles("HAVE_AS_X86_64_UNWIND_SECTION_TYPE", R"sw_xxx(.section .eh_frame,"a",@unwind)sw_xxx").setFileName("conftest.s");
}
