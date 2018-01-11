void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo.google");
    auto &p = s.addProject("protobuf", "3.5.1");
    p.Source = Git("https://github.com/google/protobuf", "v{v}");

    auto &protobuf_lite = p.addTarget<LibraryTarget>("protobuf_lite");
    protobuf_lite.ImportFromBazel = true;

    protobuf_lite += "src/google/protobuf/.*\\.h"_rr;

    protobuf_lite.Private += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
    protobuf_lite.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;

    auto &protobuf = p.addTarget<LibraryTarget>("protobuf");
    protobuf.ImportFromBazel = true;
    protobuf.BazelNames.insert("protobuf_lite");

    protobuf += "src/.*\\.h"_rr;

    protobuf.Private += sw::Shared, "LIBPROTOBUF_EXPORTS"_d;
    protobuf.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;

    protobuf.Public += "pub.cppan2.demo.madler.zlib"_dep;

    auto &js_embed = p.addTarget<ExecutableTarget>("tools.js_embed");
    js_embed += "src/google/protobuf/compiler/js/embed.cc";

    auto &protoc_lib = p.addTarget<LibraryTarget>("protoc_lib");
    protoc_lib.ImportFromBazel = true;

    protoc_lib += "src/google/protobuf/compiler/js/.*"_rr;
    protoc_lib -=
        "src/google/protobuf/compiler/js/embed.cc"
        "src/google/protobuf/compiler/js/well_known_types_embed.cc"
        ;

    protoc_lib.Public +=
        "src"_id;

    protoc_lib.Private += sw::Shared, "LIBPROTOC_EXPORTS"_d;
    protoc_lib.Public += sw::Shared, "PROTOBUF_USE_DLLS"_d;

    protoc_lib.Public += protobuf;
    (protoc_lib + js_embed)->Dummy = true;

    auto c = std::make_shared<Command>();
    c->program = js_embed.getOutputFile();
    for (auto &f : {
        "src/google/protobuf/compiler/js/well_known_types/any.js",
        "src/google/protobuf/compiler/js/well_known_types/struct.js",
        "src/google/protobuf/compiler/js/well_known_types/timestamp.js" })
    {
        c->args.push_back((protoc_lib.SourceDir / f).string());
        c->addInput(protoc_lib.SourceDir / f);
    }
    c->redirectStdout(protoc_lib.BinaryDir / "well_known_types_embed.cc");
    protoc_lib += protoc_lib.BinaryDir / "well_known_types_embed.cc";

    auto &protoc = p.addTarget<ExecutableTarget>("protoc");
    protoc.ImportFromBazel = true;
    protoc.Public += protoc_lib;
}
