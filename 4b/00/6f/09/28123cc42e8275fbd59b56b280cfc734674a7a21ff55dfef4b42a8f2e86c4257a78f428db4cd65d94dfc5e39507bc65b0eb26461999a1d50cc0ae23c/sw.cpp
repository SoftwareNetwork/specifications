void build(Solution &s)
{
    auto &t = s.addStaticLibrary("google.gumbo_parser", "0.10.1");
    t += Git("https://github.com/google/gumbo-parser", "v{v}");

    t += "src/.*"_rr;
    t -= "visualc/include/.*"_rr;

    t.Public += "src"_idir;
    if (t.getCompilerType() == CompilerType::MSVC)
        t += "visualc/include"_idir;
}
