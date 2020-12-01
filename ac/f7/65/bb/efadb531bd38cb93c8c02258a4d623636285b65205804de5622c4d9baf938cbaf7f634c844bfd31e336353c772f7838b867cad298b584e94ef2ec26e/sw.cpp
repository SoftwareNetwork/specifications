void build(Solution &s)
{
    auto &fmt = s.addTarget<LibraryTarget>("fmt", "7.1.3");
    fmt += Git("https://github.com/fmtlib/fmt", "{v}");

    fmt += cpp14;

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
