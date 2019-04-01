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

    //
    auto &t = s.addLibrary("musl", "1.1.21");
    auto alltypes = t.BinaryDir / "bits/alltypes.h";
    {
        //t += Git("git://git.musl-libc.org/musl", "v{v}");
        t += RemoteFile("https://www.musl-libc.org/releases/musl-{v}.tar.gz");

        t.CVersion = CLanguageStandard::C99;
        t.ExportAllSymbols = true;

        t += "arch/.*"_rr;
        t += "include/.*"_rr;
        t += "src/.*"_rr;
        t -= "crt/.*"_rr;
        t -= "lsdo/.*"_rr;

        t.Public += "include"_idir;

        t.Public += IncludeDirectory("arch/" + arch);
        t.Public += "arch/generic"_idir; // after arch!

        // duplicate here
        t += IncludeDirectory("arch/" + arch);
        t += "arch/generic"_idir; // after arch!

        t += "src/internal"_idir;
        t += "src/include"_idir;

        for (auto &a : arch_list)
            t -= FileRegex({}, ".*/" + a + "/.*", true);

        filter_out(t);

        // https://dev.gentoo.org/~vapier/crt.txt
        // http://gcc.gnu.org/onlinedocs/gccint/Initialization.html

        t += "crt/crt1.c";
        if (auto sf = t["crt/crt1.c"].as<NativeSourceFile>())
            sf->skip_linking = true;

        // for -static -fPIE
        // unused for now
        t += "crt/rcrt1.c";
        if (auto sf = t["crt/rcrt1.c"].as<NativeSourceFile>())
            sf->skip_linking = true;

        t += path("crt/" + arch + "/crti.s");
        if (auto sf = t["crt/" + arch + "/crti.s"].as<NativeSourceFile>())
            sf->skip_linking = true;

        t += path("crt/" + arch + "/crtn.s");
        if (auto sf = t["crt/" + arch + "/crtn.s"].as<NativeSourceFile>())
            sf->skip_linking = true;

        s.registerCallback([&musl = t](auto &t, auto cbt)
        {
            if (cbt == sw::CallbackType::CreateTargetInitialized)
            {
                if (auto nt = dynamic_cast<ExecutableTarget*>(&t))
                {
                    if (auto L = nt->Linker->as<sw::GNULinker>())
                    {
                        std::decay_t<decltype(L->StartFiles())> f;
                        f.push_back(musl["crt/crt1.c"].as<NativeSourceFile>()->output.file);
                        f.insert(f.end(), L->StartFiles().begin(), L->StartFiles().end());
                        L->StartFiles() = f;
                    }
                }
            }
        });

        auto setup_linker = [&t, &s, &arch](auto in)
        {
            if (auto L = in->as<sw::GNULinker>())
            {
                L->nostdlib = true;
                if (auto sf = t["crt/" + arch + "/crti.s"].as<NativeSourceFile>())
                    L->StartFiles().push_back(sf->output.file);
                //L->StartFiles().push_back("crtbegin.o");
                //L->EndFiles().push_back("crtend.o");
                if (auto sf = t["crt/" + arch + "/crtn.s"].as<NativeSourceFile>())
                    L->EndFiles().push_back(sf->output.file);
            }
        };

        setup_linker(t.Linker);
        setup_linker(s.Settings.Native.Linker); // for other targets

        t += "_XOPEN_SOURCE=700"_def;

        t.writeFileOnce("version.h", "#define VERSION \"" + t.pkg.version.toString() + "\"");

        {
            t.addCommand()
                << cmd::prog("org.sw.demo.gnu.sed.sed-*"_dep)
                << "-f"
                << cmd::in("tools/mkalltypes.sed")
                << cmd::in("arch/" + arch + "/bits/alltypes.h.in")
                << cmd::in("include/alltypes.h.in")
                << cmd::std_out(alltypes)
                ;
        }

        {
            t.configureFile("arch/" + arch + "/bits/syscall.h.in", "bits/syscall.h");

            t.addCommand()
                << cmd::prog("org.sw.demo.gnu.sed.sed-*"_dep)
                << "-n" << "-e" << "s/__NR_/SYS_/p"
                << cmd::in("arch/" + arch + "/bits/syscall.h.in")
                << cmd::std_out("bits/syscall.h", cmd::Append)
                ;
        }

        // conditional code only works for non-host solutions
        (t + "org.sw.demo.gnu.gawk.gawk"_dep)->Dummy = true;
        for (auto &[p, sf] : t[".*\\.s"_rr])
        {
            if (sf->skip)
                continue;
            if (auto nsf = sf->as<NativeSourceFile>())
            {
                if (auto cl = nsf->compiler->as<sw::ClangCompiler>())
                {
                    cl->InputFile = "-";
                    cl->InputFile.input_dependency = false;
                    cl->Language = "assembler";
                    auto &cmd2 = *cl->createCommand();
                    t.addCommand()
                        << cmd::prog("org.sw.demo.gnu.gawk.gawk"_dep)
                        << "-f" << cmd::in("tools/add-cfi.common.awk")
                        << "-f" << cmd::in("tools/add-cfi." + arch + ".awk")
                        << cmd::in(p, cmd::NormalizePath) | cmd2;
                }
            }
        }

        t.Interface -= "org.sw.demo.llvm_project.compiler_rt.builtins-master"_dep;
        if (sw::isClangFamily(s.Settings.Native.CompilerType))
            t.Interface += "org.sw.demo.llvm_project.compiler_rt.builtins-master"_dep;
    }
}
