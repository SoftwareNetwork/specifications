void build(Solution &s)
{
    auto &t = s.addLibrary("alandefreitas.matplotplusplus", "1.2.1");
    t += Git("https://github.com/alandefreitas/matplotplusplus", "v{v}");

    t += cpp17;

    t += "MATPLOT_EXPORTS"_api;

    t += "source/matplot/.*"_rr;
    t -= "source/matplot/backend/opengl.*"_rr;

    t += "NOMINMAX"_def;
    t += "gdi32.lib"_slib;
    t += "Shell32.lib"_slib;
    t += "user32.lib"_slib;

    t += "org.sw.demo.olvb.nodesoup-master"_dep;
    t += "org.sw.demo.dtschump.CImg"_dep;

    t.writeFileOnce("matplot/detail/exports.h");
}
