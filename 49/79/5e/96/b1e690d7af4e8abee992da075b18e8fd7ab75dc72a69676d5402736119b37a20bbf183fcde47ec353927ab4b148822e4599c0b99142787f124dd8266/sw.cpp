void build(Solution &s)
{
    auto &p = s.addProject("google.protocolbuffers", "main");
    p += Git("https://github.com/protocolbuffers/utf8_range");

    auto &r = p.addStaticLibrary("utf8_range");
    r += cpp17;
    r +=
        "naive.c",
        "range2-neon.c",
        "range2-sse.c",
        "utf8_range.h"
        ;

    auto &v = p.addStaticLibrary("utf8_validity");
    v += cpp17;
    v +=
        "utf8_validity.cc",
        "utf8_validity.h"
        ;
    v.Public += "org.sw.demo.google.abseil"_dep;
    //v.CompileOptions.push_back("/arch:AVX");
}
