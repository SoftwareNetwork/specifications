void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("biojppm.rapidyaml", "0.9.0");
    t += Git("https://github.com/biojppm/rapidyaml", "v{v}");
    t.Public += "org.sw.demo.biojppm.c4core"_dep;
}
