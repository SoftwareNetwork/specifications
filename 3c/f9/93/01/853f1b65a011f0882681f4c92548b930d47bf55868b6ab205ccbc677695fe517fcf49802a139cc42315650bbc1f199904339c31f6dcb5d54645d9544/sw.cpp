void build(Solution &s)
{
    auto &llvm_project = s.addProject("llvm_project", "10.0.0");
    //auto &llvm_project = s.addProject("llvm_project", "master");
    //llvm_project += Git("https://github.com/llvm/llvm-project");

    auto is_musl = [](auto &t)
    {
        return
            t.getSettings()["native"]["stdlib"]["c"].isValue() &&
            sw::UnresolvedPackage(t.getSettings()["native"]["stdlib"]["c"].getValue()).getPath() == "org.sw.demo.musl";
    };

    auto &builtins = llvm_project.addLibrary("compiler_rt.builtins");
    {
        auto &b = builtins;
        b += RemoteFile("https://github.com/llvm/llvm-project/releases/download/llvmorg-{v}/compiler-rt-{v}.src.tar.xz");
        if (fs::exists(b.SourceDir / "compiler-rt"))
            b.setSourceDirectory("compiler-rt");

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
            switch (b.getBuildSettings().TargetOS.Arch)
            {
            case ArchType::x86_64:
                arch = toString(b.getBuildSettings().TargetOS.Arch);
                break;
            case ArchType::x86:
                arch = "i386";
                break;
            case ArchType::arm:
                arch = "arm";
            case ArchType::aarch64:
                arch = "aarch64";
                break;
            default:
                throw SW_RUNTIME_ERROR("arch is not implemented");
            }

            arch_list.erase(arch);

            // filter out same .c files
            auto filter_out = [](auto &t)
            {
                for (const auto &[p, sf] : t[".*\\.[csS]"_rr])
                {
                    if (sf->skip)
                        continue;
                    auto p2 = p.parent_path().parent_path() / (p.stem() += ".c");
                    if (t.check_absolute(p2, true))
                        t -= p2;
                }
            };

            b.ExportAllSymbols = true;
            b += "lib/builtins/.*"_r;
            b += FileRegex("lib/builtins", arch + "/.*", true);
            b -= "lib/builtins/gcc_personality_v0.c";
            filter_out(b);
        }
    }

    auto &libunwind = llvm_project.addLibrary("libunwind");
    {
        auto &t = libunwind;
        t += RemoteFile("https://github.com/llvm/llvm-project/releases/download/llvmorg-{v}/libunwind-{v}.src.tar.xz");
        if (fs::exists(t.SourceDir / "libunwind"))
            t.setSourceDirectory("libunwind");

        t += "include/.*"_rr;
        t += "src/.*"_rr;

        if (!t.getBuildSettings().TargetOS.isApple())
        {
            t -= "src/Unwind_AppleExtras.cpp";
        }

        if (!is_musl(t))
        {
            t += "dl"_slib;
            t += "pthread"_slib;
        }

        /*if (t.getBuildSettings().TargetOS.isApple())
        {
        if (t.getCompilerType() == CompilerType::Clang)
        {
        t.CompileOptions.push_back("-fno-rtti");
        t.CompileOptions.push_back("-fno-exceptions");
        t.CompileOptions.push_back("-funwind-tables");
        }
        t.CompileOptions.push_back("-fvisibility=default");
        }*/

        // less size?
        //t += "_LIBUNWIND_IS_NATIVE_ONLY"_def;

        // need manual .eh_handle sections
        //t += "_LIBUNWIND_IS_BAREMETAL"_def;
    }

    auto &libcxxabi = llvm_project.addLibrary("libcxxabi");

    auto &libcxx = llvm_project.addLibrary("libcxx");
    {
        auto &t = libcxx;
        t += RemoteFile("https://github.com/llvm/llvm-project/releases/download/llvmorg-{v}/libcxx-{v}.src.tar.xz");
        if (fs::exists(t.SourceDir / "libcxx"))
            t.setSourceDirectory("libcxx");

        t += cpp11;
        // t += cpp14; // since 10.0
        t -= "src/support/.*"_rr;

        t += "_LIBCPP_BUILDING_LIBRARY"_def;
        //t += "_LIBCXXABI_BUILDING_LIBRARY"_def;
        t += "LIBCXX_BUILDING_LIBCXXABI"_def;
        if (t.getBuildSettings().TargetOS.isApple())
        {
            t.Public += "_LIBCPP_DISABLE_AVAILABILITY"_def;
            t.CompileOptions.push_back("-fvisibility=default");
        }

        if (!is_musl(t))
            t += "pthread"_slib;

        if (is_musl(t))
        {
            t.Public += "_LIBCPP_HAS_MUSL_LIBC"_def;
            //t.Public += "_LIBCPP_HAS_THREAD_API_PTHREAD"_def;
        }

        t.Public += libcxxabi;
        t.Interface += libunwind;
        if (t.getBuildSettings().TargetOS.isApple())
        {
            t.Interface -= libcxxabi;
            t.Interface -= libunwind;
        }

        if (isClangFamily(t.getCompilerType()))
        {
            t.Public.CompileOptions.push_back("-nostdinc++");
            t.Public.LinkOptions.push_back("-nostdlib++");
        }
    }

    // libcxxabi
    {
        auto &t = libcxxabi;
        t += RemoteFile("https://github.com/llvm/llvm-project/releases/download/llvmorg-{v}/libcxxabi-{v}.src.tar.xz");
        if (fs::exists(t.SourceDir / "libcxxabi"))
            t.setSourceDirectory("libcxxabi");

        t += cpp11;

        t -= "src/cxa_noexception.cpp";
        t += "_LIBCXXABI_BUILDING_LIBRARY"_def;
        //t += "LIBCXXABI_USE_LLVM_UNWINDER"_def;
        t += libcxx;
        if (t.getBuildSettings().TargetOS.isApple())
        {
            //t.CompileOptions.push_back("-fvisibility=default");
            //t.CompileOptions.push_back("-fvisibility-global-new-delete-hidden");
        }
    }
}
