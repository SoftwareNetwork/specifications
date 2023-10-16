void build(Solution &s)
{
    auto &sfml = s.addProject("sfml", "2.6.0");
    sfml += Git("https://github.com/SFML/SFML", "{v}");

    auto add_library = [&s, &sfml](const String &name) -> decltype(auto)
    {
        auto &t = sfml.addTarget<LibraryTarget>(name);
        t +=
            FileRegex("include/SFML/" + name, ".*", true),
            FileRegex("src/SFML/" + name, ".*", false);
        t.AllowEmptyRegexes = true;
        if (t.getBuildSettings().TargetOS.is(OSType::Windows))
            t += FileRegex("src/SFML/" + name + "/Win32", ".*", true);
        t.AllowEmptyRegexes = false;
        t += path("include/SFML/" + name + ".hpp");
        t.Private += sw::Shared, Definition("SFML_" + boost::to_upper_copy(name) + "_EXPORTS");
        t.Public += sw::Static, "SFML_STATIC"_d;
        return t;
    };

    auto &system = add_library("System");
    system += "include/SFML/Config.hpp";
    system += "include/SFML/Main.hpp";
    if (system.getBuildSettings().TargetOS.is(OSType::Windows))
        system.Protected += "winmm.lib"_slib;

    auto &window = add_library("Window");
    window += "include/SFML/OpenGL.hpp";
    window += "include/SFML/GpuPreference.hpp";
    if (window.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        window.Protected += "UNICODE"_def;
        window.Protected +=
            "gdi32.lib"_slib,
            "user32.lib"_slib,
            "Opengl32.lib"_slib,
            "ws2_32.lib"_slib,
            "advapi32.lib"_slib
            ;
    }
    window -= "src/SFML/Window/E[gG][lL].*"_r;
    window.Public += system;

    auto &network = add_library("Network");
    if (network.getBuildSettings().TargetOS.is(OSType::Windows))
        network.Protected += "ws2_32.lib"_slib;
    network.Public += system;

    auto &audio = add_library("Audio");
    audio.Public += system;
    audio += "org.sw.demo.kcat.openal"_dep;
    audio += "org.sw.demo.xiph.vorbis.vorbisfile"_dep;
    audio += "org.sw.demo.xiph.flac.libflac"_dep;

    auto &graphics = add_library("Graphics");
    graphics.Public += window;
    graphics += "org.sw.demo.freetype"_dep;
    graphics += "org.sw.demo.stb.image-master"_dep;
    graphics += "org.sw.demo.stb.image_write-master"_dep;
}
