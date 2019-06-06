#pragma sw header on

#pragma sw require header org.sw.demo.google.protobuf.protoc-3

static void gen_grpc(const DependencyPtr &protoc_in, const DependencyPtr &grpc_cpp_plugin, NativeExecutedTarget &t, const path &f, bool public_protobuf = false)
{
    {
        auto d = t + grpc_cpp_plugin;
        d->setDummy(true);
    }

    auto[protoc, _] = gen_protobuf(protoc_in, t, f, public_protobuf);

    auto n = f.filename().stem().u8string();
    auto d = f.parent_path();
    auto bdir = t.BinaryDir;

    auto o = bdir / n;
    auto ocpp = o;
    ocpp += ".grpc.pb.cc";
    auto oh = o;
    oh += ".grpc.pb.h";

    SW_MAKE_COMMAND_AND_ADD(c, t);
    c->setProgram(protoc);
    c->working_directory = bdir;
    c->args.push_back(f.u8string());
    c->args.push_back("--grpc_out=" + bdir.u8string());
    c->pushLazyArg([c = c.get(), grpc_cpp_plugin]()
    {
        if (!grpc_cpp_plugin->target)
            throw std::runtime_error("Command dependency target was not resolved: " + grpc_cpp_plugin->getPackage().toString());
        auto p = ((NativeExecutedTarget*)grpc_cpp_plugin->target)->getOutputFile();
        c->addInput(p);
        return "--plugin=protoc-gen-grpc=" + p.u8string();
    });
    c->args.push_back("-I");
    c->args.push_back(d.u8string());
    c->args.push_back("-I");
    // we cannot capture binding ref
    c->pushLazyArg([protoc = protoc]()
        {
            return (protoc->getResolvedPackage().getDirSrc2() / "src").u8string();
        });
    c->addInput(f);
    c->addOutput(ocpp);
    c->addOutput(oh);
    t += ocpp, oh;
};

#pragma sw header off

