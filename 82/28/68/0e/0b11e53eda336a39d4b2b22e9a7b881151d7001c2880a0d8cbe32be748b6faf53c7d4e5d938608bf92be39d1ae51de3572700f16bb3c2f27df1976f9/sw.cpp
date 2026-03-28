#pragma sw header on

static int cuda_preprocess_file(path i, path o)
{
    auto s = read_file(i);
    //boost::replace_all(s, "#pragma detect_mismatch(\"_ITERATOR_DEBUG_LEVEL", "// detect_mismatch(\"_ITERATOR_DEBUG_LEVEL");
    //boost::replace_all(s, "#pragma detect_mismatch(\"RuntimeLibrary", "// detect_mismatch(\"RuntimeLibrary");
    //boost::replace_all(s, "#pragma comment(lib", "// comment(lib");
    write_file(o, s);

    return 0;
}
SW_DEFINE_VISIBLE_FUNCTION_JUMPPAD2(cuda_preprocess_file)

void setup_cuda_for_target(auto &t) {
    std::shared_ptr<sw::Program> cc1(t.findProgramByExtension(".cpp")->clone());
    auto cc = std::static_pointer_cast<sw::NativeCompiler>(cc1);
    auto ext1 = ".ii.cpp"s;
    auto ext2 = ".cpp"s;
    for (auto &&[p,f] : t[".*\\.cu"_rr]) {
        if (f->skip) {
            continue;
        }
        auto n = p.filename().string();
        // this is a preprocessor step (--cuda), so we must provide everything ourselves (like -D_DEBUG etc.)
        // for flags see https://learn.microsoft.com/en-us/cpp/build/reference/md-mt-ld-use-run-time-library
        auto c = t.addCommand();
        c
            << cmd::prog("nvcc")
            << cmd::env("Path", cc->file.parent_path().string())
            //<< "-MD"
            << "--cuda"
            //<< "-c"
            << cmd::in(p)
            << "-o"
            << cmd::out(n + ext1)
            ;
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows) {
            // != GNU?
            // if (getCompilerType() == CompilerType::MSVC || getCompilerType() == CompilerType::ClangCl)
            //c << "-D_CRT_NOPRAGMA_LIBS"; // (prevent pragma comment(lib, ...) - this one is getting undeffed)
            c << "-D_USE_ANSI_CPP"; // (prevent pragma comment(lib, ...) - prevent whole header)
            c << "-D_MT"; // (non /LD mode)
            if (!t.getBuildSettings().Native.MT) {
                c << "-D_DLL";
            }
            if (t.getBuildSettings().Native.ConfigurationType == ConfigurationType::Debug) {
                c << "-D_DEBUG";
            }
        }
        t.add(sw::CallbackType::EndPrepare, [c, &t]() mutable
        {
            t.getMergeObject().addDefinitionsAndIncludeDirectories(*c.getCommand());
        });

        //t.addCommand(SW_VISIBLE_FUNCTION(cuda_preprocess_file))
        //    << cmd::in(n + ext1)
        //    << cmd::out(n + ext1 + ext2)
        //    ;
        // sed -s '/#line.*/d;/^[[:space:]]*$/d' cuda_dlib.cu.ii.cpp > 1.cpp
        //t += path{n + ext1 + ext2};
    }
}

#pragma sw header off

void build(Solution &s)
{
    auto add_target = [&](auto &&...args) -> decltype(auto) {
        auto &t = s.addTarget<LibraryTarget>(args...);
        t += EmptySource{};
        t.AutoDetectOptions = false;
        t.Empty = true;
        return t;
    };

    path cuda_root_win = "C:\\Program Files\\NVIDIA";
    auto ver = "13.2";

    auto &cuda = add_target("find.nvidia.cuda", ver);
    {
        auto &t = cuda;
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows) {
            t.Public += IncludeDirectory{cuda_root_win / "cuda" / ver / "include"s};
            t += LinkDirectory{cuda_root_win / "cuda" / ver / "lib/x64"s};
            if (!t.getBuildSettings().Native.MT) {
                t += "cudart.lib"_slib;
            } else {
                t += "cudart_static.lib"_slib;
            }
            t += "cusolver.lib"_slib;
            t += "curand.lib"_slib;
            t += "cublas.lib"_slib;
        }
    }
    auto &cudnn = add_target("find.nvidia.cudnn", ver);
    {
        auto &t = cudnn;
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows) {
            t.Public += IncludeDirectory{cuda_root_win / "cudnn/include" / ver};
            t += LinkDirectory{cuda_root_win / "cudnn/lib" / ver / "x64"s};
            t += "cudnn.lib"_slib;
        }
    }
}
