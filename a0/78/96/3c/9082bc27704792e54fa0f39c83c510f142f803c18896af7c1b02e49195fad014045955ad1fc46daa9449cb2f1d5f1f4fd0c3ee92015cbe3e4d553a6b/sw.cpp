#ifdef SW_PRAGMA_HEADER
#pragma sw header on

auto gen_protobuf(NativeExecutedTarget &t, const path &f, bool public_protobuf = false)
{
    auto protoc = THIS_PREFIX "." "google.protobuf.protoc" "-" THIS_VERSION_DEPENDENCY;
    {
        auto d = t + protoc;
        d->Dummy = true;
    }

    auto n = f.filename().stem().u8string();
    auto d = f.parent_path();
    auto bdir = t.BinaryDir;

    auto o = bdir / n;
    auto ocpp = o;
    ocpp += ".pb.cc";
    auto oh = o;
    oh += ".pb.h";

    auto c = std::make_shared<Command>();
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
    t.Storage.push_back(c);

    auto protobuf = THIS_PREFIX "." "google.protobuf.protobuf" "-" THIS_VERSION_DEPENDENCY;
    t += protobuf;
    if (public_protobuf)
        t.Public += protobuf;

    return protoc;
};

#pragma sw header off
#endif

void build(Solution &s)
{
    auto &p = s.addProject("google.protobuf", "3.6.1");
    p += Git("https://github.com/google/protobuf", "v{v}");

    auto &protobuf_lite = p.addTarget<LibraryTarget>("protobuf_lite");
    protobuf_lite.ImportFromBazel = true;
    protobuf_lite += "src/google/protobuf/.*\\.h"_rr;
    protobuf_lite += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
    protobuf_lite.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;

    auto &protobuf = p.addTarget<LibraryTarget>("protobuf");
    protobuf.ImportFromBazel = true;
    protobuf.BazelNames.insert("protobuf_lite");
    protobuf += "src/.*\\.h"_rr;
    protobuf.Private += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
    protobuf.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
    protobuf.Public += "org.sw.demo.madler.zlib"_dep;

    auto &protoc_lib = p.addTarget<LibraryTarget>("protoc_lib");
    protoc_lib.ImportFromBazel = true;
    protoc_lib.Private += sw::Shared, "LIBPROTOC_EXPORTS"_d;
    protoc_lib.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;
    protoc_lib.Public += protobuf;

    auto &protoc = p.addTarget<ExecutableTarget>("protoc");
    protoc.ImportFromBazel = true;
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
