void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("google.robotstxt", "master");
    t += Git("https://github.com/google/robotstxt");
    t += "robots\\..*"_rr;
    t.Public += "org.sw.demo.google.abseil-master"_dep;
}
