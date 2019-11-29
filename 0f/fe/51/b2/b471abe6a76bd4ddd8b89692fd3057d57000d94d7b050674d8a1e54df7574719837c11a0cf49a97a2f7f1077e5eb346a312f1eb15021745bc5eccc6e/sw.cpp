void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("libffi", "3.3.0");
    t += Git("https://github.com/libffi/libffi", "v{M}.{m}");

    t.setChecks("libffi", true);

    t += "include/.*\\.h.*"_rr;
    t -= "src/.*"_rr;
    t += "src/.*"_r;
    t -= "src/dlmalloc.c";

    t.Public += "include"_idir;

    t += "FFI_BUILDING"_def;
    t += sw::Shared, "FFI_BUILDING_DLL"_def;

    t += "FFI_HIDDEN="_def;

    String arch, arch_dir;
    switch (t.getBuildSettings().TargetOS.Arch)
    {
    case ArchType::x86_64:
        arch_dir = "x86";
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            arch = "X86_WIN64";
        break;
    case ArchType::x86:
        arch_dir = "x86";
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            arch = "X86_WIN32";
        break;
    default:
        throw SW_RUNTIME_ERROR("Not implemented");
    }

    t += FileRegex("src/" + arch_dir, ".*", false);
    t.Public += IncludeDirectory("src/" + arch_dir);

    if (t.getBuildSettings().TargetOS.Type != OSType::Windows ||
        (t.getCompilerType() == CompilerType::MSVC && t.getBuildSettings().TargetOS.Arch == ArchType::x86))
    {
        t -= path("src/" + arch_dir + "/ffiw64.c");
    }
    if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
    {
        t -= "src/x86/win64.S";
        t -= "src/x86/win64_intel.S";
    }

    t.Variables["TARGET"] = arch;
    t.Variables["FFI_EXEC_TRAMPOLINE_TABLE"] = "0";

    t.configureFile("include/ffi.h.in", "ffi.h");
    t.writeFileOnce("fficonfig.h"); // for asm

    if (s.DryRun)
        return;

    if (t.getCompilerType() == CompilerType::MSVC)
    {
        String f = t.getBuildSettings().TargetOS.Arch == ArchType::x86_64 ? "win64" : "sysv";
        f += "_intel";
        {
            auto p = t.findProgramByExtension(".c");
            if (!p)
                throw std::runtime_error("No c compiler found");
            auto ch = std::static_pointer_cast<sw::NativeCompiler>(p->clone());
            t.Storage.push_back(ch);
            auto c = dynamic_cast<VisualStudioCompiler*>(ch.get());
            c->PreprocessToStdout = true; // supress #line directives
            c->PreprocessToFile = true;
            c->CSourceFile = t.SourceDir / "src" / "x86" / (f + ".S");
            auto cmd = c->createCommand(t.getSolution().getContext());
            cmd->working_directory = t.BinaryDir;
            cmd->addOutput(t.BinaryDir / (f + ".i"));
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
            auto ch = std::static_pointer_cast<sw::NativeCompiler>(p->clone());
            t.Storage.push_back(ch);
            auto c = dynamic_cast<VisualStudioASMCompiler*>(ch.get());
            c->PreserveSymbolCase = true;
            c->SafeSEH = true;
            const auto o = t.BinaryDir / "pre.obj";
            c->Output = o;
            c->InputFile = t.BinaryDir / (f + ".i");
            auto cmd = c->createCommand(t.getSolution().getContext());
            cmd->addOutput(o);
            t += o;
            t.add(sw::CallbackType::EndPrepare, [ch, &t]()
            {
                ch->merge(t);
                ch->getCommand(t);
            });
        }
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
}
