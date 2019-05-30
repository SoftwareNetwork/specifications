void build(Solution &s)
{
    auto &t = s.addLibrary("whoozle.clunk", "master");
    t += Git("https://github.com/whoozle/clunk");

    t.ApiName = "CLUNKAPI";
    t += "clunk/.*\\.[hc]p?p?"_rr;
    t -= "clunk/clunk_c.cpp";
    t -= "clunk/sse_fft_context.cpp";
    t.Variables["CLUNK_BACKEND_SDL"] = 1;
    //t.Variables["CLUNK_USES_SSE"] = 1;
    t.configureFile("clunk/config.h.in", "clunk/config.h");
    t.Public += "org.sw.demo.valve.sdl.sdl"_dep;
}
