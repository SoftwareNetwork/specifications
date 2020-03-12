void build(Solution &s)
{
    auto &t = s.addLibrary("musl", "1.2.0");
    t += RemoteFile("https://www.musl-libc.org/releases/musl-{v}.tar.gz");

    //
    std::set<String> arch_list
    {
        "aarch64",
        "arm",
        "i386",
        "m68k",
        "microblaze",
        "mips",
        "mips64",
        "mipsn32",
        "or1k",
        "powerpc",
        "powerpc64",
        "riscv64",
        "s390x",
        "sh",
        "x32",
        "x86_64",
    };

    String arch;
    switch (t.getBuildSettings().TargetOS.Arch)
    {
    case ArchType::x86_64:
        arch = toString(t.getBuildSettings().TargetOS.Arch);
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
            auto p2 = p.parent_path().parent_path() / (p.stem() += ".c");
            if (t.check_absolute(p2, true))
                t -= p2;
        }
    };
    //

    auto alltypes = t.BinaryDir / "bits/alltypes.h";
    {
        t.setExtensionProgram(".s", t.findProgramByExtension(".c")->clone());
        t.setExtensionProgram(".S", t.findProgramByExtension(".c")->clone());

        t.CVersion = CLanguageStandard::C99;
        t.ExportAllSymbols = true;

        t += "arch/.*"_rr;
        t += "include/.*"_rr;
        t += "src/.*"_rr;
        t -= "crt/.*"_rr;
        t -= "ldso/.*"_rr;

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
        if (auto sf = t["crt/crt1.c"].as<NativeSourceFile*>())
            sf->skip_linking = true;

        // for -static -fPIE (-static-pie?)
        // unused for now
        t += "crt/rcrt1.c";
        if (auto sf = t["crt/rcrt1.c"].as<NativeSourceFile*>())
            sf->skip_linking = true;

        t += path("crt/" + arch + "/crti.s");
        if (auto sf = t["crt/" + arch + "/crti.s"].as<NativeSourceFile*>())
            sf->skip_linking = true;

        t += path("crt/" + arch + "/crtn.s");
        if (auto sf = t["crt/" + arch + "/crtn.s"].as<NativeSourceFile*>())
            sf->skip_linking = true;

        auto add_crt1 = [&t](auto nt)
        {
            if (auto L = nt->Linker->as<sw::GNULinker*>())
            {
                std::decay_t<decltype(L->StartFiles())> f;
                f.push_back(t["crt/crt1.c"].as<NativeSourceFile*>()->output);
                f.insert(f.end(), L->StartFiles().begin(), L->StartFiles().end());
                L->StartFiles() = f;
            }
        };

        auto setup_linker = [&t, &s, &arch](auto in)
        {
            if (auto L = in->as<sw::GNULinker*>())
            {
                L->nostdlib = true;
                if (auto sf = t["crt/" + arch + "/crti.s"].as<NativeSourceFile*>())
                    L->StartFiles().push_back(sf->output);
                //L->StartFiles().push_back("crtbegin.o");
                //L->EndFiles().push_back("crtend.o");
                if (auto sf = t["crt/" + arch + "/crtn.s"].as<NativeSourceFile*>())
                    L->EndFiles().push_back(sf->output);
            }
        };

        // rewrite linkers and crt for exes
        //setup_linker(t.Linker); // new targets
        for (auto &[pkg, tgts] : s.getMainBuild().getTargets()) // and old targets (including this one)
        {
            for (auto &tgt : tgts)
            {
                if (auto nt = tgt->as<NativeExecutedTarget*>())
                {
                    if (tgt.get() == &t)
                        continue;
                    setup_linker(nt->Linker);
                    if (auto e = dynamic_cast<ExecutableTarget*>(nt))
                        add_crt1(e);
                }
            }
        }
        //

        t += "_XOPEN_SOURCE=700"_def;

        t.writeFileOnce("version.h", "#define VERSION \"" + t.getPackage().getVersion().toString() + "\"");

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
                << cmd::prog("org.sw.demo.gnu.sed.sed"_dep)
                << "-n" << "-e" << "s/__NR_/SYS_/p"
                << cmd::in("arch/" + arch + "/bits/syscall.h.in")
                << cmd::std_out("bits/syscall.h", cmd::Append)
                ;
        }

        // conditional code only works for non-host solutions
        t.addDummyDependency("org.sw.demo.gnu.gawk.gawk"_dep); // set dependency early
        t -= "tools/.*"_rr; // also take input files
        for (auto &[p, sf] : t[".*\\.s"_rr])
        {
            if (sf->skip)
                continue;
            if (auto nsf = sf->as<NativeSourceFile*>())
            {
                if (auto cl = nsf->compiler->as<sw::ClangCompiler*>())
                {
                    cl->InputFile = "-";
                    cl->InputFile.input_dependency = false;
                    cl->Language = "assembler";
                    auto &cmd2 = *cl->createCommand(t.getSolution().getContext());
                    auto c = t.addCommand();
                    c
                        << cmd::prog("org.sw.demo.gnu.gawk.gawk"_dep)
                        << "-f" << cmd::in("tools/add-cfi.common.awk")
                        << "-f" << cmd::in("tools/add-cfi." + arch + ".awk")
                        << cmd::in(p, cmd::NormalizePath) | cmd2;
                }
            }
        }
    }
}
