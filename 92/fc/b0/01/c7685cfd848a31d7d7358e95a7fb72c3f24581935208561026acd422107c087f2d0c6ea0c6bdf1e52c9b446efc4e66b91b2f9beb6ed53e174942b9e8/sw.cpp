void build(Solution &s)
{
    auto &fmt = s.addTarget<LibraryTarget>("fmt", "7.0.1");
    fmt += Git("https://github.com/fmtlib/fmt", "{v}");
    //fmt.ExportAllSymbols = true;

    fmt += cpp11;

    fmt.Private += sw::Shared, "FMT_EXPORT"_d;
    fmt.Public += sw::Shared, "FMT_SHARED"_d;

    if (fmt.getCompilerType() == CompilerType::ClangCl ||
        fmt.getCompilerType() == CompilerType::Clang)
    {
        fmt.Public += "FMT_USE_EXTERN_TEMPLATES=0"_d;
        if (fmt.getCompilerType() == CompilerType::Clang)
            fmt.ExportAllSymbols = true;
    }
}
