#pragma sw header on

#pragma sw require header org.sw.demo.google.protobuf.protoc-3

static void gen_grpc_cpp(const DependencyPtr &protoc_in, const DependencyPtr &grpc_cpp_plugin,
    NativeExecutedTarget &t, const path &f, const ProtobufData &data = {})
{
    auto [protoc, _] = gen_protobuf_cpp(protoc_in, t, f, data);

    ProtocData d = data;
    d.input = f;
    d.generator = "grpc";
    d.exts = {".grpc.pb.cc", ".grpc.pb.h"};
    d.plugin = grpc_cpp_plugin;
    auto c = d.generate(protoc, t);

#if defined(SW_CPP_DRIVER_API_VERSION)
    t.addDummyDependency(grpc_cpp_plugin);
#else
    (t + grpc_cpp_plugin)->setDummy(true);
#endif
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
            t.Protected += "_WIN32_WINNT=0x0601"_def;

        t.Public += "include"_id;
        t.Public += "."_id;

        t.Public += "org.sw.demo.madler.zlib"_dep;
        t.Public += "org.sw.demo.c_ares"_dep;
        t.Public += "org.sw.demo.nanopb"_dep;
        t.Public += "org.sw.demo.openssl.ssl"_dep;
    }

    auto &grpc_plugin_support = p.addStaticLibrary("plugin_support");
    {
        auto &t = grpc_plugin_support;
        t += "src/compiler/.*\\.h"_rr;
        t += "src/compiler/.*_generator\\.cc"_rr;

        t.Public += "."_id;

        t += core;
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

    auto &cpp = p.addStaticLibrary("cpp");
    {
        auto &t = core;
        cpp += "src/cpp/.*"_rr;

        cpp += "."_id;
        cpp.Public += "src"_id;

        cpp.Public += proto;
        cpp.Public += "org.sw.demo.census.opencensus.cpp"_dep;
    }
}
