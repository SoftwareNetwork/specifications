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
    auto &p = s.addProject("google.grpc", "1.49.0");
    p += Git("https://github.com/grpc/grpc", "v{v}");

    auto cppstd = cpp17;

    auto &core = p.addStaticLibrary("core.lib");

    auto &dupb = p.addDirectory("third_party.upb");
    auto &upb = dupb.addStaticLibrary("upb");
    auto &textformat = dupb.addStaticLibrary("textformat");
    auto &json = dupb.addStaticLibrary("json");
    {
        upb.setRootDirectory("third_party/upb");
        upb += "upb/decode.*"_rr;
        upb += "upb/encode.*"_rr;
        upb += "upb/msg.*"_rr;
        upb -= "upb/msg_test.*"_rr;
        upb += "upb/table.*"_rr;
        upb += "upb/upb.*"_rr;

        auto &port = dupb.addStaticLibrary("port");
        port.setRootDirectory("third_party/upb");
        port += "upb/port_def.inc";
        port += "upb/port_undef.inc";
        upb.Public += port;

        auto &utf8_range = dupb.addStaticLibrary("utf8_range");
        utf8_range.setRootDirectory("third_party/upb");
        utf8_range += "third_party/utf8_range/.*"_rr;
        upb.Public += utf8_range;

        auto &reflection = dupb.addStaticLibrary("reflection");
        reflection.setRootDirectory("third_party/upb");
        reflection += "upb/def.*"_rr;
        reflection += "upb/reflection.*"_rr;
        //reflection += "cmake/google/.*"_rr;
        reflection.Public += "."_idir;
        //reflection.Public += "cmake"_idir;
        reflection.Public += upb;
        (reflection + core)->IncludeDirectoriesOnly = true;

        textformat.setRootDirectory("third_party/upb");
        textformat += "upb/text_encode.*"_rr;
        textformat.Public += reflection;
        (textformat + core)->IncludeDirectoriesOnly = true;

        json.setRootDirectory("third_party/upb");
        json += "upb/json.*"_rr;
        json.Public += reflection;
        (json + core)->IncludeDirectoriesOnly = true;
    }

    // core
    {
        auto &t = core;
        t += cppstd;
        t += "include/.*"_rr;
        t += "src/core/lib/.*"_rr;
        t += "src/core/ext/upb-generated/.*"_rr;

        //if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            //t.Public += "_WIN32_WINNT=0x0601"_def;

        t.Public += "include"_id;
        t.Public += "."_id;
        t.Public += "src/core/ext/upb-generated"_idir;

        t.Public += "org.sw.demo.madler.zlib"_dep;
        t.Public += "org.sw.demo.openssl.ssl"_dep;
        t.Public += "org.sw.demo.nanopb"_dep;
        t.Public += "org.sw.demo.google.abseil"_dep;
        t.Public += upb;

        t.patch("include/grpc/impl/codegen/port_platform.h",
            "#if !(defined(__has_feature) && __has_feature(cxx_thread_local))",
            "#if 1//!(defined(__has_feature) && __has_feature(cxx_thread_local))"
        );
    }

    auto &grpc_plugin_support = p.addStaticLibrary("plugin_support");
    {
        auto &t = grpc_plugin_support;
        t += cppstd;
        t += "src/compiler/.*\\.h"_rr;
        t += "src/compiler/.*_generator\\.cc"_rr;

        t.Public += "."_id;

        t.Public += core;
        t += "org.sw.demo.google.protobuf.protobuf"_dep;
    }

    auto &grpc_cpp_plugin = p.addExecutable("cpp.plugin");
    {
        auto &t = grpc_cpp_plugin;
        t += cppstd;
        t += "src/compiler/cpp_plugin.cc";
        t += grpc_plugin_support;
        t += "org.sw.demo.google.protobuf.protoc_lib"_dep;
    }

    auto &proto = p.addStaticLibrary("proto");
    {
        auto &t = proto;
        t += cppstd;
        t += "src/proto/.*\\.proto"_rr;
        //t -= "src/proto/grpc/testing/.*\\.proto"_rr;
        t -= "src/proto/grpc/status/.*\\.proto"_rr;
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

    auto &grpc_address_sorting = p.addStaticLibrary("third_party.address_sorting");
    {
        auto &t = grpc_address_sorting;
        t += cppstd;
        t += "third_party/address_sorting/.*\\.[hc]"_rr;
        t.Public += "third_party/address_sorting/include"_idir;
    }

    auto &core_plugin_registry = p.addStaticLibrary("core.plugin_registry");
    {
        auto &t = core_plugin_registry;
        t += cppstd;
        t += "src/core/plugin_registry/.*"_rr;

        t.Public += "."_id;

        t.Public += core;
    }

    auto &core_ext = p.addStaticLibrary("core.ext");
    {
        auto &t = core_ext;
        t += cppstd;
        t += "src/core/ext/.*"_rr;
        t -= "src/core/ext/upb-generated/.*"_rr;
        //t -= "src/core/ext/upbdefs-generated/.*"_rr;
        t -= "src/core/ext/transport/cronet/plugin_registry/.*"_rr;
        t -= "src/core/ext/filters/load_reporting/.*"_rr;
        t += "third_party/objective_c/Cronet/.*\\.h"_rr;

        t += "GRPC_ARES"_def;
        t.Public += "."_id;
        t += "src/core/ext/upbdefs-generated"_id;

        t.Public += proto;
        t.Public += grpc_address_sorting;
        t.Public += core_plugin_registry;
        t.Public += "org.sw.demo.Cyan4973.xxHash"_dep;
        t.Public += "org.sw.demo.c_ares"_dep;
        t.Public += "org.sw.demo.google.re2"_dep;
        t.Public += "org.sw.demo.census.opencensus.cpp"_dep;
        t.Public += textformat;
        t.Public += json;
        (core + core_ext)->IncludeDirectoriesOnly = true;
    }

    auto &core_tsi = p.addStaticLibrary("core.tsi");
    {
        auto &t = core_tsi;
        t += cppstd;
        t += "src/core/tsi/.*"_rr;

        t.Public += "."_id;

        t.Public += core;
        t.Public += proto;
        (core + core_tsi)->IncludeDirectoriesOnly = true;
    }

    auto &cpp = p.addStaticLibrary("cpp");
    {
        auto &t = cpp;
        t += cppstd;
        t += "src/cpp/.*"_rr;
        t -= "src/cpp/common/insecure_create_auth_context.cc"_rr;

        t.Public += "."_id;
        t.Public += "src"_id;

        t.Public += core_ext, core_tsi;
        (core_ext + t)->IncludeDirectoriesOnly = true;
    }
}
