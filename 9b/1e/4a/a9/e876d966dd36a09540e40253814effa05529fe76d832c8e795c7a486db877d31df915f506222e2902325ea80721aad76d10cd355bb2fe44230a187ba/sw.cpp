void build(Solution &s)
{
    auto &t = s.addLibrary("Esri.lerc", "2.2.1");
    t += Git("https://github.com/Esri/lerc", "v{M}.{m}{po}");

    t += cpp11;
    t += "include/.*"_rr;
    t += "src/LercLib/.*"_rr;

    t.Public += "include"_idir;
    t.Public += "src/LercLib"_idir;
}
