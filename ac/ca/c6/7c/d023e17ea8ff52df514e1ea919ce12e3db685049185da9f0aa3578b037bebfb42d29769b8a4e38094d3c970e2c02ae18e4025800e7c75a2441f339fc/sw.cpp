void build(Solution &s)
{
    auto &c = s.addTarget<StaticLibrary>("biojppm.c4core", "0.2.6");
    c += Git("https://github.com/biojppm/c4core");
    c += cpp11;
    c.ApiName = "C4_API";
}
