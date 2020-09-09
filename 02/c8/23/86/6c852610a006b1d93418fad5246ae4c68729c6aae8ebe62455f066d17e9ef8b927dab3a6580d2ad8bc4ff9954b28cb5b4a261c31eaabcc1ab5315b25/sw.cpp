#pragma sw require header org.sw.demo.google.grpc.cpp.plugin

void build(Solution &s)
{
    auto &p = s.addProject("google.Orbit", "1.52.0");
    p += Git("https://github.com/google/orbit", "{M}.{m}");

    auto add_library = [&p](const String &name, bool orbit = true) -> decltype(auto)
    {
        auto &t = p.addStaticLibrary(name);
        t += cpp17;
        t.setRootDirectory((orbit ? "Orbit" : "") + name);
        t += ".*"_rr;
        t.AllowEmptyRegexes = true;
        t -= ".*Test.cpp"_rr;
        t -= ".*Fuzzer.cpp"_rr;
        t.AllowEmptyRegexes = false;
        return t;
    };

    auto &tp = p.addDirectory("third_party");

    auto &multicore = tp.addStaticLibrary("multicore");
    multicore.setRootDirectory("third_party/multicore/common");
    multicore += ".*"_rr;

    auto &base = add_library("Base");
    base.Public += "org.sw.demo.google.abseil"_dep;
    base.Public += "outcome=outcome_v2"_def;
    base.Public += "org.sw.demo.ned14.outcome"_dep;
    base += "advapi32.lib"_slib;
    base += "Dbghelp.lib"_slib;

    auto &elfutils = add_library("ElfUtils", false);
    elfutils.Public += base;
    //elfutils.Public += "org.sw.demo.llvm_project.llvm.Object"_dep;

    auto &cp = add_library("ClientProtos");
    for (const auto &[p, _] : cp[".*\\.proto"_r])
    {
        ProtobufData d;
        d.public_protobuf = true;
        gen_protobuf_cpp("org.sw.demo.google.protobuf"_dep, cp, p, d);
    }

    auto &gp = add_library("GrpcProtos");
    for (const auto &[p, _] : gp[".*\\.proto"_r])
    {
        ProtobufData d;
        d.public_protobuf = true;
        gen_grpc_cpp("org.sw.demo.google.protobuf"_dep, "org.sw.demo.google.grpc.cpp.plugin"_dep, gp, p, d);
    }
    gp.Public += "org.sw.demo.google.grpc.cpp"_dep;

    auto &core = add_library("Core");
    core.Public += "UNICODE"_def;
    core.Public += base, cp, gp, multicore, elfutils;
    core.Public += "org.sw.demo.Cyan4973.xxHash"_dep;
    core.Public += "org.sw.demo.cameron314.concurrentqueue"_dep;
    core.Public += "org.sw.demo.H_EAL.oqpi-master"_dep;
}
