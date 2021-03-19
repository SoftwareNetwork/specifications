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
        t += FileRegex(t.getFile(protos, dir), ".*\\.proto", true);

        ProtobufData d;
        d.outdir = t.BinaryDir;
        d.public_protobuf = true;
        d.addIncludeDirectory(t.getFile(protos, ""));
        for (const auto &[p, _] : t[FileRegex(t.getFile(protos, dir), ".*\\.proto", false)])
            gen_grpc_cpp("org.sw.demo.google.protobuf"_dep, "org.sw.demo.google.grpc.cpp.plugin"_dep, t, p, d);
        t.Public += "org.sw.demo.google.grpc.core.lib"_dep;
    };

    auto &rpc = p.addProtoLibrary("rpc");
    process_pb(rpc, "google/rpc");

    auto &type = p.addProtoLibrary("type");
    process_pb(type, "google/type");

    auto &api = p.addProtoLibrary("api");
    process_pb(api, "google/api");
    process_pb(api, "google/logging");
    process_pb(api, "google/longrunning");
    api.Public += rpc;

    auto &cloud = p.addDirectory("cloud");
    auto &vision = cloud.addProtoLibrary("vision");
    process_pb(vision, "google/cloud/vision");
    vision.Public += api, type;
}
