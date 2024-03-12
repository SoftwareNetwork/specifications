void build(Solution &s)
{
    auto &t = s.addStaticLibrary("mohabouje.WinToast", "1.3.0");
    t += Git("https://github.com/mohabouje/WinToast");
}
