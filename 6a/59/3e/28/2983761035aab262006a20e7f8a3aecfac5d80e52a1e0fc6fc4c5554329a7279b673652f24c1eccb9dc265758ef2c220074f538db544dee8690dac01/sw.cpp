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
    auto &p = s.addProject("google.grpc", "1.76.0");
    p += Git("https://chromium.googlesource.com/external/github.com/grpc/grpc", "v{v}");

    auto cppstd = cpp20;

    auto &core = p.addStaticLibrary("core.lib");

    auto &dupb = p.addDirectory("third_party.upb");
    auto &upb = dupb.addStaticLibrary("upb");
    {
        upb.setRootDirectory("third_party");
        upb += "upb/upb/.*"_rr;
        upb -= "upb/upb/.*test.*"_rr;
        upb -= "upb/upb/.*benchmark.*"_rr;
        upb -= "upb/upb/cmake/.*"_rr;
        upb -= "upb/upb/conformance/.*"_rr;
        //upb -= "upb/upb/reflection/.*"_rr;
        //upb -= "upb/upb/reflection/internal/.*"_rr;
        upb -= "upb/upb/reflection/stage0/google/protobuf/descriptor.upb.c";
        upb += "UPB_BOOTSTRAP_STAGE0"_def;
        upb.Public += "."_idir;
        upb.Public += "upb"_idir;
        //upb += "upb/upb/reflection/stage0"_idir;
        upb += "upb/upb/reflection/cmake"_idir;
        //upb.Public += "org.sw.demo.google.protobuf.protobuf"_dep;
        //(upb + core)->IncludeDirectoriesOnly = true;
        //upb.Public += "org.sw.demo.google.protocolbuffers.utf8_range-main"_dep;
        upb.Public += "org.sw.demo.google.protobuf.utf8_range"_dep;
        //upb += "utf8_range/utf8_range.*"_rr;
        //upb.Public += "utf8_range"_idir;
        // or change to float
        upb.patch("upb/upb/message/internal/message.c", "const double kUpb_NaN = NAN;", "//const  double kUpb_NaN = NAN;");
    }

    // core
    {
        auto &t = core;
        t += cppstd;
        t += "include/.*"_rr;
        t += "src/core/lib/.*"_rr;
        t += "src/core/ext/upb-gen/.*"_rr;
        t += "src/core/ext/upbdefs-gen/.*"_rr;
        t += "src/core/ext/filters/fault_injection/.*"_rr;
        t += "src/core/ext/filters/gcp_authentication/.*"_rr;
        t += "src/core/ext/filters/rbac/.*"_rr;
        t += "src/core/ext/filters/stateful_session/.*"_rr;
        t += "src/core/call/.*"_rr;
        t += "src/core/client_channel/.*"_rr;
        t += "src/core/config/.*"_rr;
        t += "src/core/filter/.*"_rr;
        t += "src/core/service_config/.*"_rr;
        t += "src/core/resolver/.*"_rr;
        t += "src/core/channelz/.*"_rr;
        t += "src/core/handshaker/.*"_rr;
        t += "src/core/server/.*"_rr;
        t += "src/core/xds/.*"_rr;
        t += "src/core/util/.*"_rr;
        t += "src/core/telemetry/.*"_rr;
        t += "src/core/credentials/.*"_rr;
        t += "src/core/transport/.*"_rr;
        //t -= "src/core/resolver/xds/.*"_rr;
        t += "src/core/load_balancing/.*"_rr;
        //t -= "src/core/load_balancing/xds/.*"_rr;

        //if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            //t.Public += "_WIN32_WINNT=0x0601"_def;

        t.Public += "include"_id;
        t.Public += "."_id;
        t.Public += "src/core/ext/upb-gen"_idir;
        t.Public += "src/core/ext/upbdefs-gen"_idir;

        t.Public += "org.sw.demo.madler.zlib"_dep;
        t.Public += "org.sw.demo.openssl.ssl"_dep;
        t.Public += "org.sw.demo.nanopb"_dep;
        t.Public += "org.sw.demo.google.abseil"_dep;
        t.Public += "org.sw.demo.opentelemetry"_dep;
        t.Public += "org.sw.demo.google.re2"_dep;
        t.Public += upb;
        //t.Public += "org.sw.demo.google.protobuf.upb"_dep;

        t.patch("include/grpc/impl/codegen/port_platform.h",
            "#if !(defined(__has_feature) && __has_feature(cxx_thread_local))",
            "#if 1//!(defined(__has_feature) && __has_feature(cxx_thread_local))"
        );

        t -= "src/core/lib/event_engine/windows/.*"_rr;
        t -= "src/core/lib/event_engine/posix_engine/.*"_rr;
        if (t.getBuildSettings().TargetOS.is(OSType::Windows)) {
            t += "src/core/lib/event_engine/windows/.*"_rr;
        } else {
            t += "src/core/lib/event_engine/posix_engine/.*"_rr;
        }
        if (t.getBuildSettings().TargetOS.isApple()) {
            t -= "src/core/lib/event_engine/cf_engine/.*"_rr;
            t.add("src/core/lib/event_engine/cf_engine/cf_engine.cc", ".mm");
            t.add("src/core/lib/event_engine/cf_engine/cfstream_endpoint.cc", ".mm");
            t.add("src/core/lib/event_engine/cf_engine/dns_service_resolver.cc", ".mm");
        }
        t += "src/core/lib/event_engine/posix_engine/timer.*"_rr;

        t += "src/proto/.*\\.proto"_rr;
        t -= "src/proto/grpc/.*\\.proto"_rr;
        t += "src/core/ext/transport/chaotic_good/chaotic_good_frame.proto"_rr;
        ProtobufData d;
        d.public_protobuf = true;
        for (auto &[p, sf] : t["src/.*\\.proto"_rr])
        {
            if (sf->skip)
                continue;
            gen_protobuf_cpp("org.sw.demo.google.protobuf"_dep, t, p, d);
        }
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
        t += "src/compiler/proto_parser_helper.cc";
        t += grpc_plugin_support;
        t += "org.sw.demo.google.protobuf.protoc_lib"_dep;
    }

    auto &proto = p.addStaticLibrary("proto");
    {
        auto &t = proto;
        t += cppstd;
        t += "src/proto/grpc/.*\\.proto"_rr;
        t -= "src/proto/grpc/testing/.*\\.proto"_rr;
        t -= "src/proto/grpc/status/.*\\.proto"_rr;
        t.Public += core;
        ProtobufData d;
        d.public_protobuf = true;
        for (auto &[p, sf] : t["src/.*\\.proto"_rr])
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
        t -= "src/core/ext/upb-gen/.*"_rr;
        t -= "src/core/ext/filters/fault_injection/.*"_rr;
        t -= "src/core/ext/filters/gcp_authentication/.*"_rr;
        t -= "src/core/ext/filters/rbac/.*"_rr;
        t -= "src/core/ext/filters/stateful_session/.*"_rr;
        //t -= "src/core/ext/upbdefs-generated/.*"_rr;
        //t -= "src/core/ext/transport/cronet/plugin_registry/.*"_rr;
        //t -= "src/core/ext/filters/load_reporting/.*"_rr;
        //t += "third_party/objective_c/Cronet/.*\\.h"_rr;

        t += "GRPC_ARES"_def;
        t.Public += "."_id;
        t += "src/core/ext/upbdefs-gen"_id;

        t.Public += proto;
        t.Public += grpc_address_sorting;
        t.Public += core_plugin_registry;
        t.Public += "org.sw.demo.Cyan4973.xxHash"_dep;
        t.Public += "org.sw.demo.c_ares"_dep;
        t.Public += "org.sw.demo.census.opencensus.cpp"_dep;
        (core + core_ext)->IncludeDirectoriesOnly = true;
        (core_plugin_registry + core_ext)->IncludeDirectoriesOnly = true;

        //t.patch("third_party/objective_c/Cronet/bidirectional_stream_c.h", "(WIN32)", "(_WIN32)");
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
        (core_tsi + core_ext)->IncludeDirectoriesOnly = true;
    }

    auto &cpp = p.addStaticLibrary("cpp");
    {
        auto &t = cpp;
        t += cppstd;
        t += "src/cpp/.*"_rr;
        t -= "src/cpp/common/insecure_create_auth_context.cc";
        t -= "src/cpp/server/admin/.*"_rr;
        t -= "src/cpp/server/csds/.*"_rr;
        t -= "src/cpp/ext/gcp.*"_rr;
        //t -= "src/cpp/ext/gcp/observability.*"_rr;
        t -= "src/cpp/ext/otel.*"_rr;
        t -= "src/cpp/ext/csm.*"_rr;

        t.Public += "."_id;
        t.Public += "src"_id;

        t.Public += core, core_ext, core_tsi;
        (core_ext + t)->IncludeDirectoriesOnly = true;
    }
}
