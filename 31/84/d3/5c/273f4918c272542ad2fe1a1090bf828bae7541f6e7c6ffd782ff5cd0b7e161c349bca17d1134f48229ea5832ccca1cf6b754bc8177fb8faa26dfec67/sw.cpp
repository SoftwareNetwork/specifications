#pragma sw header on

#pragma sw require header org.sw.demo.google.protobuf.protoc

static void gen_grpc_cpp(const DependencyPtr &protobuf_base, const DependencyPtr &grpc_cpp_plugin,
    NativeExecutedTarget &t, const path &f, const ProtobufData &data = {})
{
    auto [protoc, _] = gen_protobuf_cpp(protobuf_base, t, f, data);

    ProtocData d = data;
    d.input = f;
    d.generator = "grpc";
    d.exts = {".grpc.pb.cc", ".grpc.pb.h"};
    d.plugin = grpc_cpp_plugin;
    d.generate(protoc, t);
}

#pragma sw header off

void build(Solution &s)
{
    auto &p = s.addProject("google.grpc", "1.23.0");
    p += Git("https://github.com/grpc/grpc", "v{v}");

    auto &core = p.addStaticLibrary("core.lib");
    {
        auto &t = core;
        t += "include/.*"_rr;
        t += "src/core/lib/.*"_rr;

        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            t.Public += "_WIN32_WINNT=0x0601"_def;

        t.Public += "include"_id;
        t.Public += "."_id;

        t.Public += "org.sw.demo.madler.zlib"_dep;
        t.Public += "org.sw.demo.openssl.ssl"_dep;
        t.Public += "org.sw.demo.nanopb"_dep;
    }

    auto &grpc_plugin_support = p.addStaticLibrary("plugin_support");
    {
        auto &t = grpc_plugin_support;
        t += "src/compiler/.*\\.h"_rr;
        t += "src/compiler/.*_generator\\.cc"_rr;

        t.Public += "."_id;

        t.Public += core;
        t += "org.sw.demo.google.protobuf.protobuf"_dep;
    }

    auto &grpc_cpp_plugin = p.addExecutable("cpp.plugin");
    {
        auto &t = grpc_cpp_plugin;
        t += "src/compiler/cpp_plugin.cc";
        t += grpc_plugin_support;
        t += "org.sw.demo.google.protobuf.protoc_lib"_dep;
    }

    auto &proto = p.addStaticLibrary("proto");
    {
        auto &t = proto;
        t += "src/proto/.*\\.proto"_rr;
        t -= "src/proto/grpc/testing/.*\\.proto"_rr;
        t.Public += core;
        ProtobufData d;
        d.public_protobuf = true;
        for (auto &[p, sf] : t["src/proto/.*\\.proto"_rr])
        {
            if (sf->skip)
                continue;
            gen_grpc_cpp("org.sw.demo.google.protobuf"_dep, std::make_shared<Dependency>(grpc_cpp_plugin), t, p, d);
        }
    }

    auto &grpc_address_sorting = p.addTarget<StaticLibraryTarget>("third_party.address_sorting");
    grpc_address_sorting += "third_party/address_sorting/.*\\.[hc]"_rr;
    grpc_address_sorting.Public += "third_party/address_sorting/include"_idir;

    auto &core_plugin_registry = p.addStaticLibrary("core.plugin_registry");
    {
        auto &t = core_plugin_registry;
        t += "src/core/plugin_registry/grpc_plugin_registry.cc";
        t.Public += "."_id;

        t.Public += core;
    }

    auto &core_ext = p.addStaticLibrary("core.ext");
    {
        auto &t = core_ext;
        t += "src/core/ext/.*"_rr;
        t += "third_party/objective_c/Cronet/.*\\.h"_rr;
        t.Public += "."_id;
        t -= "src/core/ext/upb-generated/.*"_rr;
        t -= "src/core/ext/transport/cronet/plugin_registry/.*"_rr;

        t += "GRPC_ARES"_def;

        t.Public += proto;
        t.Public += grpc_address_sorting;
        t.Public += core_plugin_registry;
        t.Public += "org.sw.demo.c_ares"_dep;
        t.Public += "org.sw.demo.google.protocolbuffers.upb.upb-master"_dep;
        t.Public += "org.sw.demo.census.opencensus.cpp"_dep;
        (core + core_ext)->IncludeDirectoriesOnly = true;
    }

    auto &core_tsi = p.addStaticLibrary("core.tsi");
    {
        auto &t = core_tsi;
        t += "src/core/tsi/.*"_rr;
        t.Public += "."_id;

        t.Public += proto;
        (core + core_tsi)->IncludeDirectoriesOnly = true;
    }

    auto &cpp = p.addStaticLibrary("cpp");
    {
        auto &t = core;
        cpp += "src/cpp/.*"_rr;

        cpp.Public += "."_id;
        cpp.Public += "src"_id;

        cpp.Public += core_ext, core_tsi;
        (core_ext + cpp)->IncludeDirectoriesOnly = true;
    }
}
