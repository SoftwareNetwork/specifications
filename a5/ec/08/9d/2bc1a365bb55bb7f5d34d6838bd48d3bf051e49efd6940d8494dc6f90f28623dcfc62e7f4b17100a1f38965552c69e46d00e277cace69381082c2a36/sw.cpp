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
            hash_combine(h, plugin->getPackage().toString());
        return std::to_string(h);
    }

    auto generate(const DependencyPtr &protoc, NativeExecutedTarget &t)
    {
        if (idirs.empty())
        {
            addIncludeDirectory(t.SourceDir);
            //throw SW_RUNTIME_ERROR("protoc: empty include dirs, provide atleast one");
        }
        if (input.is_absolute())
        {
            for (auto &i : idirs)
            {
                if (is_under_root(input, i))
                {
                    input = input.lexically_relative(i);
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

#if defined(SW_CPP_DRIVER_API_VERSION)
        auto deps_file = t.BinaryDir.parent_path() / "obj" / (input.filename().u8string() + "." + getHash() + ".d");
        auto gc = std::make_shared<::sw::driver::GNUCommand>(t.getSolution().getContext());
        gc->deps_file = deps_file;
        gc->output_dirs.insert(gc->deps_file.parent_path());
#endif

        auto c = t.addCommand(
#if defined(SW_CPP_DRIVER_API_VERSION)
            gc
#endif
        );
        c << cmd::prog(protoc);

#if defined(SW_CPP_DRIVER_API_VERSION)
        // deps file
        c << ("--dependency_out=" + normalize_path(deps_file));
#endif

        // idirs first
        for (auto &i : idirs)
            c << "-I" << normalize_path(i);

        // generator
        c << ("--" + generator + "_out=" + normalize_path(outdir));

        // plugin
        if (plugin)
        {
            c << [c = c.c.get(), plugin = plugin, generator = generator]()
            {
#if defined(SW_CPP_DRIVER_API_VERSION)
                auto t = plugin->getTarget().as<NativeExecutedTarget*>();
#else
                auto t = (NativeExecutedTarget*)&plugin->getTarget();
#endif
                if (!t)
                    throw SW_RUNTIME_ERROR("no grpc_cpp_plugin resolved");
                auto p = t->getOutputFile();
                c->addInput(p);
                return "--plugin=protoc-gen-" + generator + "=" + p.u8string();
            }
            ;
        }

        // input
        c << cmd::in(normalize_path(input)); // must be normalized
        c << cmd::end();

        auto o = outdir / input.parent_path() / input.stem();
        for (auto &e : exts)
            c << cmd::out(path(o) += e);
        return c.c;
    }
};

struct ProtobufData : ProtocData
{
    bool public_protobuf = false;
};

static auto gen_protobuf_cpp(const DependencyPtr &protobuf_base, NativeExecutedTarget &t,
    const path &f, const ProtobufData &data = {})
{
    auto protoc = std::make_shared<Dependency>(protobuf_base->package);
    protoc->package.ppath /= "protoc";

    auto protobuf = std::make_shared<Dependency>(protobuf_base->package);
    protobuf->package.ppath /= "protobuf";

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
    auto &p = s.addProject("google.protobuf", "3.9.1");
    p += Git("https://github.com/protocolbuffers/protobuf", "v{v}");

    auto &protobuf_lite = p.addTarget<LibraryTarget>("protobuf_lite");
    protobuf_lite.ImportFromBazel = true;
    if (protobuf_lite.getBuildSettings().TargetOS.Type != OSType::Windows)
        protobuf_lite.ExportAllSymbols = true;
    protobuf_lite += "src/google/protobuf/.*\\.h"_rr;
    protobuf_lite += "src/google/protobuf/.*\\.inc"_rr;
    protobuf_lite += "src/google/protobuf/io/strtod.cc";
    protobuf_lite += "src/google/protobuf/parse_context.cc";
    protobuf_lite += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
    protobuf_lite.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
    if (protobuf_lite.getBuildSettings().TargetOS.Type != OSType::Windows)
    {
        protobuf_lite.Public += "HAVE_PTHREAD"_d;
        protobuf_lite += "pthread"_slib;
    }

    auto &protobuf = p.addTarget<LibraryTarget>("protobuf");
    protobuf.ImportFromBazel = true;
    if (protobuf.getBuildSettings().TargetOS.Type != OSType::Windows)
        protobuf.ExportAllSymbols = true;
    protobuf.BazelNames.insert("protobuf_lite");
    protobuf += "src/.*\\.h"_rr;
    protobuf += "src/.*\\.inc"_rr;
    protobuf += "src/google/protobuf/io/strtod.cc";
    protobuf += "src/google/protobuf/parse_context.cc";
    protobuf.Private += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
    protobuf.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
    protobuf.Public += "org.sw.demo.madler.zlib"_dep;
    if (protobuf.getBuildSettings().TargetOS.Type != OSType::Windows)
    {
        protobuf.Public += "HAVE_PTHREAD"_d;
        protobuf += "pthread"_slib;
    }

    auto &protoc_lib = p.addTarget<LibraryTarget>("protoc_lib");
    protoc_lib.ImportFromBazel = true;
    if (protoc_lib.getBuildSettings().TargetOS.Type != OSType::Windows)
        protoc_lib.ExportAllSymbols = true;
    protoc_lib.Private += sw::Shared, "LIBPROTOC_EXPORTS"_d;
    protoc_lib.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
    protoc_lib.Public += protobuf;

    auto &protoc = p.addTarget<ExecutableTarget>("protoc");
    protoc.ImportFromBazel = true;
    if (protoc.getBuildSettings().TargetOS.Type != OSType::Windows)
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
