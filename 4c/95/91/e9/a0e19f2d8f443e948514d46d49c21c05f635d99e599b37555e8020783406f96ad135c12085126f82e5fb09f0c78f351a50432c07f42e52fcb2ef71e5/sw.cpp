#pragma sw header on

struct ProtocData
{
    path input;
    path outdir;
    String generator;
    Strings exts;
    FilesOrdered idirs;
    DependencyPtr plugin;

    void addIncludeDirectory(const path &p) { idirs.push_back(p); }

    String getHash() const
    {
        size_t h = 0;
        hash_combine(h, input);
        hash_combine(h, outdir);
        hash_combine(h, generator);
        hash_combine(h, exts);
        hash_combine(h, idirs);
        if (plugin)
#if SW_CPP_DRIVER_API_VERSION > 1
            hash_combine(h, plugin->getUnresolvedPackageId().getName().toString());
#else
            hash_combine(h, plugin->getPackage().toString());
#endif
        return std::to_string(h);
    }

    auto generate(const DependencyPtr &protoc, NativeExecutedTarget &t)
    {
        if (idirs.empty())
        {
            addIncludeDirectory(t.SourceDir);
            //throw SW_RUNTIME_ERROR("protoc: empty include dirs, provide atleast one");
        }
        path rel_input = input;
        if (input.is_absolute())
        {
            for (auto &i : idirs)
            {
                if (is_under_root(input, i))
                {
                    rel_input = input.lexically_relative(i);
                    break;
                }
            }
        }
        if (generator.empty())
            throw SW_LOGIC_ERROR("protoc: empty generator");
        if (exts.empty())
            throw SW_LOGIC_ERROR("protoc: empty exts");
        if (outdir.empty())
            outdir = t.BinaryDir;

        // append protoc idir
        addIncludeDirectory(t.getFile(protoc, "src"));

        auto deps_file = t.BinaryDir.parent_path() / "obj" / (input.filename() += "." + getHash() + ".d");
#if SW_CPP_DRIVER_API_VERSION > 1
        auto gc = std::make_shared<::sw::driver::Command>();
#else
        auto gc = std::make_shared<::sw::driver::Command>(t.getMainBuild());
#endif
        gc->deps_processor = Command::DepsProcessor::Gnu;
        gc->deps_file = deps_file;

        auto c = t.addCommand(gc);
        c << cmd::prog(protoc);
        c << cmd::wdir(outdir);

        // deps file
        c << ("--dependency_out=" + to_string(normalize_path(deps_file)));

        // idirs first
        for (auto &i : idirs)
            c << "-I" << normalize_path(i);

        // generator
        c << ("--" + generator + "_out=" + to_string(normalize_path(outdir)));

        // plugin
        if (plugin)
        {
            c << [c = c.getCommand().get(), plugin = plugin, generator = generator, &ctx = t.getContext()]()
            {
                path p;
                if (auto t = plugin->getTarget().as<NativeExecutedTarget *>())
                {
                    p = t->getOutputFile();
                }
                else if (auto t = plugin->getTarget().as<PredefinedTarget *>())
                {
                    p = t->getInterfaceSettings()["output_file"].getPathValue(ctx.getLocalStorage());
                }
                else
                    throw SW_RUNTIME_ERROR("no grpc_cpp_plugin resolved (missing target code)");
                c->addInput(p);
                return "--plugin=protoc-gen-" + generator + "=" + to_string(normalize_path(p));
            }
            ;

            t.addDummyDependency(plugin);
            std::dynamic_pointer_cast<::sw::driver::Command>(c.getCommand())->addProgramDependency(plugin);
        }

        // input
        c << cmd::in(normalize_path(input)); // must be normalized
        c << cmd::end();

        // create deps output dir!
        c->output_dirs.insert(deps_file.parent_path());

        auto o = outdir / rel_input.parent_path() / rel_input.stem();
        for (auto &e : exts)
            c << cmd::out(path(o) += e);
        return c.getCommand();
    }
};

struct ProtobufData : ProtocData
{
    bool public_protobuf = false;
};

