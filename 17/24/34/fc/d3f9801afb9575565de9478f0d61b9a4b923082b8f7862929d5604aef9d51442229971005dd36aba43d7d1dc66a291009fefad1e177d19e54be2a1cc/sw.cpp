void build(Solution &s)
{
    auto &t = s.addStaticLibrary("alandefreitas.matplotplusplus", "1.0.1");
    t += Git("https://github.com/alandefreitas/matplotplusplus", "v{v}");

    t += cpp17;

    t += "source/matplot/.*"_rr;
    t -= "source/matplot/backend/opengl.*"_rr;

    t += "NOMINMAX"_def;
    t += "gdi32.lib"_slib;
    t += "Shell32.lib"_slib;
    t += "user32.lib"_slib;

    t += "org.sw.demo.olvb.nodesoup-master"_dep;
    t += "org.sw.demo.dtschump.CImg"_dep;
}
