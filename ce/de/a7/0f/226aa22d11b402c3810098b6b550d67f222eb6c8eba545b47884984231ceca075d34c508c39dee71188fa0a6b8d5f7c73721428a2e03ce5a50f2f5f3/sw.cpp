void build(Build &b)
{
    auto &p = b.addProject("google.protocolbuffers.upb", "master");
    p += Git("https://github.com/protocolbuffers/upb");

    auto &port = p.addStaticLibrary("port");
    port += "upb/port.c";
    port += "upb/port_def.inc";
    port += "upb/port_undef.inc";

    auto &upb = p.addStaticLibrary("upb");
    upb += "upb/decode.*"_rr;
    upb += "upb/encode.*"_rr;
    upb += "upb/msg.*"_rr;
    upb += "upb/table.c";
    upb += "upb/table.int.h";
    upb += "upb/upb.*"_rr;
    upb.Public += port;
}
