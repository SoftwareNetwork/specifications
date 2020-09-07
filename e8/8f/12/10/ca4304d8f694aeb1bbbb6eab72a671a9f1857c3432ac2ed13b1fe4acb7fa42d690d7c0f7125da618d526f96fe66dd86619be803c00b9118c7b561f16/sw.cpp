void build(Solution &s)
{
    auto &fmt = s.addTarget<LibraryTarget>("fmt", "7.0.3");
    fmt += Git("https://github.com/fmtlib/fmt", "{v}");

    fmt += cpp14;

    fmt.Private += sw::Shared, "FMT_EXPORT"_d;
    fmt.Public += sw::Shared, "FMT_SHARED"_d;

    if (fmt.getCompilerType() == CompilerType::GNU ||
        fmt.getCompilerType() == CompilerType::Clang)
    {
        fmt.ExportAllSymbols = true;
    }
}
