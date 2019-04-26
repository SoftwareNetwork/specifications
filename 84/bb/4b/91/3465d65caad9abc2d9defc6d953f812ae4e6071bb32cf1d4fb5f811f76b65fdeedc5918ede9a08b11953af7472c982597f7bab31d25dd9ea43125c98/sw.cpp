void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("DigitalInBlue.Celero", "2.4.0");
    t += Git("https://github.com/DigitalInBlue/Celero", "v{v}");
}
