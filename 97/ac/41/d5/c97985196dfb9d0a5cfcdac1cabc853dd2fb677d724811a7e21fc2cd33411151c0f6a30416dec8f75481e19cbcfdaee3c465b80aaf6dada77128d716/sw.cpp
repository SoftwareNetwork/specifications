#pragma sw require header org.sw.demo.google.grpc.cpp.plugin

#define addProtoLibrary addStaticLibrary

void build(Solution &s)
{
    auto &p = s.addProject("google.googleapis", "master");
    p += Git("https://github.com/googleapis/googleapis");

    auto &protos = p.addProtoLibrary("protos");
    protos += "google/.*\\.proto"_rr;

    auto process_pb = [&protos](auto &t, auto &&dir)
    {
        t += cpp17;
        t += FileRegex(t.getFile(protos, dir), ".*\\.proto", true);

        ProtobufData d;
        d.outdir = t.BinaryDir;
        d.public_protobuf = true;
        d.addIncludeDirectory(t.getFile(protos, ""));
        for (const auto &[p, _] : t[FileRegex(t.getFile(protos, dir), ".*\\.proto", false)])
            gen_grpc_cpp("org.sw.demo.google.protobuf"_dep, "org.sw.demo.google.grpc.cpp.plugin"_dep, t, p, d);
        t.Public += "org.sw.demo.google.grpc.core.lib"_dep;
    };

    auto add_simple_lib = [&](auto &&name) -> decltype(auto) {
        auto &t = p.addProtoLibrary(name);
        process_pb(t, "google/"s + name);
        return t;
    };

    #define ADD_LIB(x) auto &x = add_simple_lib(#x)

    ADD_LIB(rpc);
    ADD_LIB(type);

    auto &api = p.addProtoLibrary("api");
    process_pb(api, "google/api");
    process_pb(api, "google/logging");
    process_pb(api, "google/longrunning");
    api.Public += rpc;

    ADD_LIB(iam);
    iam.Public += api, type;
    ADD_LIB(storage);
    storage.Public += iam;

    auto &cloud = p.addDirectory("cloud");
    auto &vision = cloud.addProtoLibrary("vision");
    process_pb(vision, "google/cloud/vision");
    vision.Public += api, type;
}
