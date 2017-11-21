void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &libffi = s.addTarget<LibraryTarget>("weltling.winlibs.libffi", "3.2.1");
    libffi.Source = Git("https://github.com/winlibs/libffi", "libffi-{v}");
    libffi.ApiName = "SW_FFI_API";

    libffi.setChecks("libffi");

    libffi.ExportAllSymbols = true;
    libffi +=
        "include/.*\\.h"_rr,
        "include/.*\\.h.in"_rr,
        "src/.*"_rr;

    libffi -=
        "src/.*"_rr;

    libffi.Public +=
        "include"_id,
        "src/x86"_id;

    libffi.Private += "FFI_HIDDEN="_d;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        libffi.Private += "HAVE_FASTCALL=0"_d;
    }
    libffi.Private += sw::Shared, "FFI_BUILDING"_d;

    libffi.writeFileOnce("fficonfig.h", true);
    libffi.replaceInFileOnce("include/ffi.h", "#define FFI_EXTERN extern", "");
    libffi.replaceInFileOnce("include/ffi.h", "__declspec(dllimport)", "");
    libffi.replaceInFileOnce("include/ffi.h", "} ffi_type;", "} ffi_type;\n#define FFI_EXTERN extern SW_FFI_API");
    libffi.replaceInFileOnce("src/types.c", "maybe_const ffi_type ffi_type_##name", "FFI_EXTERN maybe_const ffi_type ffi_type_##name");
    libffi.replaceInFileOnce("src/types.c", "const ffi_type ffi_type_void", "FFI_EXTERN const ffi_type ffi_type_void");

    // ASM related settings
    libffi.Variables["HAVE_AS_REGISTER_PSEUDO_OP"] = "0";
    libffi.Variables["HAVE_AS_CFI_PSEUDO_OP"] = "0";
    libffi.Variables["HAVE_AS_X86_PCREL"] = "0";
    libffi.Variables["HAVE_AS_ASCII_PSEUDO_OP"] = "0";
    libffi.Variables["HAVE_AS_STRING_PSEUDO_OP"] = "1";
    // GCC related settings
    libffi.Variables["HAVE_RO_EH_FRAME"] = "0";
    libffi.Variables["EH_FRAME_FLAGS"] = "aw";
    libffi.Variables["HAVE_HIDDEN_VISIBILITY_ATTRIBUTE"] = "0";

    libffi += "src/.*\\.c"_r;
    libffi -= "src/dlmalloc.c";

    auto have64bit = s.Settings.TargetOS.Arch == ArchType::x86_64;
    if (s.Settings.TargetOS.Arch == ArchType::x86_64)
    {
        libffi.Variables["HAVE_64BIT"] = "1";
        libffi.Variables["HAVE_AS_X86_64_UNWIND_SECTION_TYPE"] = "1";
    }
    else
    {
        libffi.Variables["HAVE_64BIT"] = "0";
    }

    libffi.Variables["FFI_EXEC_TRAMPOLINE_TABLE"] = "0";

    Files Asm;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        libffi.Variables["FFI_CLOSURES"] = "1";
        libffi.Variables["SYMBOL_UNDERSCORE"] = "1";
        libffi += "src/x86/ffi.c";
    }
    else if (s.Settings.TargetOS.Type == OSType::Cygwin)
    {
        libffi.Variables["FFI_CLOSURES"] = "1";
        libffi.Variables["SYMBOL_UNDERSCORE"] = "1";
        libffi += "src/x86/ffi.c";
        if (have64bit)
        {
            libffi.Variables["TARGET"] = "X86_WIN64";
            Asm.insert("src/x86/win64.S");
        }
        else
        {
            libffi.Variables["TARGET"] = "X86_WIN32";
            Asm.insert("src/x86/win32.S");
        }
    }
    else if (s.Settings.TargetOS.Type == OSType::Macos)
    {
        libffi.Variables["TARGET"] = "X86_DARWIN";
        libffi.Variables["FFI_CLOSURES"] = "1";
        libffi.Variables["FFI_EXEC_TRAMPOLINE_TABLE"] = "1";
        libffi.Variables["FFI_MMAP_EXEC_WRIT"] = "1";
        libffi += "src/x86/ffi.c";
        libffi += "src/x86/ffi64.c";
        Asm.insert("src/x86/darwin.S");
        Asm.insert("src/x86/darwin64.S");
    }
    else
    {
        libffi.Variables["HAVE_AS_X86_PCREL"] = "1";
        libffi += "src/x86/ffi.c";
        Asm.insert("src/x86/sysv.S");
        if (have64bit)
        {
            libffi.Variables["TARGET"] = "X86_64";
            libffi += "src/x86/ffi64.c";
            Asm.insert("src/x86/unix64.S");
        }
        else
        {
            libffi.Variables["TARGET"] = "X86";
        }
    }

    if (s.Settings.Native.CompilerType == CompilerType::MSVC)
    {
        if (have64bit)
        {
            libffi.Variables["TARGET"] = "X86_WIN64";
            libffi += "src/x86/ffi64.c";
            Asm.insert("src/x86/unix64.S");
        }
        else
        {
            libffi.Variables["TARGET"] = "X86_WIN32";
        }
    }

    libffi.configureFile("include/ffi.h.in", "ffi.h");

    if (s.Settings.Native.CompilerType == CompilerType::MSVC)
    {
        {
            auto ch = s.Settings.Native.CCompiler->clone();
            libffi.Storage.push_back(ch);
            auto c = ch->as<VisualStudioCCompiler>();
            c->IncludeDirectories.insert(libffi.BinaryDir);
            c->IncludeDirectories.insert(libffi.SourceDir / "src" / "x86");
            c->IncludeDirectories.insert(libffi.SourceDir / "src");
            c->CopyOutputToStdOut = true;
            c->CSourceFile = libffi.SourceDir / "src" / "x86" / ("win"s + (have64bit ? "64" : "32") + ".S");
            auto cmd = c->getCommand();
            cmd->redirectStdout(libffi.BinaryDir / "pre.asm");
        }

        {
            auto ch = s.Settings.Native.ASMCompiler->clone();
            libffi.Storage.push_back(ch);
            auto c = ch->as<VisualStudioASMCompiler>();
            c->PreserveSymbolCase = true;
            c->SafeSEH = true;
            const auto o = libffi.BinaryDir / "pre.obj";
            c->ObjectFile = o;
            c->InputFile = libffi.BinaryDir / "pre.asm";
            auto cmd = c->getCommand();
            cmd->addOutput(o);
            libffi += o;
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
