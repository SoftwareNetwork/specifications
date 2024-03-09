void build(Solution &s)
{
    auto &t = s.addTarget<Library>("scintilla", "5.4.3");
    t += RemoteFile("https://www.scintilla.org/scintilla{M}{m}{p}.zip");

    t += cpp17;
    t += "include/.*"_rr;
    t += "src/.*"_rr;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t += "win32/.*\\.h"_rr;
        t += "win32/.*\\.cxx"_rr;
        t += "win32/.*\\.def"_rr;

        t += "imm32.lib"_slib;
        t += "ole32.lib"_slib;
        t += "OleAut32.lib"_slib;
        t += "gdi32.lib"_slib;
        t += "user32.lib"_slib;
        t += "Msimg32.lib"_slib;
        t += "uuid.lib"_slib;
    }
}
