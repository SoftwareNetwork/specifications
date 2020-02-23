void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("cnjinhao.nana", "1.7.3");
    t += Git("https://github.com/cnjinhao/nana", "v{v}");

    t += cpp17;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t += "gdi32.lib"_slib;
        t += "user32.lib"_slib;
        t += "Shell32.lib"_slib;
        t += "ole32.lib"_slib;
        t += "Comdlg32.lib"_slib;
    }
}
