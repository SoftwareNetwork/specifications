void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("Microsoft.LightGBM", "2.2.3");
    t += Git("https://github.com/Microsoft/LightGBM", "v{v}");
    t -= "src/main.cpp";
}
