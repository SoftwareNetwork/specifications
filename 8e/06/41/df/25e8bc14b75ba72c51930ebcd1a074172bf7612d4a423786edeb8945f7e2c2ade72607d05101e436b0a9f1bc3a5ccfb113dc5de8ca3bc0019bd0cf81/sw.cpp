void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("lemire.simdjson", "0.7.1");
    t += Git("https://github.com/lemire/simdjson", "v{v}");
    t += cpp17;
    if (t.getCompilerType() == CompilerType::MSVC ||
        t.getCompilerType() == CompilerType::ClangCl)
    {
        t.CompileOptions.push_back("-arch:AVX2");
    }
}
