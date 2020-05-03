void build(Solution &s)
{
    auto &t = s.addLibrary("Esri.lerc", "100.9.0.2772");
    t += Git("https://github.com/Esri/lerc", "runtimecore_{v}");

    t += "include/.*"_rr;
    t += "src/LercLib/.*"_rr;

    t.Public += "include"_idir;
    t.Public += "src/LercLib"_idir;
}
