void build(Build &b)
{
    auto &p = b.addProject("google.protocolbuffers.upb", "0.0.1");
    p += Git("https://github.com/protocolbuffers/upb", "", "main");

    auto &utf8_range = p.addStaticLibrary("third_party.utf8_range");
    utf8_range += "third_party/utf8_range/utf8_range.*"_rr;

    auto &port = p.addStaticLibrary("port");
    port += "upb/port_def.inc";
    port += "upb/port_undef.inc";

    auto &upb = p.addStaticLibrary("upb");
    upb += "upb/decode.*"_rr;
    upb += "upb/encode.*"_rr;
    upb += "upb/msg.*"_rr;
    upb -= "upb/msg_test.*"_rr;
    upb += "upb/table.*"_rr;
    upb += "upb/upb.*"_rr;
    upb += utf8_range;
    upb.Public += port;

    auto &reflection = p.addStaticLibrary("reflection");
    reflection += "upb/def.*"_rr;
    reflection += "upb/reflection.*"_rr;
    reflection += "cmake/google/.*"_rr;
    reflection.Public += "."_idir;
    reflection.Public += "cmake"_idir;
    reflection.Public += upb;

    auto &textformat = p.addStaticLibrary("textformat");
    textformat += "upb/text_encode.*"_rr;
    textformat.Public += reflection;
}
