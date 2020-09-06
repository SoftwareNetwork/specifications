void build(Solution &s)
{
    auto &t = s.addStaticLibrary("arvidn.try_signal", "master");
    t += Git("https://github.com/arvidn/try_signal");

    t += "signal.*"_r;
    t += "try_signal.*"_r;
}
