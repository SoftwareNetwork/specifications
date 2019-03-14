void build(Solution &s)
{
    auto &t = s.addTarget<Library>("scintilla", "4.1.4");
    t += RemoteFile("https://www.scintilla.org/scintilla{M}{m}{p}.zip");

    t.CPPVersion = CPPLanguageStandard::CPP17;
    t += "include/.*"_rr;
    t += "lexers/.*"_rr;
    t += "lexlib/.*"_rr;
    t += "src/.*"_rr;

    t += "lexlib"_id;

    if (s.Settings.TargetOS.Type == OSType::Windows)
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
    }
}
