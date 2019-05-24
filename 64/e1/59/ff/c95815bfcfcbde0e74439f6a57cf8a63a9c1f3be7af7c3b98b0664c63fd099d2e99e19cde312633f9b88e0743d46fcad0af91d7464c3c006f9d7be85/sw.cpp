void build(Solution &s)
{
    auto &t = s.addLibrary("antlr.runtime.cpp", "4.7.2");
    t += Git("https://github.com/antlr/antlr4");
    t.setRootDirectory("runtime/Cpp/runtime");

    t += sw::Shared, "ANTLR4CPP_EXPORTS"_def;
    t.Public += sw::Static, "ANTLR4CPP_STATIC"_def;
}
