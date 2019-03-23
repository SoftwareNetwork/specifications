void build(Solution &s)
{
    auto &p = s.addProject("facebook.incubator", "2019.3.18.0");
    p += Git("https://github.com/facebookincubator/fizz", "v{M}.{m:02}.{p:02}.{t:02}");

    auto &t = p.addTarget<Executable>("fizz");
    //t.CPPVersion = CPPLanguageStandard::CPP17;

    t += "fizz/.*"_rr;
    t -= "fizz/.*test.*"_rr;
    t -= "fizz/.*[Tt]est.*"_rr;

    t += "org.sw.demo.jedisct1.libsodium"_dep;
    //t += "org.sw.demo.boost.variant"_dep;
    t += "org.sw.demo.facebook.folly"_dep;
}