void build(Solution &s)
{
    auto &p = s.addProject("google.grpc", "1.21.2");
    p += Git("https://github.com/grpc/grpc", "v{v}");

    auto setup_grpc = [](auto &t)
    {
        t.ImportFromBazel = true;
        t.BazelTargetFunction = "grpc_cc_library";
        t.Public.IncludeDirectories.insert(t.SourceDir);
        t.Public.IncludeDirectories.insert(t.SourceDir / "include");
    };

    auto &grpcpp_config_proto = p.addTarget<StaticLibraryTarget>("grpcpp_config_proto");
    grpcpp_config_proto.BazelTargetName = "grpc++_config_proto";
    setup_grpc(grpcpp_config_proto);

    auto &grpc_plugin_support = p.addTarget<StaticLibraryTarget>("grpc_plugin_support");
    setup_grpc(grpc_plugin_support);
    grpc_plugin_support.Public += grpcpp_config_proto, "org.sw.demo.google.protobuf.protoc_lib-3"_dep;

    auto &grpc_cpp_plugin = p.addTarget<ExecutableTarget>("grpc_cpp_plugin");
    setup_grpc(grpc_cpp_plugin);
    grpc_cpp_plugin.BazelTargetFunction = "grpc_proto_plugin";
    grpc_cpp_plugin.Public += grpc_plugin_support;

    auto &gpr_codegen = p.addTarget<StaticLibraryTarget>("gpr_codegen");
    setup_grpc(gpr_codegen);
    if (gpr_codegen.getSettings().TargetOS.Type == OSType::Windows)
        gpr_codegen.Public += "_WIN32_WINNT=0x0600"_d;

    auto &gpr_base = p.addTarget<StaticLibraryTarget>("gpr_base");
    setup_grpc(gpr_base);
    gpr_base.Public += gpr_codegen;

    auto &gpr = p.addTarget<StaticLibraryTarget>("gpr");
    setup_grpc(gpr);
    gpr.Public += gpr_base;

    auto &grpc_nanopb = p.addTarget<StaticLibraryTarget>("third_party.nanopb");
    grpc_nanopb += "third_party/nanopb/[^/]*\\.[hc]"_rr;
    grpc_nanopb.Public += "third_party/nanopb"_idir;
    grpc_nanopb.Public += "PB_FIELD_32BIT"_d;

    auto &grpc_address_sorting = p.addTarget<StaticLibraryTarget>("third_party.address_sorting");
    grpc_address_sorting += "third_party/address_sorting/.*\\.[hc]"_rr;
    grpc_address_sorting.Public += "third_party/address_sorting/include"_idir;

    auto &atomic = p.addTarget<StaticLibraryTarget>("atomic");
    setup_grpc(atomic);
    atomic.Public += gpr;

    auto &grpc_codegen = p.addTarget<StaticLibraryTarget>("grpc_codegen");
    setup_grpc(grpc_codegen);
    grpc_codegen.Public += gpr_codegen;

    auto &grpc_trace = p.addTarget<StaticLibraryTarget>("grpc_trace");
    setup_grpc(grpc_trace);
    grpc_trace.Public += gpr, grpc_codegen;

    auto &inlined_vector = p.addTarget<StaticLibraryTarget>("inlined_vector");
    setup_grpc(inlined_vector);
    inlined_vector.Public += gpr_base;

    auto &debug_location = p.addTarget<StaticLibraryTarget>("debug_location");
    setup_grpc(debug_location);

    auto &optional = p.addTarget<StaticLibraryTarget>("optional");
    setup_grpc(optional);
    optional.Public += gpr_base;

    auto &ref_counted_ptr = p.addTarget<StaticLibraryTarget>("ref_counted_ptr");
    setup_grpc(ref_counted_ptr);
    gpr_base.Public += gpr_base;

    auto &ref_counted = p.addTarget<StaticLibraryTarget>("ref_counted");
    setup_grpc(ref_counted);
    ref_counted.Public += atomic, debug_location, gpr_base, grpc_trace, ref_counted_ptr;

    auto &orphanable = p.addTarget<StaticLibraryTarget>("orphanable");
    setup_grpc(orphanable);
    orphanable.Public += debug_location, gpr_base, grpc_trace, ref_counted_ptr;

    auto &grpc_base_c = p.addTarget<StaticLibraryTarget>("grpc_base_c");
    setup_grpc(grpc_base_c);
    grpc_base_c.Public += gpr_base, grpc_trace, inlined_vector, optional, orphanable, ref_counted, "org.sw.demo.madler.zlib-1"_dep;

    auto &grpc_base = p.addTarget<StaticLibraryTarget>("grpc_base");
    setup_grpc(grpc_base);
    grpc_base.Public += grpc_base_c;

    auto &census = p.addTarget<StaticLibraryTarget>("census");
    setup_grpc(census);
    census.Public += grpc_base, grpc_nanopb;

    auto &grpc_client_authority_filter = p.addTarget<StaticLibraryTarget>("grpc_client_authority_filter");
    setup_grpc(grpc_client_authority_filter);
    grpc_client_authority_filter.Public += grpc_base;

    auto &grpc_deadline_filter = p.addTarget<StaticLibraryTarget>("grpc_deadline_filter");
    setup_grpc(grpc_deadline_filter);
    grpc_deadline_filter.Public += grpc_base;

    auto &health_proto = p.addTarget<StaticLibraryTarget>("health_proto");
    setup_grpc(health_proto);
    health_proto.Public += grpc_nanopb;

    auto &grpc_client_channel = p.addTarget<StaticLibraryTarget>("grpc_client_channel");
    setup_grpc(grpc_client_channel);
    grpc_client_channel.Public += gpr_base, grpc_base, grpc_client_authority_filter, grpc_deadline_filter, inlined_vector,
        orphanable, ref_counted, ref_counted_ptr, health_proto;

    auto &grpc_lb_subchannel_list = p.addTarget<StaticLibraryTarget>("grpc_lb_subchannel_list");
    setup_grpc(grpc_lb_subchannel_list);
    grpc_lb_subchannel_list.Public += grpc_base, grpc_client_channel;

    auto &grpc_lb_policy_pick_first = p.addTarget<StaticLibraryTarget>("grpc_lb_policy_pick_first");
    setup_grpc(grpc_lb_policy_pick_first);
    grpc_lb_policy_pick_first.Public += grpc_base, grpc_client_channel, grpc_lb_subchannel_list;

    auto &grpc_lb_policy_round_robin = p.addTarget<StaticLibraryTarget>("grpc_lb_policy_round_robin");
    setup_grpc(grpc_lb_policy_round_robin);
    grpc_lb_policy_round_robin.Public += grpc_lb_subchannel_list;

    auto &grpc_max_age_filter = p.addTarget<StaticLibraryTarget>("grpc_max_age_filter");
    setup_grpc(grpc_max_age_filter);
    grpc_max_age_filter.Public += grpc_base;

    auto &grpc_message_size_filter = p.addTarget<StaticLibraryTarget>("grpc_message_size_filter");
    setup_grpc(grpc_message_size_filter);
    grpc_message_size_filter.Public += grpc_base, grpc_client_channel;

    auto &grpc_resolver_dns_selection = p.addTarget<StaticLibraryTarget>("grpc_resolver_dns_selection");
    setup_grpc(grpc_resolver_dns_selection);
    grpc_resolver_dns_selection.Public += grpc_base;

    auto &grpc_resolver_dns_ares = p.addTarget<StaticLibraryTarget>("grpc_resolver_dns_ares");
    setup_grpc(grpc_resolver_dns_ares);
    grpc_resolver_dns_ares.Public += grpc_base, grpc_client_channel, grpc_address_sorting, grpc_resolver_dns_selection,
        "org.sw.demo.c_ares-1"_dep;

    auto &grpc_resolver_dns_native = p.addTarget<StaticLibraryTarget>("grpc_resolver_dns_native");
    setup_grpc(grpc_resolver_dns_native);
    grpc_resolver_dns_native.Public += grpc_base, grpc_client_channel, grpc_resolver_dns_selection;

    auto &grpc_resolver_fake = p.addTarget<StaticLibraryTarget>("grpc_resolver_fake");
    setup_grpc(grpc_resolver_fake);
    grpc_resolver_fake.Public += grpc_base, grpc_client_channel;

    auto &grpc_resolver_sockaddr = p.addTarget<StaticLibraryTarget>("grpc_resolver_sockaddr");
    setup_grpc(grpc_resolver_sockaddr);
    grpc_resolver_sockaddr.Public += grpc_base, grpc_client_channel;

    auto &grpc_server_backward_compatibility = p.addTarget<StaticLibraryTarget>("grpc_server_backward_compatibility");
    setup_grpc(grpc_server_backward_compatibility);
    grpc_server_backward_compatibility.Public += grpc_base;

    auto &grpc_http_filters = p.addTarget<StaticLibraryTarget>("grpc_http_filters");
    setup_grpc(grpc_http_filters);
    grpc_http_filters.Public += grpc_base;

    auto &grpc_transport_chttp2_alpn = p.addTarget<StaticLibraryTarget>("grpc_transport_chttp2_alpn");
    setup_grpc(grpc_transport_chttp2_alpn);
    grpc_transport_chttp2_alpn.Public += gpr;

    auto &grpc_transport_chttp2 = p.addTarget<StaticLibraryTarget>("grpc_transport_chttp2");
    setup_grpc(grpc_transport_chttp2);
    grpc_transport_chttp2.Public += gpr_base, grpc_base, grpc_http_filters, grpc_transport_chttp2_alpn;

    auto &grpc_transport_chttp2_client_connector = p.addTarget<StaticLibraryTarget>("grpc_transport_chttp2_client_connector");
    setup_grpc(grpc_transport_chttp2_client_connector);
    grpc_transport_chttp2_client_connector.Public += grpc_base, grpc_client_channel, grpc_transport_chttp2;

    auto &grpc_transport_chttp2_client_insecure = p.addTarget<StaticLibraryTarget>("grpc_transport_chttp2_client_insecure");
    setup_grpc(grpc_transport_chttp2_client_insecure);
    grpc_transport_chttp2_client_insecure.Public += grpc_base, grpc_client_channel, grpc_transport_chttp2, grpc_transport_chttp2_client_connector;

    auto &grpc_transport_chttp2_server = p.addTarget<StaticLibraryTarget>("grpc_transport_chttp2_server");
    setup_grpc(grpc_transport_chttp2_server);
    grpc_transport_chttp2_server.Public += grpc_base, grpc_transport_chttp2;

    auto &grpc_transport_chttp2_server_insecure = p.addTarget<StaticLibraryTarget>("grpc_transport_chttp2_server_insecure");
    setup_grpc(grpc_transport_chttp2_server_insecure);
    grpc_transport_chttp2_server_insecure.Public += grpc_base, grpc_transport_chttp2, grpc_transport_chttp2_server;

    auto &grpc_transport_inproc = p.addTarget<StaticLibraryTarget>("grpc_transport_inproc");
    setup_grpc(grpc_transport_inproc);
    grpc_transport_inproc.Public += grpc_base;

    auto &grpc_workaround_cronet_compression_filter = p.addTarget<StaticLibraryTarget>("grpc_workaround_cronet_compression_filter");
    setup_grpc(grpc_workaround_cronet_compression_filter);
    grpc_workaround_cronet_compression_filter.Public += grpc_server_backward_compatibility;

    auto &grpc_common = p.addTarget<StaticLibraryTarget>("grpc_common");
    setup_grpc(grpc_common);
    grpc_common.AutoDetectOptions = false;
    grpc_common.Public += census, grpc_base, grpc_client_authority_filter, grpc_deadline_filter, grpc_lb_policy_pick_first,
        grpc_lb_policy_round_robin, grpc_max_age_filter, grpc_message_size_filter, grpc_resolver_dns_ares, grpc_resolver_dns_native,
        grpc_resolver_fake, grpc_resolver_sockaddr, grpc_server_backward_compatibility, grpc_transport_chttp2_client_insecure,
        grpc_transport_chttp2_server_insecure, grpc_transport_inproc, grpc_workaround_cronet_compression_filter;

    auto &alts_proto = p.addTarget<StaticLibraryTarget>("alts_proto");
    setup_grpc(alts_proto);
    alts_proto.Public += "org.sw.demo.nanopb-0"_dep;

    auto &alts_util = p.addTarget<StaticLibraryTarget>("alts_util");
    setup_grpc(alts_util);
    alts_util.Public += alts_proto, gpr, grpc_base;

    auto &tsi_interface = p.addTarget<StaticLibraryTarget>("tsi_interface");
    setup_grpc(tsi_interface);
    tsi_interface.Public += gpr, grpc_trace;

    auto &grpc_shadow_boringssl = p.addTarget<StaticLibraryTarget>("grpc_shadow_boringssl");
    setup_grpc(grpc_shadow_boringssl);

    auto &alts_frame_protector = p.addTarget<StaticLibraryTarget>("alts_frame_protector");
    setup_grpc(alts_frame_protector);
    alts_frame_protector.Public += gpr, grpc_base, tsi_interface, grpc_shadow_boringssl, "org.sw.demo.openssl.ssl-1.*.*.*"_dep;

    auto &tsi = p.addTarget<StaticLibraryTarget>("tsi");
    setup_grpc(tsi);
    tsi.Public += alts_frame_protector, alts_util, gpr, grpc_base, grpc_transport_chttp2_client_insecure, tsi_interface;

    auto &grpc_secure = p.addTarget<StaticLibraryTarget>("grpc_secure");
    setup_grpc(grpc_secure);
    grpc_secure.Public += alts_util, grpc_base, grpc_transport_chttp2_alpn, tsi;

    auto &grpclb_proto = p.addTarget<StaticLibraryTarget>("grpclb_proto");
    setup_grpc(grpclb_proto);
    grpclb_proto.Public += grpc_nanopb;

    auto &grpc_lb_policy_grpclb_secure = p.addTarget<StaticLibraryTarget>("grpc_lb_policy_grpclb_secure");
    setup_grpc(grpc_lb_policy_grpclb_secure);
    grpc_lb_policy_grpclb_secure.Public += grpc_base, grpc_client_channel, grpc_resolver_fake, grpc_secure, grpclb_proto;

    auto &grpc_transport_chttp2_client_secure = p.addTarget<StaticLibraryTarget>("grpc_transport_chttp2_client_secure");
    setup_grpc(grpc_transport_chttp2_client_secure);
    grpc_transport_chttp2_client_secure.Public += grpc_base, grpc_client_channel, grpc_secure, grpc_transport_chttp2, grpc_transport_chttp2_client_connector;

    auto &grpc_transport_chttp2_server_secure = p.addTarget<StaticLibraryTarget>("grpc_transport_chttp2_server_secure");
    setup_grpc(grpc_transport_chttp2_server_secure);
    grpc_transport_chttp2_server_secure.Public += grpc_base, grpc_secure, grpc_transport_chttp2, grpc_transport_chttp2_server;

    auto &grpc_lb_policy_xds_secure = p.addTarget<StaticLibraryTarget>("grpc_lb_policy_xds_secure");
    setup_grpc(grpc_lb_policy_xds_secure);
    grpc_lb_policy_xds_secure.Public += grpc_base, grpc_secure, grpclb_proto, grpc_client_channel, grpc_resolver_fake;

    auto &grpc = p.addTarget<StaticLibraryTarget>("grpc");
    setup_grpc(grpc);
    grpc.Public += grpc_common, grpc_lb_policy_grpclb_secure, grpc_secure, grpc_transport_chttp2_client_secure,
        grpc_transport_chttp2_server_secure, grpc_lb_policy_xds_secure;

    auto &grpcpp_internal_hdrs_only = p.addTarget<StaticLibraryTarget>("grpcpp_internal_hdrs_only");
    grpcpp_internal_hdrs_only.BazelTargetName = "grpc++_internal_hdrs_only";
    setup_grpc(grpcpp_internal_hdrs_only);
    grpcpp_internal_hdrs_only.Public += gpr_codegen;

    auto &grpcpp_codegen_base = p.addTarget<StaticLibraryTarget>("grpcpp_codegen_base");
    grpcpp_codegen_base.BazelTargetName = "grpc++_codegen_base";
    setup_grpc(grpcpp_codegen_base);
    grpcpp_codegen_base.Public += grpc_codegen, grpcpp_internal_hdrs_only;

    auto &grpcpp_base = p.addTarget<StaticLibraryTarget>("grpcpp_base");
    grpcpp_base.BazelTargetName = "grpc++_base";
    setup_grpc(grpcpp_base);
    grpcpp_base.Public += grpc, grpcpp_codegen_base;

    auto &grpcpp_codegen_base_src = p.addTarget<StaticLibraryTarget>("grpcpp_codegen_base_src");
    grpcpp_codegen_base_src.BazelTargetName = "grpc++_codegen_base_src";
    setup_grpc(grpcpp_codegen_base_src);
    grpcpp_codegen_base_src.Public += grpcpp_codegen_base;

    auto &grpcpp_codegen_proto = p.addTarget<StaticLibraryTarget>("grpcpp_codegen_proto");
    grpcpp_codegen_proto.BazelTargetName = "grpc++_codegen_proto";
    setup_grpc(grpcpp_codegen_proto);
    grpcpp_codegen_proto.Public += grpcpp_codegen_base, grpcpp_config_proto;

    auto &grpcpp = p.addTarget<StaticLibraryTarget>("grpcpp");
    grpcpp.BazelTargetName = "grpc++";
    setup_grpc(grpcpp);
    grpcpp.Public += gpr, grpc, grpcpp_base,
        grpcpp_codegen_base, grpcpp_codegen_base_src, grpcpp_codegen_proto;
}
