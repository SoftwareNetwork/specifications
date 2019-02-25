void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("lemire.simdjson", "master");
    t += Git("https://github.com/lemire/simdjson", "", "{v}");
    t.CPPVersion = CPPLanguageStandard::CPP17;
    if (s.Settings.Native.CompilerType == CompilerType::MSVC ||
        s.Settings.Native.CompilerType == CompilerType::ClangCl)
    {
        t.CompileOptions.push_back("-arch:AVX2");
    }
}
