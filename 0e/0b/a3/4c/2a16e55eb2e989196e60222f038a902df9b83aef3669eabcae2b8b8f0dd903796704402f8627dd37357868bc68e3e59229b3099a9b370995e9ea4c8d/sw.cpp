void build(Solution &s)
{
    auto &y = s.addTarget<StaticLibrary>("biojppm.rapidyaml", "master");
    y += Git("https://github.com/biojppm/rapidyaml");
    y.Public += "org.sw.demo.biojppm.c4core"_dep;
}
