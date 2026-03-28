#pragma sw header on

void setup_cuda_for_target(auto &t) {
    std::shared_ptr<sw::Program> cc1(t.findProgramByExtension(".cpp")->clone());
    auto cc = std::static_pointer_cast<sw::NativeCompiler>(cc1);
    auto ext = ".ii.cpp"s;
    for (auto &&[p,f] : t[".*\\.cu"_rr]) {
        if (f->skip) {
            continue;
        }
        auto n = p.filename().string();
        auto c = t.addCommand();
        c
            << cmd::prog("nvcc")
            << cmd::env("Path", cc->file.parent_path().string())
            //<< "-MD"
            << "-cuda"
            //<< "-c"
            << cmd::in(p)
            << "-o"
            << cmd::out(n + ext)
            ;
        t.add(sw::CallbackType::EndPrepare, [c, &t]() mutable
        {
            t.getMergeObject().addDefinitionsAndIncludeDirectories(*c.getCommand());
        });
        t += path{n + ext};
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
            t += "cudart_static.lib"_slib;
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
