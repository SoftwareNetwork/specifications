#pragma sw header on

static auto gen_protobuf(const DependencyPtr &base, NativeExecutedTarget &t, path f, bool public_protobuf = false, const path &out_dir = {})
{
    auto protoc = std::make_shared<Dependency>(base->package);
    protoc->package.ppath /= "protoc";

    auto protobuf = std::make_shared<Dependency>(base->package);
    protobuf->package.ppath /= "protobuf";

    {
        auto d = t + protoc;
        d->Dummy = true;
    }

    if (!f.is_absolute())
        f = t.SourceDir / f;

    auto n = f.filename().stem().u8string();
    auto d = f.parent_path();

    if (out_dir.is_absolute())
        throw std::logic_error("Make out_dir relative");
    auto bdir = t.BinaryDir / out_dir;

    auto o = bdir / n;
    auto ocpp = o;
    ocpp += ".pb.cc";
    auto oh = o;
    oh += ".pb.h";

    SW_MAKE_COMMAND_AND_ADD(c, t);
    c->setProgram(protoc);
    c->working_directory = bdir;
    c->args.push_back(f.u8string());
    c->args.push_back("--cpp_out=" + bdir.u8string());
    c->args.push_back("-I");
    c->args.push_back(d.u8string());
    c->args.push_back("-I");
    c->pushLazyArg([protoc]()
    {
        return (protoc->getResolvedPackage().getDirSrc2() / "src").u8string();
    });
    c->addInput(f);
    c->addOutput(ocpp);
    c->addOutput(oh);
    t += ocpp, oh;

    t += protobuf;
    if (public_protobuf)
        t.Public += protobuf;

    return std::tuple{ protoc, c };
}

#pragma sw header off

void build(Solution &s)
{
    auto &p = s.addProject("google.protobuf", "3.6.1");
    p += Git("https://github.com/protocolbuffers/protobuf", "v{v}");

    auto &protobuf_lite = p.addTarget<LibraryTarget>("protobuf_lite");
    protobuf_lite.ImportFromBazel = true;
    if (s.Settings.TargetOS.Type != OSType::Windows)
        protobuf_lite.ExportAllSymbols = true;
    protobuf_lite += "src/google/protobuf/.*\\.h"_rr;
    protobuf_lite += "src/google/protobuf/.*\\.inc"_rr;
    protobuf_lite += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
    protobuf_lite.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;

    auto &protobuf = p.addTarget<LibraryTarget>("protobuf");
    protobuf.ImportFromBazel = true;
    if (s.Settings.TargetOS.Type != OSType::Windows)
        protobuf.ExportAllSymbols = true;
    protobuf.BazelNames.insert("protobuf_lite");
    protobuf += "src/.*\\.h"_rr;
    protobuf += "src/.*\\.inc"_rr;
    protobuf.Private += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
    protobuf.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
    protobuf.Public += "org.sw.demo.madler.zlib"_dep;

    if (s.Settings.TargetOS.Type != OSType::Windows)
    {
        protobuf_lite.Public += "HAVE_PTHREAD"_d;
        protobuf.Public += "HAVE_PTHREAD"_d;
    }

    auto &protoc_lib = p.addTarget<LibraryTarget>("protoc_lib");
    protoc_lib.ImportFromBazel = true;
    if (s.Settings.TargetOS.Type != OSType::Windows)
        protoc_lib.ExportAllSymbols = true;
    protoc_lib.Private += sw::Shared, "LIBPROTOC_EXPORTS"_d;
    protoc_lib.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
    protoc_lib.Public += protobuf;

    auto &protoc = p.addTarget<ExecutableTarget>("protoc");
    protoc.ImportFromBazel = true;
    if (s.Settings.TargetOS.Type != OSType::Windows)
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
