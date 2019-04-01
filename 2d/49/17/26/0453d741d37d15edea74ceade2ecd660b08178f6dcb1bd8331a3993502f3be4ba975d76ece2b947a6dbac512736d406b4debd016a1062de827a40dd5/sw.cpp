void configure(Build &b)
{
    auto &s = b.addSolution();
    s.Settings.TargetOS.Type = OSType::Linux;
    s.Settings.Native.CompilerType = CompilerType::Clang;
}

void build(Solution &s)
{
    std::set<String> arch_list
    {
        "aarch64",
        "arm",
        "i386",
        "microblaze",
        "mips",
        "mipsn32",
        "mips64",
        "m68k",
        "or1k",
        "powerpc",
        "powerpc64",
        "s390x",
        "sh",
        "x32",
        "x86_64",
    };

    String arch;
    switch (s.Settings.TargetOS.Arch)
    {
    case ArchType::x86_64:
        arch = toString(s.Settings.TargetOS.Arch);
        break;
    case ArchType::x86:
        arch = "i386";
        break;
    default:
        throw SW_RUNTIME_ERROR("arch is not implemented");
    }

    arch_list.erase(arch);

    // filter out same .c files
    auto filter_out = [](auto &t)
    {
        for (auto &[p, sf] : t[".*\\.[csS]"_rr])
        {
            if (sf->skip)
                continue;
            t -= p.parent_path().parent_path() / (p.stem() += ".c");
        }
    };

    auto &b = s.addLibrary("llvm_project.compiler_rt.builtins", "master");
    {
        b += Git("https://llvm.org/git/compiler-rt");
        b.ExportAllSymbols = true;
        b.setRootDirectory("d:\\dev\\llvm\\tools\\compiler-rt");
        b += "lib/builtins/.*"_r;
        b += FileRegex("lib/builtins", arch + "/.*", true);
        b -= "lib/builtins/gcc_personality_v0.c";
        auto d = "org.sw.demo.musl"_dep;
        d->IncludeDirectoriesOnly = true;
        d->GenerateCommandsBefore = true;
        b += d;
        filter_out(b);
    }
}
