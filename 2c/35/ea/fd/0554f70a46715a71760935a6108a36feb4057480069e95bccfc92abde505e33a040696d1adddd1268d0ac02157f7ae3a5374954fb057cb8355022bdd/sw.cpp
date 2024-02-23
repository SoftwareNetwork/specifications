void build(Solution &s)
{
    auto &p = s.addProject("Genivia.REflex", "4.0.1");
    p += Git("https://github.com/Genivia/RE-flex", "v{v}");

    auto &lib = p.addStaticLibrary("lib");
    lib += "include/.*"_rr;
    lib += "lib/.*"_rr;
    lib += "unicode/.*"_rr;

    auto &exe = p.addExecutable("exe");
    exe += "src/.*"_rr;
    exe += lib;
}
