void build(Solution &s)
{
    auto &c = s.addTarget<StaticLibrary>("biojppm.c4core", "master");
    c += Git("https://github.com/biojppm/c4core");
    c.ApiName = "C4_API";
}
