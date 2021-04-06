void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("liballeg.allegro5", "5.2.7.0");
    t += Git("https://github.com/liballeg/allegro5");

    t.setChecks("x");

    t += "include/.*"_rr;
    t -= "src/.*"_rr;
    t += "src/.*"_r;

    t.Public += sw::Static, "ALLEGRO_STATICLINK"_def;
    t += "ALLEGRO_SRC"_def;
    t += "ALLEGRO_LIB_BUILD"_def;

    t.Variables["ALLEGRO_LITTLE_ENDIAN"] = 1;

    //t += "src/sdl/.*"_rr;
    //t.Public += "org.sw.demo.valve.sdl"_dep;
    //t.Variables["ALLEGRO_SDL"] = 1;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t += "src/win/.*"_rr;
        //t += "dxguid.lib"_slib;
        t.Variables["ALLEGRO_MSVC"] = 1;
        t.Variables["ALLEGRO_CFG_D3D"] = 1;
        t.Variables["ALLEGRO_CFG_XINPUT"] = 1;
    }
    else
    {
        t += "src/unix/.*"_rr;
    }
    if (t.getBuildSettings().TargetOS.Type == OSType::Linux)
        t.Variables["ALLEGRO_LINUX"] = 1;
    if (t.getBuildSettings().TargetOS.Type == OSType::Macos)
    {
        t += "src/macosx/.*"_rr;
        t.Variables["ALLEGRO_DARWIN"] = 1;
    }

    t.configureFile("include/allegro5/platform/alplatf.h.cmake", "allegro5/platform/alplatf.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("x");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("fseeki64");
    s.checkFunctionExists("ftelli64");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("time.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/io.h");
    s.checkIncludeExists("sal.h");
    s.checkIncludeExists("stdbool.h");

    for (auto &check : s.all)
        check->Prefixes.insert("ALLEGRO_");
}
