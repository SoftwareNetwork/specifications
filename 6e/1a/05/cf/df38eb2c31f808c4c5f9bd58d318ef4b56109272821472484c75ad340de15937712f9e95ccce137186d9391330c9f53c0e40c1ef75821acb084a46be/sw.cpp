void build(Solution &s)
{
    auto &c = s.addTarget<StaticLibrary>("biojppm.c4core", "0.2.7");
    c += Git("https://github.com/biojppm/c4core");
    c += cpp11;
    c.ApiName = "C4_API";
    c.Public += "org.sw.demo.biojppm.debugbreak-master"_dep;
}
