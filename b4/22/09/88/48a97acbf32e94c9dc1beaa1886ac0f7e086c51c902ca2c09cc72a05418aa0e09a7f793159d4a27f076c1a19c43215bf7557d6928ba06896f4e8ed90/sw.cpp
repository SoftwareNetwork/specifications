void build(Solution &s)
{
    auto &t = s.addLibrary("Naios.function2", "4.1.0");
    t += Git("https://github.com/Naios/function2");
}
