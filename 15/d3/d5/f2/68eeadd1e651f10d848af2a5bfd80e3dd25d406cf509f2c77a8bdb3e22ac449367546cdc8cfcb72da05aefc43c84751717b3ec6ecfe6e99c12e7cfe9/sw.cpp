void build(Solution &s)
{
    auto &benchmark = s.addTarget<StaticLibraryTarget>("google.benchmark", "1.5.0");
    benchmark += Git("https://github.com/google/benchmark", "v{v}");

    benchmark.CPPVersion = CPPLanguageStandard::CPP11;
}
