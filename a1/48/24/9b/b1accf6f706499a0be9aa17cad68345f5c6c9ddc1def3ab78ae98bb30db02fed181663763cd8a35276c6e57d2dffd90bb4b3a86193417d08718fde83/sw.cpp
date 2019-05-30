void build(Solution &s)
{
    auto &t = s.addLibrary("stalkerg.clunk", "master");
    t += Git("https://github.com/stalkerg/clunk");

    t.ApiName = "CLUNKAPI";
    t += ".*\\.h"_rr;
    t +=
        "buffer.cpp",
        "clunk_ex.cpp",
        "context.cpp",
        "distance_model.cpp",
        "kemar.c",
        "logger.cpp",
        "object.cpp",
        "sample.cpp",
        "sdl_ex.cpp",
        "source.cpp",
        "stream.cpp",
        "clunk_c.cpp"
        ;
    t += "NEED_POW10F"_def;
    t += "NEED_LOG2F"_def;
    t.Public += "org.sw.demo.valve.sdl.sdl"_dep;
}
