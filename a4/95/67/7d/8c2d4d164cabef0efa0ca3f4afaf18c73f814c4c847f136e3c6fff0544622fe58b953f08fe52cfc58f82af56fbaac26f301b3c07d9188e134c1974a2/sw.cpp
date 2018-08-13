void build(Solution &s)
{
    auto &fmt = s.addTarget<LibraryTarget>("fmt", "5.1.0");
    fmt += Git("https://github.com/fmtlib/fmt", "", "master");

    fmt.Private += sw::Shared, "FMT_EXPORT"_d;
    fmt.Public += sw::Shared, "FMT_SHARED"_d;
}

