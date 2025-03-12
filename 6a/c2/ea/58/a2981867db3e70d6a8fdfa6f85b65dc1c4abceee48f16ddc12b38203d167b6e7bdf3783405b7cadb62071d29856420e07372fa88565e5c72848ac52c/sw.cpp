void build(Solution &s)
{
    auto &t = s.addStaticLibrary("mohabouje.WinToast", "1.3.1");
    t += Git("https://github.com/mohabouje/WinToast");
    t.Public += "com.Microsoft.Windows.SDK.winrt"_dep;
    t += "uuid.lib"_slib;
}
