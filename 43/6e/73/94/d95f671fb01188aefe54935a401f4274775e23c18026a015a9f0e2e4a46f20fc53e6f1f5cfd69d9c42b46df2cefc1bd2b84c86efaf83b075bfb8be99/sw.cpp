void build(Build &b)
{
    auto &upb = b.addStaticLibrary("google.protocolbuffers.upb", "24");
    upb += Git("https://github.com/protocolbuffers/upb", "", "24.x");

    //upb += "cmake/google/.*"_rr;
    upb += "upb/.*"_rr;

    upb -= ".*test.*"_rr;
    upb -= "upb/conform.*"_rr;

    //upb.Public += "cmake"_idir;
    upb.Public += "upb/reflection/stage0"_idir;
    upb.Public += "."_idir;

    upb.Public += "org.sw.demo.google.protocolbuffers.utf8_range-main"_dep;
}
