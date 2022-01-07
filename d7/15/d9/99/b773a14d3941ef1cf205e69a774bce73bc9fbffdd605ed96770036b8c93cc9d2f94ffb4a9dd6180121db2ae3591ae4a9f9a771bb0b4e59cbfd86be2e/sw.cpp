void build(Solution &s)
{
    auto &fmt = s.addTarget<LibraryTarget>("fmt", "8.1.1");
    fmt += Git("https://github.com/fmtlib/fmt", "{v}");

    fmt += cpp17;

    fmt -= "src/.*"_rr;
    //fmt -= "src/fmt.cc";
    fmt.Public += "FMT_HEADER_ONLY"_def;

    fmt.Private += sw::Shared, "FMT_EXPORT"_d;
    fmt.Public += sw::Shared, "FMT_SHARED"_d;

    if (fmt.getCompilerType() == CompilerType::GNU ||
        fmt.getCompilerType() == CompilerType::Clang ||
        fmt.getCompilerType() == CompilerType::AppleClang)
    {
        fmt.ExportAllSymbols = true;
    }

    if (fmt.getCompilerType() == CompilerType::ClangCl ||
        fmt.getCompilerType() == CompilerType::Clang ||
        fmt.getCompilerType() == CompilerType::AppleClang)
    {
        fmt.Public += "FMT_USE_EXTERN_TEMPLATES=0"_d;
    }
}
