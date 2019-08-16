void build(Build &b)
{
    auto &t = b.addStaticLibrary("google.protocolbuffers.upb.upb", "master");
    t += Git("https://github.com/protocolbuffers/upb");

    t.ImportFromBazel = true;

    /*t += "upb/.*"_r;
    t += "upb/bindings/std.*"_rr;
    t += "upb/json/.*"_r;
    t += "upb/pb/.*"_r;*/
}