static auto gen_protobuf_cpp(const DependencyPtr &protobuf_base, NativeExecutedTarget &t,
    const path &f, const ProtobufData &data = {})
{
#if SW_CPP_DRIVER_API_VERSION > 1
    auto protoc = std::make_shared<Dependency>(sw::UnresolvedPackageName{
        protobuf_base->getUnresolvedPackageId().getName().getPath() / "protoc",
        protobuf_base->getUnresolvedPackageId().getName().getRange()});
    auto protobuf = std::make_shared<Dependency>(sw::UnresolvedPackageName{
        protobuf_base->getUnresolvedPackageId().getName().getPath() / "protobuf",
        protobuf_base->getUnresolvedPackageId().getName().getRange()});
#else
    auto protoc = std::make_shared<Dependency>(sw::UnresolvedPackage{protobuf_base->package.getPath() / "protoc", protobuf_base->package.getRange()});
    auto protobuf = std::make_shared<Dependency>(sw::UnresolvedPackage{protobuf_base->package.getPath() / "protobuf", protobuf_base->package.getRange()});
#endif

    ProtocData d = data;
    d.input = f;
    d.generator = "cpp";
    d.exts = {".pb.cc", ".pb.h"};
    auto c = d.generate(protoc, t);

    t += protobuf;
    if (data.public_protobuf)
        t.Public += protobuf;

    return std::tuple{ protoc, c };
}

#pragma sw header off

void build(Solution &s)
{
    auto &p = s.addProject("google.protobuf", "3.14.0");
    p += Git("https://github.com/protocolbuffers/protobuf", "v{v}");

    auto win_or_mingw = [](auto &t)
    {
        return
            t.getBuildSettings().TargetOS.Type == OSType::Windows/* ||
                                                                 t.getBuildSettings().TargetOS.Type == OSType::Mingw*/
            ;
    };

    auto &protobuf_lite = p.addTarget<LibraryTarget>("protobuf_lite");
    protobuf_lite += cpp11;
    protobuf_lite.ImportFromBazel = true;
    if (!win_or_mingw(protobuf_lite))
        protobuf_lite.ExportAllSymbols = true;
    protobuf_lite += "src/google/protobuf/.*\\.h"_rr;
    protobuf_lite += "src/google/protobuf/.*\\.inc"_rr;
    protobuf_lite += "src/google/protobuf/io/strtod.cc";
    protobuf_lite += "src/google/protobuf/parse_context.cc";
    protobuf_lite += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
    protobuf_lite.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
    if (!win_or_mingw(protobuf_lite))
    {
        protobuf_lite.Public += "HAVE_PTHREAD"_d;
        protobuf_lite += "pthread"_slib;
    }
    protobuf_lite.patch("src/google/protobuf/parse_context.h", "PROTOBUF_EXPORT_TEMPLATE_DEFINE", "//PROTOBUF_EXPORT_TEMPLATE_DEFINE");

    auto &protobuf = p.addTarget<LibraryTarget>("protobuf");
    protobuf += cpp11;
    protobuf.ImportFromBazel = true;
    if (!win_or_mingw(protobuf))
        protobuf.ExportAllSymbols = true;
    protobuf.BazelNames.insert("protobuf_lite");
    protobuf += "src/.*\\.h"_rr;
    protobuf += "src/.*\\.inc"_rr;
    protobuf += "src/google/protobuf/io/strtod.cc";
    protobuf += "src/google/protobuf/parse_context.cc";
    protobuf.Private += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
    protobuf.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
    protobuf.Public += "org.sw.demo.madler.zlib"_dep;
    if (!win_or_mingw(protobuf))
    {
        protobuf.Public += "HAVE_PTHREAD"_d;
        protobuf += "pthread"_slib;
    }
    protobuf.patch("src/google/protobuf/parse_context.h", "PROTOBUF_EXPORT_TEMPLATE_DEFINE", "//PROTOBUF_EXPORT_TEMPLATE_DEFINE");

    auto &protoc_lib = p.addTarget<LibraryTarget>("protoc_lib");
    protoc_lib += cpp11;
    protoc_lib.ImportFromBazel = true;
    if (!win_or_mingw(protoc_lib))
        protoc_lib.ExportAllSymbols = true;
    protoc_lib.Private += sw::Shared, "LIBPROTOC_EXPORTS"_d;
    protoc_lib.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
    protoc_lib.Public += protobuf;

    auto &protoc = p.addTarget<ExecutableTarget>("protoc");
    protoc += cpp11;
    protoc.ImportFromBazel = true;
    if (!win_or_mingw(protoc))
        protoc.ExportAllSymbols = true;
    protoc +=
        "src/google/protobuf/any.proto",
        "src/google/protobuf/api.proto",
        "src/google/protobuf/compiler/plugin.proto",
        "src/google/protobuf/descriptor.proto",
        "src/google/protobuf/duration.proto",
        "src/google/protobuf/empty.proto",
        "src/google/protobuf/field_mask.proto",
        "src/google/protobuf/source_context.proto",
        "src/google/protobuf/struct.proto",
        "src/google/protobuf/timestamp.proto",
        "src/google/protobuf/type.proto",
        "src/google/protobuf/wrappers.proto";
    protoc.Public += protoc_lib;
}
