void build(Solution &s)
{
    auto &t = s.addLibrary("TankOs.SFGUI", "0.4.0");
    t += Git("https://github.com/TankOs/SFGUI");

    t += cpp17;

    t += "extlibs/.*"_rr;
    t += "include/.*"_rr;
    t += "src/.*"_rr;

    t.Public += sw::Static, "SFGUI_STATIC"_def;
    t += "SFGUI_EXPORTS"_def;

    t += "extlibs/libELL/include"_idir;
    t.Public += "include"_idir;
    t += "src"_idir;

    t.Public += "org.sw.demo.sfml.Graphics"_dep;
    t += "org.sw.demo.find.opengl-master"_dep;
}
