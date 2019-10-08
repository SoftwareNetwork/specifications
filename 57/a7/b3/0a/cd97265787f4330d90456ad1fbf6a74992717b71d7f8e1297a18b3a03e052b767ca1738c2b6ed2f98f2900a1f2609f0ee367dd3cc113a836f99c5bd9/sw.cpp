void build(Solution &s)
{
    auto &p = s.addProject("WohlSoft.AudioCodecs", "master");
    p += Git("https://github.com/WohlSoft/AudioCodecs");

    auto &t = p.addStaticLibrary("libtimidity");
    t.setSourceDirectory("libtimidity-sdl");
    t += "org.sw.demo.valve.sdl.sdl"_dep;
}
