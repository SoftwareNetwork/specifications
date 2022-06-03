void build(Solution &s)
{
    auto &t = s.addLibrary("Esri.lerc", "3.0.0");
    t += Git("https://github.com/Esri/lerc", "v{M}.{m}{po}");

    t += cpp11;
    t += "src/LercLib/.*"_rr;

    t.Public += "src/LercLib/include"_idir;
    t.Public += "src/LercLib"_idir;
}
